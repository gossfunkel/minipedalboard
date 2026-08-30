#include "miniaudio.h"

#define MIN_RATIO 1
#define MAX_RATIO 100

#define MAX_ATTACK 1.f
#define MAX_RELEASE 2.f

/*
typedef struct {
} p3d_compress_config;

typedef struct {
    //ma_uint32 cursor;
    //float *pBuffer;
} p3d_compress;

p3d_compress_config p3d_compress_config_init(
    ma_uint32 channels, 
    ma_uint32 sampleRate, 
    float threshold, 
    float ratio,
    float attack,
    float release,
    float wetDry
);

ma_result p3d_compress_init(
    const p3d_compress_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_compress *pCompress
);
void p3d_compress_uninit(p3d_compress *pCompress, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result p3d_compress_process_pcm_frames(
    p3d_compress *pCompress,
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
} p3d_compress_node_config;

typedef struct {
    ma_node_base baseNode;
    p3d_compress_node_config config;
    float onTime;
    float offTime;
} p3d_compress_node;

p3d_compress_node_config p3d_compress_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    float threshold,
    float ratio,
    float attack,
    float release,
    float wetDry
);
ma_result p3d_compress_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_compress_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_compress_node *pCompressNode
);
void p3d_compress_node_uninit(p3d_compress_node *pCompressNode, const ma_allocation_callbacks *pAllocationCallbacks);
void p3d_compress_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void p3d_compress_node_set_threshold(p3d_compress_node *pCompress, float value);
float p3d_compress_node_get_threshold(const p3d_compress_node *pCompress);
void p3d_compress_node_set_ratio(p3d_compress_node *pCompress, float value);
float p3d_compress_node_get_ratio(const p3d_compress_node *pCompress);
void p3d_compress_node_set_wet_dry(p3d_compress_node *pCompress, float value);
float p3d_compress_node_get_wet_dry(const p3d_compress_node *pCompress);

static ma_node_vtable p3d_compress_node_vtable = {
    p3d_compress_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};