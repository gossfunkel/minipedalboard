#include "miniaudio.h"

// NOTE we don't need reverb, miniaudio includes one as an extra

MA_API ma_reverb_config ma_reverb_config_init(ma_uint32 channels, ma_uint32 sampleRate, float reverbTime, float density, float earlyReflections, float wetDry);

MA_API ma_result ma_reverb_init(const ma_reverb_config *pConfig, ma_allocation_callbacks *pAllocationCallbacks, ma_reverb *reverb);
MA_API void ma_reverb_uninit(ma_reverb *pReverb, ma_allocation_callbacks *pAllocationCallbacks);
MA_API ma_result ma_reverb_process_pcm_frames(ma_reverb *pReverb, void *pFramesOut, const void *pFramesIn, ma_uint32 frameCount);
MA_API void ma_reverb_set_reverb_time(ma_reverb *pReverb, float value);
MA_API float ma_reverb_get_reverb_time(const ma_reverb *pReverb);
MA_API void ma_reverb_set_density(ma_reverb *pReverb, float value);
MA_API float ma_reverb_get_density(const ma_reverb *pReverb);
MA_API void ma_reverb_set_early_reflections(ma_reverb *pReverb, float value);
MA_API float ma_reverb_get_early_reflections(const ma_reverb *pReverb);
MA_API void ma_reverb_set_dry_wet(ma_reverb *pReverb, float value);
MA_API float ma_reverb_get_dry_wet(const ma_reverb *pReverb);

MA_API ma_reverb_node_config ma_reverb_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float reverbTime, float density, float earlyReflections, float wetDry)

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    float reverbTime;
    float density;
    float earlyReflections;
    float wetDry;
} ma_reverb_config;

typedef struct {
    ma_reverb_config config;
    ma_uint32 cursor; // always at or before playhead
    ma_uint32 bufferSizeInFrames;
    float *pBuffer;
} ma_reverb;

typedef struct {
    ma_node_config nodeConfig;
    ma_reverb_config reverb;
} ma_reverb_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_reverb reverb;
} ma_reverb_node;

MA_API ma_result ma_reverb_init(const ma_reverb_config *pConfig, ma_allocation_callbacks *pAllocationCallbacks, ma_reverb *pReverb) {
    if (pReverb == NULL) return MA_INVALID_ARGS;

    MA_ZERO_OBJECT(pReverb);

    if (pConfig == NULL) return MA_INVALID_ARGS;
    if (pConfig.reverbTime < 0.f) return MA_INVALID_ARGS;
    if (pConfig.wetDry < 0.f || pConfig.wetDry > 1.f) return MA_INVALID_ARGS;

    pReverb->config = pConfig;
    pReverb->bufferSizeInFrames = pConfig->reverbTime * pConfig->sampleRate;
    pReverb->cursor = 0;

    pReverb->pBuffer = (float *)ma_malloc((size_t)(pReverb->bufferSizeInFrames * ma_get_bytes_per_frame(ma_format_f32, pConfig->channels)), pAllocationCallbacks);
    if (pReverb->pBuffer == NULL) return MA_OUT_OF_MEMORY;

    ma_silence_pcm_frames(pReverb->pBuffer, pReverb->bufferSizeInFrames, ma_format_f32, pConfig->channels);

    return MA_SUCCESS;
}

MA_API void ma_reverb_uninit(ma_reverb *pReverb, ma_allocation_callbacks *pAllocationCallbacks) {
    if (pReverb == NULL) return;

    ma_free(pReverb->pBuffer, pAllocationCallbacks);
}

MA_API ma_result ma_reverb_process_pcm_frames(ma_reverb *pReverb, void *pFramesOut, const void *pFramesIn, ma_uint32 frameCount) {
    
}

MA_API ma_reverb_config ma_reverb_config_init(ma_uint32 channels, ma_uint32 sampleRate, float reverbTime, float density, float earlyReflections, float wetDry) {
    ma_reverb_config config;

    MA_ZERO_OBJECT(&config);
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.reverbTime = reverbTime;
    config.density = density;
    config.earlyReflections = earlyReflections;
    config.wetDry = wetDry;
}

MA_API ma_reverb_node_config ma_reverb_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float reverbTime, float density, float earlyReflections, float wetDry) {
    
}