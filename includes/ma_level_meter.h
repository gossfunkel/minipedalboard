#include "miniaudio.h"

// EFFECT:

typedef struct {
    float peak;
    float rms;
} ma_level_data;

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
} ma_level_meter_config;

typedef struct {
    ma_level_meter_config config;
    ma_uint32 cursor;
    ma_level_data **pData;
} ma_level_meter;

ma_level_meter_config ma_level_meter_config_init(ma_uint32 channels, ma_uint32 sampleRate);

ma_result ma_level_meter_init(
    const ma_level_meter_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_level_meter *pLevelMeter
);

void ma_level_meter_uninit(ma_level_meter *pLevelMeter, const ma_allocation_callbacks *pAllocationCallbacks);

ma_result ma_level_meter_process_pcm_frames(
    ma_level_meter *pLevelMeter,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);

// NODE:

typedef struct {
    ma_node_config nodeConfig;
    ma_level_meter_config levelMeterConfig;
} ma_level_meter_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_level_meter levelMeter;
} ma_level_meter_node;

ma_level_meter_node_config ma_level_meter_node_config_init(ma_uint32 channels, ma_uint32 sampleRate);

ma_result ma_level_meter_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_level_meter_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_level_meter_node *pLevelMeterNode
);

void ma_level_meter_node_uninit(ma_level_meter_node *pLevelMeterNode, const ma_allocation_callbacks *pAllocationCallbacks);

void ma_level_meter_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);