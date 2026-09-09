#include "miniaudio.h"

#define MIN_FADE_TIME 0.1f
#define MAX_FADE_TIME 20000.f

#define MIN_MAX_AMP 1.f
#define MAX_MAX_AMP 100000.f

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    float fade_time;
    float threshold;
    float max_amp;
} ma_normalize_config;

typedef struct {
    ma_normalize_config config;
    float peak_level;
    float time_on;
    float time_off;
} ma_normalize;

typedef struct {
    ma_node_base baseNode;
    ma_normalize normalize;
} ma_normalize_node;

typedef struct {
    ma_node_config nodeConfig;
    ma_normalize_config normalizeConfig;
} ma_normalize_node_config;

ma_normalize_config ma_normalize_config_init(ma_uint32 channels, ma_uint32 sampleRate, float fade_time, float threshold, float max_amp);
ma_result ma_normalize_init(
    const ma_normalize_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_normalize *pNormalize
);
void ma_normalize_uninit(ma_normalize *pNormalize, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result ma_normalize_process_pcm_frames(
    ma_normalize *pNormalize,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);

ma_normalize_node_config ma_normalize_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float fade_time, float threshold, float max_amp);
ma_result ma_normalize_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_normalize_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_normalize_node *pNormalizeNode
);
void ma_normalize_node_uninit(ma_normalize_node *pNormalizeNode, const ma_allocation_callbacks *pAllocationCallbacks);
void ma_normalize_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void ma_normalize_set_fade_time(ma_normalize *pNormalize, float value);
float ma_normalize_get_fade_time(const ma_normalize *pNormalize);
void ma_normalize_set_threshold(ma_normalize *pNormalize, float value);
float ma_normalize_get_threshold(const ma_normalize *pNormalize);
void ma_normalize_set_max_amp(ma_normalize *pNormalize, float value);
float ma_normalize_get_max_amp(const ma_normalize *pNormalize);

static ma_node_vtable ma_normalize_node_vtable = {
    ma_normalize_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};