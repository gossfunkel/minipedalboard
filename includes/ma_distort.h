#include "miniaudio.h"

#define MIN_DRIVE 0.f
#define MAX_DRIVE 100.f
#define LIM_BIAS 3.f

#define DISTORT_MODE_CLIP 0
#define DISTORT_MODE_TANH 1
#define DISTORT_MODE_SIN 2

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    float (*transfer_fn)(float, float, float);
    float drive;
    float factor;
    float bias;
    float wetDry;
} ma_distort_config;

typedef struct {
    ma_node_config nodeConfig;
    ma_distort_config distort;
} ma_distort_node_config;

typedef struct {
    ma_distort_config config;
} ma_distort;

typedef struct {
    ma_node_base baseNode;
    ma_distort distort;
} ma_distort_node;

ma_distort_config ma_distort_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    float (*transfer_fn)(float, float, float),
    float drive,
    float factor,
    float bias,
    float wetDry
);
ma_result ma_distort_init(
    const ma_distort_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_distort *pDistort
);
void ma_distort_uninit(ma_distort *pDistort, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result ma_distort_process_pcm_frames(
    ma_distort *pDistort,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);
ma_distort_node_config ma_distort_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    ma_uint32 mode,
    float drive,
    float factor,
    float bias,
    float wetDry
);
ma_result ma_distort_node_init(
    ma_node_graph *pNodeGraph, 
    const ma_distort_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    ma_distort_node *pDistortNode
);
void ma_distort_node_uninit(ma_distort_node *pDistortNode, const ma_allocation_callbacks *pAllocationCallbacks);
void ma_distort_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void ma_distort_set_drive(ma_distort *pDistort, float value);
float ma_distort_get_drive(const ma_distort *pDistort);
void ma_distort_set_factor(ma_distort *pDistort, float value);
float ma_distort_get_factor(const ma_distort *pDistort);
void ma_distort_set_bias(ma_distort *pDistort, float value);
float ma_distort_get_bias(const ma_distort *pDistort);
void ma_distort_set_wet_dry(ma_distort *pDistort, float value);
float ma_distort_get_wet_dry(const ma_distort *pDistort);

static ma_node_vtable ma_distort_node_vtable = {
    ma_distort_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};
