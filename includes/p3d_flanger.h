#include "miniaudio.h"

#define MIN_DELAY 0.f
#define MAX_DELAY 5.f

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    float rate;
    float depth;
    float dryWet;
} p3d_flanger_config;

typedef struct {
    p3d_flanger_config config;
    ma_uint32 cursor;
    ma_uint32 bufferSizeInFrames;
    float *pBuffer;
} p3d_flanger;

typedef struct {
    ma_node_config nodeConfig;
    p3d_flanger_config flangerConfig;
} p3d_flanger_node_config;

typedef struct {
    ma_node_base baseNode;
    p3d_flanger flanger;
} p3d_flanger_node;

p3d_flanger_config p3d_flanger_config_init(ma_uint32 channels, ma_uint32 sampleRate, float rate, float depth, float dryWet);
ma_result p3d_flanger_init(
    const p3d_flanger_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_flanger *pFlanger
);
void p3d_flanger_uninit(p3d_flanger *pFlanger, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result p3d_flanger_process_pcm_frames(
    p3d_flanger *pFlanger,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);

p3d_flanger_node_config p3d_flanger_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float rate, float depth, float dryWet);
ma_result p3d_flanger_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_flanger_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_flanger_node *pFlangerNode
);
void p3d_flanger_node_uninit(p3d_flanger_node *pFlangerNode, const ma_allocation_callbacks *pAllocationCallbacks);
void p3d_flanger_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void p3d_flanger_set_rate(p3d_flanger *pFlanger, float value);
float p3d_flanger_get_rate(const p3d_flanger *pFlanger);
void p3d_flanger_set_depth(p3d_flanger *pFlanger, float value);
float p3d_flanger_get_depth(const p3d_flanger *pFlanger);
void p3d_flanger_set_wet_dry(p3d_flanger *pFlanger, float value);
float p3d_flanger_get_wet_dry(const p3d_flanger *pFlanger);

static ma_node_vtable p3d_flanger_node_vtable = {
    p3d_flanger_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};