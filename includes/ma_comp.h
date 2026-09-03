#include "miniaudio.h"

#define MIN_RATIO 1
#define MAX_RATIO 100

#define MAX_ATTACK 1.f
#define MAX_RELEASE 2.f

/*
typedef struct {
} ma_compress_config;

typedef struct {
    //ma_uint32 cursor;
    //float *pBuffer;
} ma_compress;

ma_compress_config ma_compress_config_init(
    ma_uint32 channels, 
    ma_uint32 sampleRate, 
    float threshold, 
    float ratio,
    float attack,
    float release,
    float wetDry
);

ma_result ma_compress_init(
    const ma_compress_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_compress *pCompress
);
void ma_compress_uninit(ma_compress *pCompress, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result ma_compress_process_pcm_frames(
    ma_compress *pCompress,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);
*/

typedef struct {
    ma_node_config nodeConfig;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 bufferSizeInFrames;
    float threshold;
    float ratio;
    float attack;
    float release;
    float wetDry;
} ma_compress_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_compress_node_config config;
    float onTime;
    float offTime;
} ma_compress_node;

ma_compress_node_config ma_compress_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    float threshold,
    float ratio,
    float attack,
    float release,
    float wetDry
);
ma_result ma_compress_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_compress_node *pCompressNode
);
void ma_compress_node_uninit(ma_compress_node *pCompressNode, const ma_allocation_callbacks *pAllocationCallbacks);
void ma_compress_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void ma_compress_node_set_threshold(ma_compress_node *pCompress, float value);
float ma_compress_node_get_threshold(const ma_compress_node *pCompress);
void ma_compress_node_set_ratio(ma_compress_node *pCompress, float value);
float ma_compress_node_get_ratio(const ma_compress_node *pCompress);
void ma_compress_node_set_attack(ma_compress_node *pCompress, float value);
float ma_compress_node_get_attack(const ma_compress_node *pCompress);
void ma_compress_node_set_release(ma_compress_node *pCompress, float value);
float ma_compress_node_get_release(const ma_compress_node *pCompress);
void ma_compress_node_set_wet_dry(ma_compress_node *pCompress, float value);
float ma_compress_node_get_wet_dry(const ma_compress_node *pCompress);

static ma_node_vtable ma_compress_node_vtable = {
    ma_compress_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};