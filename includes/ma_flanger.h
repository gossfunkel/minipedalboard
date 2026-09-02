#include "miniaudio.h"

#define MIN_DELAY 0.f
#define MAX_DELAY 5.f

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    float rate;
    float depth;
    float dryWet;
} ma_flanger_config;

typedef struct {
    ma_flanger_config config;
    ma_uint32 cursor;
    ma_uint32 bufferSizeInFrames;
    float *pBuffer;
} ma_flanger;

typedef struct {
    ma_node_config nodeConfig;
    ma_flanger_config flangerConfig;
} ma_flanger_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_flanger flanger;
} ma_flanger_node;

ma_flanger_config ma_flanger_config_init(ma_uint32 channels, ma_uint32 sampleRate, float rate, float depth, float dryWet);
ma_result ma_flanger_init(
    const ma_flanger_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_flanger *pFlanger
);
void ma_flanger_uninit(ma_flanger *pFlanger, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result ma_flanger_process_pcm_frames(
    ma_flanger *pFlanger,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);

ma_flanger_node_config ma_flanger_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float rate, float depth, float dryWet);
ma_result ma_flanger_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_flanger_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_flanger_node *pFlangerNode
);
void ma_flanger_node_uninit(ma_flanger_node *pFlangerNode, const ma_allocation_callbacks *pAllocationCallbacks);
void ma_flanger_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void ma_flanger_set_rate(ma_flanger *pFlanger, float value);
float ma_flanger_get_rate(const ma_flanger *pFlanger);
void ma_flanger_set_depth(ma_flanger *pFlanger, float value);
float ma_flanger_get_depth(const ma_flanger *pFlanger);
void ma_flanger_set_wet_dry(ma_flanger *pFlanger, float value);
float ma_flanger_get_wet_dry(const ma_flanger *pFlanger);

static ma_node_vtable ma_flanger_node_vtable = {
    ma_flanger_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};