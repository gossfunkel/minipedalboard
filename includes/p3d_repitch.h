#include "miniaudio.h"

typedef struct {
    ma_node_config nodeConfig;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 bufferSizeInFrames;
    float pitch;
    float parameter;
    float wetDry;
} p3d_repitch_node_config;

typedef struct {
    ma_node_base baseNode;
    p3d_repitch_node_config config;
} p3d_repitch_node;

p3d_compress_node_config p3d_repitch_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    ma_uint32 mode,
    float pitch,
    float parameter,
    float wetDry
);
ma_result p3d_repitch_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_compress_node *pCompressNode
);
void p3d_repitch_node_uninit(p3d_repitch_node *pRepitchNode, const ma_allocation_callbacks *pAllocationCallbacks);
void p3d_repitch_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void p3d_repitch_node_set_pitch(p3d_repitch_node *pRepitch, float value);
float p3d_repitch_node_get_pitch(const p3d_repitch_node *pRepitch);
void p3d_repitch_node_set_parameter(p3d_repitch_node *pRepitch, float value);
float p3d_repitch_node_get_parameter(const p3d_repitch_node *pRepitch);
void p3d_repitch_node_set_wet_dry(p3d_repitch_node *pRepitch, float value);
float p3d_repitch_node_get_wet_dry(const p3d_repitch_node *pRepitch);

static ma_node_vtable p3d_repitch_node_vtable = {
    p3d_repitch_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};