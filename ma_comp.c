#include "ma_comp.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*ma_result p3d_compress_init(const p3d_compress_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, p3d_compress *pCompress) {
    if (pCompress == NULL) return MA_INVALID_ARGS;

    memset(pCompress, 0, sizeof *pCompress);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    /
    {
        //p3d_compress_node_config config;
        p3d_compress_config conf;
        conf.nodeConfig = ma_node_config_init();
        // default settings: 1 channel, 44100 samplerate, threshold at .8/1., ratio 1:2, 50/50 drywet
        conf.compress = p3d_compress_config_init(1, 44100, .8f, 2.f, .5f);
    } else { 
    /
        if (pConfig->threshold < 0.f || pConfig->threshold > 1.f) return MA_INVALID_ARGS;
        if (pConfig->ratio < MIN_RATIO || pConfig->ratio > MAX_RATIO) return MA_INVALID_ARGS;
        if (pConfig->wetDry < 0.f || pConfig->wetDry > 1.f) return MA_INVALID_ARGS;

        pCompress->config = *pConfig;
        pCompress->bufferSizeInFrames = pConfig->sampleRate/20;
        //pCompress->cursor = 0;
        //pCompress->pBuffer = (float *)ma_malloc((size_t)(pCompress->bufferSizeInFrames 
        //                                        * ma_get_bytes_per_frame(ma_format_f32, pConfig->channels)), 
        //                                        pAllocationCallbacks);
        //if (pCompress->pBuffer == NULL) return MA_OUT_OF_MEMORY;
    //}

    return MA_SUCCESS;
}

void p3d_compress_uninit(p3d_compress *pCompress, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pCompress == NULL) return;
}*/

/*ma_result p3d_compress_process_pcm_frames(p3d_compress *pCompress, void *pFramesOut, const void *pFramesIn, ma_uint32 frameCount) {

            if (pFramesInF32[iFrame] > threshold) {
                pCompress->onTime += spf;
                pCompress->offTime = release;
            } else {
                // reset timers on first release frame
                if (pCompress->onTime != 0.f) {
                    pCompress->offTime = 0.f;
                    pCompress->onTime = 0.f;
                }
                pCompress->offTime += spf;
            }
            comp = 1.f + (ratio - 1.f) * fmin(pCompress->onTime/attack, 1.f) 
                     + (ratio - 1.f) * fmax(0.f, 1.f - pCompress->offTime/release);
            pFramesOutF32[iFrame] = dryWet * pFramesInF32[iFrame] +
                                    wetDry * pFramesInF32[iFrame] / comp; 
        }
    return MA_SUCCESS;
}*/

/*p3d_compress_config p3d_compress_config_init(
        ma_uint32 channels, 
        ma_uint32 sampleRate, 
        float threshold,
        float ratio,
        float attack,
        float release,
        float wetDry) {
    p3d_compress_config config;

    return config;
}*/

// TODO attack/decay

p3d_compress_node_config p3d_compress_node_config_init(
        ma_uint32 channels, 
        ma_uint32 sampleRate, 
        float threshold,
        float ratio,
        float attack,
        float release,
        float wetDry) {
    p3d_compress_node_config config;

    memset(&config, 0, sizeof config);

    config.nodeConfig = ma_node_config_init();
    /*config.compress = p3d_compress_config_init(
        channels, sampleRate, threshold, ratio, attack, release, wetDry
    );*/
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.bufferSizeInFrames = sampleRate/20;
    config.threshold = threshold;
    config.ratio = ratio;
    config.attack = attack;
    config.release = release;
    config.wetDry = wetDry;

    return config;
}

ma_result p3d_compress_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_compress_node *pCompressNode) {
    ma_result result;

    if (pCompressNode == NULL) return MA_INVALID_ARGS;

    memset(pCompressNode, 0, sizeof *pCompressNode);

    //result = p3d_compress_init(&pConfig->compress, pAllocationCallbacks, &pCompressNode->compress);
    //if (result != MA_SUCCESS) return result;
    if (pConfig->threshold < 0.f || pConfig->threshold > 1.f) return MA_INVALID_ARGS;
    if (pConfig->ratio < MIN_RATIO || pConfig->ratio > MAX_RATIO) return MA_INVALID_ARGS;
    if (pConfig->wetDry < 0.f || pConfig->wetDry > 1.f) return MA_INVALID_ARGS;

    pCompressNode->config = *pConfig;
    pCompressNode->onTime = 0.f;
    pCompressNode->offTime = MAX_RELEASE;

    ma_node_config baseConfig  = pConfig->nodeConfig;
    baseConfig.vtable          = &p3d_compress_node_vtable;
    baseConfig.pInputChannels  = &pConfig->channels;
    baseConfig.pOutputChannels = &pConfig->channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pCompressNode->baseNode);
   /* if (result != MA_SUCCESS) {
        p3d_compress_uninit(&pCompressNode->compress, pAllocationCallbacks);
    }*/

    return result;
}

void p3d_compress_node_uninit(p3d_compress_node *pCompressNode, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pCompressNode == NULL) return;

    // base node uninitialises first
    ma_node_uninit(pCompressNode, pAllocationCallbacks);
    //p3d_compress_uninit(&pCompressNode->compress, pAllocationCallbacks);
}

void p3d_compress_node_process_pcm_frames(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut, ma_uint32 *pFrameCountOut) {
    p3d_compress_node *pCompress = (p3d_compress_node *)pNode;

    if (pCompress == NULL || ppFramesOut == NULL || ppFramesIn == NULL) return;

    ma_uint32 frameCount = *pFrameCountIn;
    
    float *pFramesOutF32 = (float *)*ppFramesOut;
    const float *pFramesInF32 = (const float *)*ppFramesIn;
    ma_uint32 channels = pCompress->config.channels;
    ma_uint32 iFrame = 0;

    float wetDry = pCompress->config.wetDry;
    float dryWet = 1.f - wetDry;

    float threshold = pCompress->config.threshold;
    float ratio = pCompress->config.ratio;
    float attack = pCompress->config.attack;
    float release = pCompress->config.release;
    float comp;
    float spf = 1.f/pCompress->config.sampleRate;
    float peakVal;
    ma_uint32 buffSize = pCompress->config.bufferSizeInFrames;

    while (iFrame < channels * (frameCount - buffSize))
        for (ma_uint32 iChannel = 0; iChannel < channels; iChannel++) {
            // maybe need to set engineConfig->periodSizeInFrames to fit window size?
            peakVal = 0.f;
            ma_uint32 bloc_end = (*pFrameCountIn > iFrame + buffSize) ? iFrame + buffSize : *pFrameCountIn;
            for (ma_uint32 frame = iFrame; frame < bloc_end; frame++)
                if (pFramesInF32[frame] > peakVal) peakVal = pFramesInF32[frame];

            while (iFrame < bloc_end) {
                if (peakVal > threshold) {
                    pCompress->onTime += spf;
                    pCompress->offTime = release;
                } else {
                    // reset timers on first release frame
                    if (pCompress->onTime != 0.f) {
                        pCompress->offTime = 0.f;
                        pCompress->onTime = 0.f;
                    }
                    pCompress->offTime += spf;
                }
                comp = 1.f + (ratio - 1.f) * fmin(pCompress->onTime/attack, 1.f) 
                     + (ratio - 1.f) * fmax(0.f, 1.f - pCompress->offTime/release);
                pFramesOutF32[iFrame] = dryWet * pFramesInF32[iFrame] +
                                       wetDry * pFramesInF32[iFrame] / comp;
                iFrame++;
            }
        }

    *pFrameCountIn = iFrame; *pFrameCountOut = iFrame;
}

// getters and setters

void p3d_compress_node_set_threshold(p3d_compress_node *pCompress, float value) {
    if (value < 0.f) return;
    pCompress->config.threshold = value;
}

float p3d_compress_node_get_threshold(const p3d_compress_node *pCompress) {
    return pCompress->config.threshold;
}

void p3d_compress_node_set_ratio(p3d_compress_node *pCompress, float value) {
    if (value > MAX_RATIO || value < MIN_RATIO) return;
    pCompress->config.ratio = value;
}

float p3d_compress_node_get_ratio(const p3d_compress_node *pCompress) {
    return pCompress->config.ratio;
}

void p3d_compress_node_set_attack(p3d_compress_node *pCompress, float value) {
    if (value < 0.f || value > MAX_ATTACK) return;
    pCompress->config.attack = value;
}

float p3d_compress_node_get_attack(const p3d_compress_node *pCompress) {
    return pCompress->config.attack;
}

void p3d_compress_node_set_release(p3d_compress_node *pCompress, float value) {
    if (value < 0.f || value > MAX_RELEASE) return;
    pCompress->config.release = value;
}

float p3d_compress_node_get_release(const p3d_compress_node *pCompress) {
    return pCompress->config.release;
}

void p3d_compress_node_set_wet_dry(p3d_compress_node *pCompress, float value) {
    if (value > 1.f || value < 0.f) return;
    pCompress->config.wetDry = value;
}

float p3d_compress_node_get_wet_dry(const p3d_compress_node *pCompress) {
    return pCompress->config.ratio;
}