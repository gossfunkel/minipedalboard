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
} p3d_distort_config;

typedef struct {
    ma_node_config nodeConfig;
    p3d_distort_config distort;
} p3d_distort_node_config;

typedef struct {
    p3d_distort_config config;
} p3d_distort;

typedef struct {
    ma_node_base baseNode;
    p3d_distort distort;
} p3d_distort_node;

p3d_distort_config p3d_distort_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    float (*transfer_fn)(float, float, float),
    float drive,
    float factor,
    float bias,
    float wetDry
);
ma_result p3d_distort_init(
    const p3d_distort_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_distort *pdistort
);
void p3d_distort_uninit(p3d_distort *pDistort, const ma_allocation_callbacks *pAllocationCallbacks);
ma_result p3d_distort_process_pcm_frames(
    p3d_distort *pDistort,
    void *pFramesOut,
    const void *pFramesIn,
    ma_uint32 frameCount
);
p3d_distort_node_config p3d_distort_node_config_init(
    ma_uint32 channels,
    ma_uint32 sampleRate,
    ma_uint32 mode,
    float drive,
    float factor,
    float bias,
    float wetDry
);
ma_result p3d_distort_node_init(
    ma_node_graph *pNodeGraph, 
    const p3d_distort_node_config *pConfig, 
    const ma_allocation_callbacks *pAllocationCallbacks, 
    p3d_distort_node *pDistortNode
);
void p3d_distort_node_uninit(p3d_distort_node *pDistortNode, const ma_allocation_callbacks *pAllocationCallbacks);
void p3d_distort_node_process_pcm_frames(
    ma_node *pNode,
    const float **ppFramesIn,
    ma_uint32 *pFrameCountIn,
    float **ppFramesOut,
    ma_uint32 *pFrameCountOut
);

void p3d_distort_set_drive(p3d_distort *pDistort, float value);
float p3d_distort_get_drive(const p3d_distort *pDistort);
void p3d_distort_set_factor(p3d_distort *pDistort, float value);
float p3d_distort_get_factor(const p3d_distort *pDistort);
void p3d_distort_set_bias(p3d_distort *pDistort, float value);
float p3d_distort_get_bias(const p3d_distort *pDistort);
void p3d_distort_set_wet_dry(p3d_distort *pDistort, float value);
float p3d_distort_get_wet_dry(const p3d_distort *pDistort);

static ma_node_vtable p3d_distort_node_vtable = {
    p3d_distort_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};
