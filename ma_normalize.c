#include "ma_normalize.h"
#include <string.h>
#include <math.h>
#include <stdbool.h>

static ma_node_vtable ma_normalize_node_vtable = {
    ma_normalize_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};

ma_normalize_config ma_normalize_config_init(ma_uint32 channels, ma_uint32 sampleRate, float fade_time, float threshold, float max_amp) {
    ma_normalize_config config;
    memset(&config, 0, sizeof config);

    config.channels   = channels;
    config.sampleRate = sampleRate;
    config.fade_time  = fade_time;
    config.threshold  = threshold;
    config.max_amp    = max_amp;

    return config;
}

ma_result ma_normalize_init(
    const ma_normalize_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_normalize *pNormalize
) {
    if (pNormalize == NULL) return MA_INVALID_ARGS;

    memset(pNormalize, 0, sizeof *pNormalize);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    if (pConfig->fade_time < MIN_FADE_TIME || pConfig->fade_time > MAX_FADE_TIME) return MA_INVALID_ARGS;
    if (pConfig->threshold < 0.f 		   || pConfig->threshold > 1.f) 		  return MA_INVALID_ARGS;
    if (pConfig->max_amp < MIN_MAX_AMP 	   || pConfig->max_amp > MAX_MAX_AMP) 	  return MA_INVALID_ARGS;

    pNormalize->config 			   = *pConfig;
    pNormalize->peak_level 		   = 0.f;
    pNormalize->time_on 		   = 0.f;
    pNormalize->time_off 		   = pConfig->fade_time / 1000.f;

    return MA_SUCCESS;
}

void ma_normalize_uninit(ma_normalize *pNormalize, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pNormalize == NULL) return;
}

ma_result ma_normalize_process_pcm_frames(
    ma_normalize *pNormalize,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
) {
    if (pNormalize == NULL || pFramesOut == NULL || pFramesIn == NULL) return MA_INVALID_ARGS;
    float *pFramesOutF32 = (float *)pFramesOut;
    const float *pFramesInF32 = (const float *)pFramesIn;

    //float fade_in;
    //float fade_out;
    // milliseconds per frame (frame length in milliseconds)
    float mspf = 1000.f/pNormalize->config.sampleRate;

    //float fade_time_s = pNormalize->config.fade_time / 1000.f;

    bool peak_detected;
    for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame += pNormalize->config.channels) {
        peak_detected = false;
        for (ma_uint32 iChannel = 0; iChannel < pNormalize->config.channels; ++iChannel) {
        	if (pFramesInF32[iFrame + iChannel] > pNormalize->config.threshold) {
        		peak_detected = true;
        		pNormalize->peak_level = 
        			fmax(pNormalize->peak_level, abs(pFramesInF32[iFrame + iChannel]));
        	}
        	pNormalize->current_gain = 
        		fmax(1.f, fmin(1.f/pNormalize->peak_level, pNormalize->config.max_amp));
        	pNormalize->current_gain = (pNormalize->time_on > 0.f) 		?
        		1.f + 		 fmin(pNormalize->time_on  / .1f, 1.f) 
        					   * (pNormalize->current_gain  - 1.f) 	:
        		1.f + (1.f - fmin(pNormalize->time_off / .1f, 1.f)) 
        					   * (pNormalize->current_gain  - 1.f);
            pFramesOutF32[iFrame + iChannel] = pFramesInF32[iFrame + iChannel] * pNormalize->current_gain;
        }
        if (peak_detected || (0.f < pNormalize->time_on && pNormalize->time_on < pNormalize->config.fade_time + .1f)) {
        	pNormalize->time_off = 0.f;
        	pNormalize->time_on += mspf;
        } else {
        	pNormalize->time_on = 0.f;
        	pNormalize->time_off += mspf;
        	pNormalize->peak_level *= fmax(0.f, fmin(1.f - pNormalize->time_off/0.1f, 1.f));
        }
    }

    return MA_SUCCESS;
}

ma_normalize_node_config ma_normalize_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float fade_time, float threshold, float max_amp) {
    ma_normalize_node_config config;

    config.nodeConfig = ma_node_config_init();
    config.normalizeConfig = 
        ma_normalize_config_init(channels, sampleRate, fade_time, threshold, max_amp);

    return config;
}

ma_result ma_normalize_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_normalize_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_normalize_node *pNormalizeNode
) {
    if (pNormalizeNode == NULL) return MA_INVALID_ARGS;

    memset(pNormalizeNode, 0, sizeof *pNormalizeNode);

    ma_result result = 
        ma_normalize_init(&pConfig->normalizeConfig, pAllocationCallbacks, &pNormalizeNode->normalize);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &ma_normalize_node_vtable;
    baseConfig.pInputChannels = &pConfig->normalizeConfig.channels;
    baseConfig.pOutputChannels = &pConfig->normalizeConfig.channels;

    result = 
        ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pNormalizeNode->baseNode);
    if (result != MA_SUCCESS) ma_normalize_uninit(&pNormalizeNode->normalize, pAllocationCallbacks);

    return result;
}

void ma_normalize_node_uninit(ma_normalize_node *pNormalizeNode, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pNormalizeNode == NULL) return;
    ma_node_uninit(pNormalizeNode, pAllocationCallbacks);
    ma_normalize_uninit(&pNormalizeNode->normalize, pAllocationCallbacks);
}

void ma_normalize_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
) {
    ma_normalize_node *pNormalizeNode = (ma_normalize_node *)pNode;
    (void)pFrameCountIn;
    ma_normalize_process_pcm_frames(&pNormalizeNode->normalize, ppFramesOut[0], ppFramesIn[0], *pFrameCountOut);
}

void ma_normalize_set_fade_time(ma_normalize *pNormalize, float value) {
    if (value < MIN_FADE_TIME || value > MAX_FADE_TIME) return;
    pNormalize->config.fade_time = value;
}
float ma_normalize_get_fade_time(const ma_normalize *pNormalize) {
	return pNormalize->config.fade_time;
}

void ma_normalize_set_threshold(ma_normalize *pNormalize, float value) {
    if (value < 0.f || value > 1.f) return;
	pNormalize->config.threshold = value;
}
float ma_normalize_get_threshold(const ma_normalize *pNormalize) {
	return pNormalize->config.threshold;
}

void ma_normalize_set_max_amp(ma_normalize *pNormalize, float value) {
    if (value < MIN_MAX_AMP || value > MAX_MAX_AMP) return;
    pNormalize->config.max_amp = value;
}
float ma_normalize_get_max_amp(const ma_normalize *pNormalize) {
	return pNormalize->config.max_amp;
}
