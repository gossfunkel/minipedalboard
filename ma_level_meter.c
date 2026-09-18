#include "ma_level_meter.h"
#include <string.h>
#include <math.h>

// EFFECT:

ma_level_meter_config ma_level_meter_config_init (
        ma_uint32 channels, 
        ma_uint32 sampleRate) {
    ma_level_meter_config config;
    memset(&config, 0, sizeof config);
    config.channels   = channels;
    config.sampleRate = sampleRate;
    return config;
}

ma_result ma_level_meter_init (
        const ma_level_meter_config *pConfig, 
        const ma_allocation_callbacks *pAllocationCallbacks, 
        ma_level_meter *pLevelMeter) {
    if (pLevelMeter == NULL) return MA_INVALID_ARGS;
    memset(pLevelMeter, 0, sizeof *pLevelMeter);

    if (pConfig == NULL) return MA_INVALID_ARGS;

    size_t data_size = pLevelMeter->config.channels * sizeof (ma_level_data);
    pLevelMeter->config = *pConfig;
    pLevelMeter->pData = (ma_level_data **)ma_malloc(data_size, pAllocationCallbacks);
    memset(pLevelMeter->pData, 0, data_size);

    return MA_SUCCESS;
}

void ma_level_meter_uninit (
        ma_level_meter *pLevelMeter, 
        const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pLevelMeter == NULL) return;
    ma_free(pLevelMeter->pData, pAllocationCallbacks);
}

ma_result ma_level_meter_process_pcm_frames (
        ma_level_meter *pLevelMeter,
        void *pFramesOut,
        const void *pFramesIn,
        ma_uint32 frameCount) {
    if (pLevelMeter == NULL || pFramesOut == NULL || pFramesIn == NULL) return MA_INVALID_ARGS;
    float *pFramesOutF32 = (float *)pFramesOut;
    const float *pFramesInF32 = (const float *)pFramesIn;
    ma_uint32 channels = pLevelMeter->config.channels;

    float sum_sq[channels] = {};

    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++) {
        for (ma_uint32 iChannel = 0; iChannel < channels; iChannel++) {
        	pLevelMeter->pData[iChannel]->peak = fmax(pFramesInF32[iChannel], pLevelMeter->pData[iChannel]->peak);
        	sum_sq[iChannel] += pFramesInF32[iChannel]*pFramesInF32[iChannel];
        	pFramesOutF32[iChannel] = pFramesInF32[iChannel];
        }
        pFramesInF32 += channels;
    }
    for (ma_uint32 iChannel = 0; iChannel < channels; iChannel++) {
    	pLevelMeter->pData[iChannel]->rms = sqrt(sum_sq[iChannel] / (float)frameCount);
    }
    return MA_SUCCESS;
}

// NODE:

static ma_node_vtable ma_level_meter_node_vtable = {
    ma_level_meter_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};

ma_level_meter_node_config ma_level_meter_node_config_init (
        ma_uint32 channels, ma_uint32 sampleRate) {
    ma_level_meter_node_config config;

    config.nodeConfig = ma_node_config_init();
    config.levelMeterConfig = 
        ma_level_meter_config_init(channels, sampleRate);

    return config;
}

ma_result ma_level_meter_node_init (
        ma_node_graph *pNodeGraph, 
        const ma_level_meter_node_config *pConfig, 
        const ma_allocation_callbacks *pAllocationCallbacks, 
        ma_level_meter_node *pLevelMeterNode) {
    if (pLevelMeterNode == NULL) return MA_INVALID_ARGS;

    memset(pLevelMeterNode, 0, sizeof *pLevelMeterNode);

    ma_result result = 
        ma_level_meter_init(&pConfig->levelMeterConfig, pAllocationCallbacks, &pLevelMeterNode->levelMeter);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &ma_level_meter_node_vtable;
    baseConfig.pInputChannels = &pConfig->levelMeterConfig.channels;
    baseConfig.pOutputChannels = &pConfig->levelMeterConfig.channels;

    result = 
        ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pLevelMeterNode->baseNode);
    if (result != MA_SUCCESS) ma_level_meter_uninit(&pLevelMeterNode->levelMeter, pAllocationCallbacks);

    return result;
}

void ma_level_meter_node_uninit (
        ma_level_meter_node *pLevelMeterNode, 
        const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pLevelMeterNode == NULL) return;
    ma_node_uninit(pLevelMeterNode, pAllocationCallbacks);
    ma_level_meter_uninit(&pLevelMeterNode->levelMeter, pAllocationCallbacks);
}

void ma_level_meter_node_process_pcm_frames (
        ma_node *pNode,
        const float **ppFramesIn,
        ma_uint32 *pFrameCountIn,
        float **ppFramesOut,
        ma_uint32 *pFrameCountOut) {
    ma_level_meter_node *pLevelMeterNode = (ma_level_meter_node *)pNode;
    (void)pFrameCountIn;
    ma_level_meter_process_pcm_frames(&pLevelMeterNode->levelMeter, ppFramesOut[0], ppFramesIn[0], *pFrameCountOut);
}