#include "ma_flanger.h"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define TAU (M_PI * 2.)

#define secsToFrames(seconds, sampleRate) (ma_uint32)((seconds) * (sampleRate))

ma_flanger_config ma_flanger_config_init (
        ma_uint32 channels, 
        ma_uint32 sampleRate, 
        float rate, 
        float depth, 
        float dryWet) {
    ma_flanger_config config;
    memset(&config, 0, sizeof config);

    config.channels   = channels;
    config.sampleRate = sampleRate;
    config.rate       = rate;
    config.depth      = depth;
    config.dryWet     = dryWet;

    return config;
}

ma_result ma_flanger_init (
        const ma_flanger_config *pConfig, 
        const ma_allocation_callbacks *pAllocationCallbacks, 
        ma_flanger *pFlanger) {
    if (pFlanger == NULL) return MA_INVALID_ARGS;

    memset(pFlanger, 0, sizeof *pFlanger);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    if (pConfig->rate < MIN_RATE   || pConfig->rate > MAX_RATE)  return MA_INVALID_ARGS;
    if (pConfig->depth < MIN_DELAY || pConfig->depth > MAX_DELAY) return MA_INVALID_ARGS;
    if (pConfig->dryWet < 0.f      || pConfig->dryWet > 1.f)       return MA_INVALID_ARGS;

    pFlanger->config = *pConfig;
    pFlanger->bufferSizeInFrames = secsToFrames(MAX_DELAY, pConfig->sampleRate);
    pFlanger->cursor = 0;
    pFlanger->timer = 0.;
    pFlanger->currentDelayInFrames = 1;

    pFlanger->pBuffer = 
        (float *)ma_malloc((size_t)(pFlanger->bufferSizeInFrames * 
        ma_get_bytes_per_frame(ma_format_f32, pConfig->channels)), pAllocationCallbacks);

    ma_silence_pcm_frames(pFlanger->pBuffer, pFlanger->bufferSizeInFrames, ma_format_f32, pConfig->channels);

    return MA_SUCCESS;
}

void ma_flanger_uninit (
        ma_flanger *pFlanger, 
        const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pFlanger == NULL) return;
    ma_free(pFlanger->pBuffer, pAllocationCallbacks);
}

ma_result ma_flanger_process_pcm_frames (
        ma_flanger *pFlanger,
        void *pFramesOut,
        const void *pFramesIn,
        ma_uint32 frameCount) {
    if (pFlanger == NULL || pFramesOut == NULL || pFramesIn == NULL) return MA_INVALID_ARGS;
    float *pFramesOutF32 = (float *)pFramesOut;
    const float *pFramesInF32 = (const float *)pFramesIn;
    ma_uint32 channels = pFlanger->config.channels;

    /* 
     * each frame should interpolate the closest frames
     * if we only modify some frames, the signal will be unevenly stretched
     * so all frames must undergo some interpolation other than those which
     *  exactly alias to the phase-altered signal
     * 
     * play rate in buffer = original sample rate
     * ratio bFrames/inFrames is the depth (relative speed of buffer to input)
            e.g. half speed buffer -> 180 phase diff at period. 5s (fps/2) behind at 1s. 
                 at 48000fps, 24000 frames behind at .5s
                 so of the 24000 played frames, 12000 are absolutely unaligned
                 2 output frames per 3 input frames
            e.g. 1ms depth & 1hz rate -> at .5s, buffer is 1ms delayed (fps/1000 frames delay)
                 at 48000fps, 48 frames behind at .5s
                 so of the 24000 played frames, 23,952 are absolutely unaligned
                 1 output frame per 500 input frames?
     * buffer speed determined by depth
     * buffer speed inversion point determined by rate

     use ma_resampler - it allows realtime resetting of ratio with ma_resampler_set_rate_ratio()
     or ma_linear_resampler_interpolate_frame_f32()
     note: uses ma_mix_f32_fast
     */
    
    ma_uint32 hDepthMaxFrame = channels * secsToFrames(pFlanger->config.depth, pFlanger->config.sampleRate)/2;
    double period = 1. / (double)pFlanger->config.rate;
    double spf = 1. / (double)pFlanger->config.sampleRate;
    //float wetDry = 1.f - pFlanger->config.dryWet;


    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++) {
        for (ma_uint32 iChannel = 0; iChannel < channels; ++iChannel) {
            pFramesOutF32[iChannel] = 
                pFlanger->pBuffer[pFlanger->cursor * channels + iChannel] * pFlanger->config.dryWet
                + pFramesInF32[iChannel];// * wetDry;
            pFlanger->pBuffer[pFlanger->cursor * channels + iChannel] = pFramesInF32[iChannel];
        }
        
        pFlanger->timer = fmod(pFlanger->timer + spf, period);
        size_t delayFrames = (ma_uint32)(-cos(TAU * pFlanger->timer/period) * (double)hDepthMaxFrame) + hDepthMaxFrame;
        pFlanger->currentDelayInFrames = (delayFrames <= 0) ? 1 : delayFrames;
        pFlanger->cursor = (pFlanger->cursor + 1) % pFlanger->currentDelayInFrames;
        //pFlanger->cursor = (pFlanger->cursor + 1) % pFlanger->bufferSizeInFrames;
        pFramesInF32 += channels;
        pFramesOutF32 += channels;
    }

    return MA_SUCCESS;
}

ma_flanger_node_config ma_flanger_node_config_init (
        ma_uint32 channels, 
        ma_uint32 sampleRate, 
        float rate, 
        float depth, 
        float dryWet) {
    ma_flanger_node_config config;

    config.nodeConfig = ma_node_config_init();
    config.flangerConfig = 
        ma_flanger_config_init(channels, sampleRate, rate, depth, dryWet);

    return config;
}

ma_result ma_flanger_node_init (
        ma_node_graph *pNodeGraph, 
        const ma_flanger_node_config *pConfig, 
        const ma_allocation_callbacks *pAllocationCallbacks, 
        ma_flanger_node *pFlangerNode) {
    if (pFlangerNode == NULL) return MA_INVALID_ARGS;

    memset(pFlangerNode, 0, sizeof *pFlangerNode);

    ma_result result = 
        ma_flanger_init(&pConfig->flangerConfig, pAllocationCallbacks, &pFlangerNode->flanger);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &ma_flanger_node_vtable;
    baseConfig.pInputChannels = &pConfig->flangerConfig.channels;
    baseConfig.pOutputChannels = &pConfig->flangerConfig.channels;

    result = 
        ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pFlangerNode->baseNode);
    if (result != MA_SUCCESS) ma_flanger_uninit(&pFlangerNode->flanger, pAllocationCallbacks);

    return result;
}

void ma_flanger_node_uninit (
        ma_flanger_node *pFlangerNode, 
        const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pFlangerNode == NULL) return;
    ma_node_uninit(pFlangerNode, pAllocationCallbacks);
    ma_flanger_uninit(&pFlangerNode->flanger, pAllocationCallbacks);
}

void ma_flanger_node_process_pcm_frames (
        ma_node *pNode,
        const float **ppFramesIn,
        ma_uint32 *pFrameCountIn,
        float **ppFramesOut,
        ma_uint32 *pFrameCountOut) {
    ma_flanger_node *pFlangerNode = (ma_flanger_node *)pNode;
    (void)pFrameCountIn;
    ma_flanger_process_pcm_frames(&pFlangerNode->flanger, ppFramesOut[0], ppFramesIn[0], *pFrameCountOut);
}

void ma_flanger_set_rate(ma_flanger *pFlanger, float value) {
    if (value < MIN_DELAY || value > MAX_DELAY) return;
    pFlanger->config.rate = value;
}

float ma_flanger_get_rate(const ma_flanger *pFlanger) {
    return pFlanger->config.rate;
}

void ma_flanger_set_depth(ma_flanger *pFlanger, float value) {
    pFlanger->config.depth = value;
}

float ma_flanger_get_depth(const ma_flanger *pFlanger) {
    return pFlanger->config.depth;
}

void ma_flanger_set_dry_wet(ma_flanger *pFlanger, float value) {
    if (value < 0.f || value > 1.f) return;
    pFlanger->config.dryWet = value;
}

float ma_flanger_get_dry_wet(const ma_flanger *pFlanger) {
    return pFlanger->config.dryWet;
}
