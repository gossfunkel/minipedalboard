#include "p3d_distort.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

ma_result p3d_distort_init(const p3d_distort_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, p3d_distort *pDistort) {
    if (pDistort == NULL) return MA_INVALID_ARGS;

    memset(pDistort, 0, sizeof *pDistort);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    /*{
        //p3d_distort_node_config config;
        p3d_distort_config conf;
        conf.nodeConfig = ma_node_config_init();
        // default settings: 1 channel, 44100 samplerate, threshold at .8/1., ratio 1:2, 50/50 drywet
        conf.distort = p3d_distort_config_init(1, 44100, .8f, 2.f, .5f);
    } else {*/
        if (pConfig->threshold < 0.f || pConfig->threshold > 1.f) return MA_INVALID_ARGS;
        if (pConfig->ratio < MIN_RATIO || pConfig->ratio > MAX_RATIO) return MA_INVALID_ARGS;
        if (pConfig->wetDry < 0.f || pConfig->wetDry > 1.f) return MA_INVALID_ARGS;

        pDistort->config = *pConfig;
    //}

    return MA_SUCCESS;
}

void p3d_distort_uninit(p3d_distort *pDistort, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pDistort == NULL) return;
}

ma_result p3d_distort_process_pcm_frames(p3d_distort *pDistort, void *pFramesOut, const void *pFramesIn, ma_uint32 frameCount) {
    float *pFramesOutF32 = (float *)pFramesOut;
    const float *pFramesInF32 = (const float *)pFramesIn;
    ma_uint32 channels = pDistort->config.channels;
    ma_uint32 iFrame = 0;

    if (pDistort == NULL || pFramesOut == NULL || pFramesIn == NULL) return MA_INVALID_ARGS;

    float wetDry = pDistort->config.wetDry;
    float dryWet = 1.f - wetDry;

    float threshold = pDistort->config.threshold;
    float ratio = pDistort->config.ratio;

    while (iFrame < channels * frameCount)
        for (ma_uint32 iChannel = 0; iChannel < channels; iChannel++) {
            pFramesOutF32[iFrame] = (fmin(threshold, pFramesInF32[iFrame]) + 
                                    (fmax(pFramesInF32[iFrame], threshold) - threshold) / ratio) * wetDry 
                                    + pFramesInF32[iFrame] * dryWet;
            iFrame += 1;
        }
    return MA_SUCCESS;
}

p3d_distort_config p3d_distort_config_init(ma_uint32 channels, ma_uint32 sampleRate, float threshold, float ratio, float wetDry) {
    p3d_distort_config config;

    memset(&config, 0, sizeof config);
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.threshold = threshold;
    config.ratio = ratio;
    config.wetDry = wetDry;

    return config;
}

// TODO attack/decay

p3d_distort_node_config p3d_distort_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float threshold, float ratio, float wetDry) {
    p3d_distort_node_config config;

    config.nodeConfig = ma_node_config_init();
    config.distort = p3d_distort_config_init(channels, sampleRate, threshold, ratio, wetDry);

    return config;
}

ma_result p3d_distort_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_distort_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_distort_node *pDistortNode) {
    ma_result result;

    if (pDistortNode == NULL) return MA_INVALID_ARGS;

    memset(pDistortNode, 0, sizeof *pDistortNode);

    result = p3d_distort_init(&pConfig->distort, pAllocationCallbacks, &pDistortNode->distort);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig  = pConfig->nodeConfig;
    baseConfig.vtable          = &p3d_distort_node_vtable;
    baseConfig.pInputChannels  = &pConfig->distort.channels;
    baseConfig.pOutputChannels = &pConfig->distort.channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pDistortNode->baseNode);
    if (result != MA_SUCCESS) {
        p3d_distort_uninit(&pDistortNode->distort, pAllocationCallbacks);
    }

    return result;
}

void p3d_distort_node_uninit(p3d_distort_node *pDistortNode, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pDistortNode == NULL) return;

    // base node uninitialises first
    ma_node_uninit(pDistortNode, pAllocationCallbacks);
    p3d_distort_uninit(&pDistortNode->distort, pAllocationCallbacks);
}

void p3d_distort_node_process_pcm_frames(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut, ma_uint32 *pFrameCountOut) {
    p3d_distort_node *pDistortNode = (p3d_distort_node *)pNode;

    (void)pFrameCountIn;

    p3d_distort_process_pcm_frames(&pDistortNode->distort, ppFramesOut[0], ppFramesIn[0], *pFrameCountOut);
}

// getters and setters

void p3d_distort_set_threshold(p3d_distort *pDistort, float value) {
    if (value < 0.f) return;
    pDistort->config.threshold = value;
}

float p3d_distort_get_threshold(const p3d_distort *pDistort) {
    return pDistort->config.threshold;
}

void p3d_distort_set_ratio(p3d_distort *pDistort, float value) {
    if (value > MAX_RATIO || value < MIN_RATIO) return;
    pDistort->config.ratio = value;
}

float p3d_distort_get_ratio(const p3d_distort *pDistort) {
    return pDistort->config.ratio;
}
void p3d_distort_set_wet_dry(p3d_distort *pDistort, float value) {
    if (value > 1.f || value < 0.f) return;
    pDistort->config.wetDry = value;
}

float p3d_distort_get_wet_dry(const p3d_distort *pDistort) {
    return pDistort->config.ratio;
}