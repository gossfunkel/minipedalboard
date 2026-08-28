#include "ma_comp.h"
#include <math.h>

ma_result p3d_compress_init(const p3d_compress_config *pConfig, ma_allocation_callbacks *pAllocationCallbacks, p3d_compress *pCompress) {
    if (pCompress == NULL) return MA_INVALID_ARGS;

    MA_ZERO_OBJECT(pCompress);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    if (pConfig->threshold < 0.f || pConfig->threshold > 1.f) return MA_INVALID_ARGS;
    if (pConfig->ratio < MIN_RATIO || pConfig->ratio > MAX_RATIO) return MA_INVALID_ARGS;
    if (pConfig->wetDry < 0.f || pConfig->wetDry > 1.f) return MA_INVALID_ARGS;

    pCompress->config = *pConfig;

    return MA_SUCCESS;
}

void p3d_compress_uninit(p3d_compress *pCompress, ma_allocation_callbacks *pAllocationCallbacks) {
    if (pCompress == NULL) return;
}

ma_result p3d_compress_process_pcm_frames(p3d_compress *pCompress, void *pFramesOut, const void *pFramesIn, ma_uint32 frameCount) {
    float *pFramesOutF32 = (float *)pFramesOut;
    const float *pFramesInF32 = (const float *)pFramesIn;
    ma_uint32 channels = pCompress->config.channels;
    ma_uint32 iFrame = 0;

    if (pCompress == NULL || pFramesOut == NULL || pFramesIn == NULL) return MA_INVALID_ARGS;

    float wetDry = pCompress->config.wetDry;
    float dryWet = 1.f - wetDry;

    float threshold = pCompress->config.threshold;
    float ratio = pCompress->config.ratio;

    while (iFrame < channels * frameCount)
        for (ma_uint32 iChannel = 0; iChannel < channels; iChannel++)
            pFramesOutF32[iFrame] = (fmin(threshold, pFramesInF32[iFrame]) + 
                                    (fmax(pFramesInF32[iFrame], threshold) - threshold) / ratio) * wetDry 
                                    + pFramesInF32[iFrame++] * dryWet;
}

p3d_compress_config p3d_compress_config_init(ma_uint32 channels, ma_uint32 sampleRate, float threshold, float ratio, float wetDry) {
    p3d_compress_config config;

    MA_ZERO_OBJECT(&config);
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.threshold = threshold;
    config.ratio = ratio;
    config.wetDry = wetDry;
}

static ma_node_vtable p3d_compress_node_vtable = {
    p3d_compress_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};

p3d_compress_node_config p3d_compress_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float threshold, float ratio, float wetDry) {
    ma_compress_node_config config;

    config.nodeConfig = ma_node_config_init();
    config.nodeConfig.vtable = &p3d_compress_node_vtable;
    config.nodeConfig.channelsIn = channels;
    config.nodeConfig.channelsOut = channels;
    config.compress = ma_compress_config_init(channels, sampleRate, threshold, ratio, wetDry);

    return config;
}

ma_result p3d_compress_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_compress_node *pCompressNode) {
    ma_result result;

    if (pCompressNode == NULL) return MA_INVALID_ARGS;

    MA_ZERO_OBJECT(pCompressNode);
    result = p3d_compress_init(&pConfig->compress, pAllocationCallbacks, &pCompressNode->compress);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &p3d_compress_node_vtable;
    baseConfig.channelsIn = &pConfig->compress.channels;
    baseConfig.channelsOut = &pConfig->compress.channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pCompressNode->baseNode);
    if (result != MA_SUCCESS) {
        p3d_compress_uninit(&pCompressNode->compress, pAllocationCallbacks);
    }

    return result;
}

void p3d_compress_node_uninit(p3d_compress_node *pCompressNode, ma_allocation_callbacks *pAllocationCallbacks) {
    if (pCompressNode == NULL) return;

    // base node uninitialises first
    ma_node_uninit(pCompressNode, pAllocationCallbacks);
    p3d_compress_uninit(&pCompressNode->compress, pAllocationCallbacks);
}

void p3d_compress_node_process_pcm_frames(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut, ma_uint32 *pFrameCountOut) {
    p3d_compress_node *pCompressNode = (p3d_compress_node *)pNode;

    (void)pFrameCountIn;

    p3d_compress_process_pcm_frames(&pCompressNode->compress, ppFramesOut[0], ppFramesIn[0], *pFrameCountOut);
}

// getters and setters

void p3d_compress_set_threshold(p3d_compress *pCompress, float value) {
    if (value < 0.f) return;
    pCompress->config.threshold = value;
}

float p3d_compress_get_threshold(const p3d_compress *pCompress) {
    return pCompress->config.threshold;
}

void p3d_compress_set_ratio(p3d_compress *pCompress, float value) {
    if (value > MAX_RATIO || value < MIN_RATIO) return;
    pCompress->config.ratio = value;
}

float p3d_compress_get_ratio(const p3d_compress *pCompress) {
    return pCompress->config.ratio;
}
void p3d_compress_set_wet_dry(p3d_compress *pCompress, float value) {
    if (value > 1.f || value < 0.f) return;
    pCompress->config.wetDry = value;
}

float p3d_compress_get_wet_dry(const p3d_compress *pCompress) {
    return pCompress->config.ratio;
}