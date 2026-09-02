#include "miniaudio.h"

typedef struct {
    ma_node_config nodeConfig;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 bufferSizeInFrames;
    float depth;
    float dryWet;
} p3d_phaser_node_config;

typedef struct {
    ma_node_base baseNode;
    p3d_phaser_node_config config;
} p3d_phaser_node;