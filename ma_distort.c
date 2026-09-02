#include "ma_distort.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <stdlib.h>

float process_clip(float drive, float factor, float signal) {
    (void)factor;
    return fmin(drive * signal, 1.f);
}

float process_tanh(float drive, float factor, float signal) {
    (void)factor;
    // TODO swap for LUT/fast map 
    return tanh(drive * signal);
}

float process_sin(float drive, float factor, float signal) {
    return signal - factor * sin(M_PI * signal);
}

ma_result ma_distort_init(const ma_distort_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, ma_distort *pDistort) {
    if (pDistort == NULL) return MA_INVALID_ARGS;

    memset(pDistort, 0, sizeof *pDistort);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    /*{
        //ma_distort_node_config config;
        ma_distort_config conf;
        conf.nodeConfig = ma_node_config_init();
        // default settings: 1 channel, 44100 samplerate, threshold at .8/1., ratio 1:2, 50/50 drywet
        conf.distort = ma_distort_config_init(1, 44100, .8f, 2.f, .5f);
    } else {*/
        if (pConfig->drive < MIN_DRIVE || pConfig->drive > MAX_DRIVE) return MA_INVALID_ARGS;
        if (pConfig->bias < -LIM_BIAS || pConfig->bias > LIM_BIAS) return MA_INVALID_ARGS;
        if (pConfig->wetDry < 0.f || pConfig->wetDry > 1.f) return MA_INVALID_ARGS;

        pDistort->config = *pConfig;
    //}

    return MA_SUCCESS;
}

void ma_distort_uninit(ma_distort *pDistort, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pDistort == NULL) return;
}

ma_result ma_distort_process_pcm_frames(ma_distort *pDistort, void *pFramesOut, const void *pFramesIn, ma_uint32 frameCount) {
    float *pFramesOutF32 = (float *)pFramesOut;
    const float *pFramesInF32 = (const float *)pFramesIn;
    ma_uint32 channels = pDistort->config.channels;
    ma_uint32 iFrame = 0;

    if (pDistort == NULL || pFramesOut == NULL || pFramesIn == NULL) return MA_INVALID_ARGS;

    float wetDry = pDistort->config.wetDry;
    float dryWet = 1.f - wetDry;

    float drive = pDistort->config.drive;
    float bias = pDistort->config.bias;
    float factor = pDistort->config.factor;

    while (iFrame < channels * frameCount)
        for (ma_uint32 iChannel = 0; iChannel < channels; iChannel++) {
            pFramesOutF32[iFrame] = pDistort->config.transfer_fn(drive, factor, bias + pFramesInF32[iFrame]) * wetDry 
                                    + pFramesInF32[iFrame] * dryWet;
            iFrame += 1;
        }
    return MA_SUCCESS;
}

ma_distort_config ma_distort_config_init(
        ma_uint32 channels,
        ma_uint32 sampleRate,
        float (*transfer_fn)(float, float, float),
        float drive,
        float factor,
        float bias,
        float wetDry) {
    ma_distort_config config;

    memset(&config, 0, sizeof config);
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.transfer_fn = transfer_fn;
    config.drive = drive;
    config.factor = factor;
    config.bias = bias;
    config.wetDry = wetDry;

    return config;
}

// TODO env

ma_distort_node_config ma_distort_node_config_init(
        ma_uint32 channels,
        ma_uint32 sampleRate,
        ma_uint32 mode,
        float drive,
        float factor,
        float bias,
        float wetDry) {
    ma_distort_node_config config;

    config.nodeConfig = ma_node_config_init();
    switch (mode) {
    case DISTORT_MODE_CLIP:
        config.distort = ma_distort_config_init(channels, sampleRate, process_clip, drive, factor, bias, wetDry);
        break;
    case DISTORT_MODE_TANH:
        config.distort = ma_distort_config_init(channels, sampleRate, process_tanh, drive, factor, bias, wetDry);
        break;
    case DISTORT_MODE_SIN:
        config.distort = ma_distort_config_init(channels, sampleRate, process_sin, drive, factor, bias, wetDry);
        break;
    }

    return config;
}

ma_result ma_distort_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_distort_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_distort_node *pDistortNode) {
    ma_result result;

    if (pDistortNode == NULL) return MA_INVALID_ARGS;

    memset(pDistortNode, 0, sizeof *pDistortNode);

    result = ma_distort_init(&pConfig->distort, pAllocationCallbacks, &pDistortNode->distort);
    if (result != MA_SUCCESS) return result;

    ma_node_config baseConfig  = pConfig->nodeConfig;
    baseConfig.vtable          = &ma_distort_node_vtable;
    baseConfig.pInputChannels  = &pConfig->distort.channels;
    baseConfig.pOutputChannels = &pConfig->distort.channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pDistortNode->baseNode);
    if (result != MA_SUCCESS) {
        ma_distort_uninit(&pDistortNode->distort, pAllocationCallbacks);
    }

    return result;
}

void ma_distort_node_uninit(ma_distort_node *pDistortNode, const ma_allocation_callbacks *pAllocationCallbacks) {
    if (pDistortNode == NULL) return;

    // base node uninitialises first
    ma_node_uninit(pDistortNode, pAllocationCallbacks);
    ma_distort_uninit(&pDistortNode->distort, pAllocationCallbacks);
}

void ma_distort_node_process_pcm_frames(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut, ma_uint32 *pFrameCountOut) {
    ma_distort_node *pDistortNode = (ma_distort_node *)pNode;

    (void)pFrameCountIn;

    ma_distort_process_pcm_frames(&pDistortNode->distort, ppFramesOut[0], ppFramesIn[0], *pFrameCountOut);
}

// getters and setters

void ma_distort_set_drive(ma_distort *pDistort, float value) {
    if (value < MIN_DRIVE || value > MAX_DRIVE) return;
    pDistort->config.drive = value;
}

float ma_distort_get_drive(const ma_distort *pDistort) {
    return pDistort->config.drive;
}

void ma_distort_set_factor(ma_distort *pDistort, float value) {
    pDistort->config.factor = value;
}

float ma_distort_get_factor(const ma_distort *pDistort) {
    return pDistort->config.factor;
}

void ma_distort_set_bias(ma_distort *pDistort, float value) {
    if (value < -LIM_BIAS || value > LIM_BIAS) return;
    pDistort->config.bias = value;
}

float ma_distort_get_bias(const ma_distort *pDistort) {
    return pDistort->config.bias;
}

void ma_distort_set_wet_dry(ma_distort *pDistort, float value) {
    if (value > 1.f || value < 0.f) return;
    pDistort->config.wetDry = value;
}

float ma_distort_get_wet_dry(const ma_distort *pDistort) {
    return pDistort->config.wetDry;
}