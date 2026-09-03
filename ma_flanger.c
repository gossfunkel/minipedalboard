#include "ma_flanger.h"

ma_flanger_config ma_flanger_config_init (
    ma_uint32 channels, 
    ma_uint32 sampleRate, 
    float rate, 
    float depth, 
    float dryWet) {

}

ma_result ma_flanger_init (
    const ma_flanger_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_flanger *pFlanger) {
    if (pFlanger == NULL) return MA_INVALID_ARGS;

    memset(pFlanger, 0, sizeof *pFlanger);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    if (pConfig->rate < MIN_DELAY || pConfig->rate > MAX_DELAY) return MA_INVALID_ARGS;
    if (pConfig->depth < 0.f      || pConfig->depth  > 1.f) return MA_INVALID_ARGS;
    if (pConfig->wetDry < 0.f     || pConfig->wetDry > 1.f) return MA_INVALID_ARGS;

    pFlanger->config = *pConfig;
    pFlanger->bufferSizeInFrames = (ma_uint32)(0.001f * MAX_DELAY * pConfig->sampleRate);
    pFlanger->cursor = 0;

    pFlanger->pBuffer = (float *)ma_malloc((size_t)(pFlanger->bufferSizeInFrames * 
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

}

ma_flanger_node_config ma_flanger_node_config_init (
    ma_uint32 channels, 
    ma_uint32 sampleRate, 
    float rate, 
    float depth, 
    float dryWet) {

}

ma_result ma_flanger_node_init (
    ma_node_graph *pNodeGraph, 
    const ma_flanger_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_flanger_node *pFlangerNode) {
    if (pFlangerNode == NULL) return MA_INVALID_ARGS;

    memset(pFlangerNode, 0, sizeof *pFlangerNode);

    ma_result result = ma_flanger_init(pConfig->flangerConfig, pAllocationCallbacks, pFlangerNode->flanger);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &ma_flanger_node_vtable;
    baseConfig.pInputChannels = &pConfig->flangerConfig.channels;
    baseConfig.pOutputChannels = &pConfig->flangerConfig.channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pFlangerNode->baseNode);
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

void ma_flanger_set_wet_dry(ma_flanger *pFlanger, float value) {
    if (value < 0.f || value > 1.f) return;
    pFlanger->config.dryWet = value;
}

float ma_flanger_get_wet_dry(const ma_flanger *pFlanger) {
    return pFlanger->config.dryWet;
}
