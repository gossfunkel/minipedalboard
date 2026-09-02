#include "miniaudio.h"

typedef struct {
    ma_node_config nodeConfig;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 bufferSizeInFrames;
    float pitch;
    float parameter;
    float wetDry;
} ma_repitch_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_repitch_node_config config;
} ma_repitch_node;

ma_compress_node_config ma_repitch_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    ma_uint32 mode,
    float pitch,
    float parameter,
    float wetDry
);
ma_result ma_repitch_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_compress_node *pCompressNode
);
void ma_repitch_node_uninit(ma_repitch_node *pRepitchNode, const ma_allocation_callbacks *pAllocationCallbacks);
void ma_repitch_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void ma_repitch_node_set_pitch(ma_repitch_node *pRepitch, float value);
float ma_repitch_node_get_pitch(const ma_repitch_node *pRepitch);
void ma_repitch_node_set_parameter(ma_repitch_node *pRepitch, float value);
float ma_repitch_node_get_parameter(const ma_repitch_node *pRepitch);
void ma_repitch_node_set_wet_dry(ma_repitch_node *pRepitch, float value);
float ma_repitch_node_get_wet_dry(const ma_repitch_node *pRepitch);

static ma_node_vtable ma_repitch_node_vtable = {
    ma_repitch_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};