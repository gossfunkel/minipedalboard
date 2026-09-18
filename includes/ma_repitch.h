#include "miniaudio.h"

// EFFECT:

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    float pitch;
    float grainSize;
    float wetDry;
} ma_repitch_config;

typedef struct {
    ma_repitch_config config;
    ma_uint32 bufferSizeInFrames;
    float *pBuffer;
} ma_repitch;

// NODE:

typedef struct {
    ma_node_config nodeConfig;
} ma_repitch_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_repitch_node_config config;
} ma_repitch_node;

ma_compress_node_config ma_repitch_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    float pitch,
    float grainSize,
    float wetDry
);

ma_result ma_repitch_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_repitch_node *pRepitchNode
);

void ma_repitch_node_uninit(ma_repitch_node *pRepitchNode, const ma_allocation_callbacks *pAllocationCallbacks);

void ma_repitch_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

// GETTERS/SETTERS:

void ma_repitch_node_set_pitch(ma_repitch_node *pRepitch, float value);
float ma_repitch_node_get_pitch(const ma_repitch_node *pRepitch);
void ma_repitch_node_set_grain_size(ma_repitch_node *pRepitch, float value);
float ma_repitch_node_get_grain_size(const ma_repitch_node *pRepitch);
void ma_repitch_node_set_wet_dry(ma_repitch_node *pRepitch, float value);
float ma_repitch_node_get_wet_dry(const ma_repitch_node *pRepitch);
