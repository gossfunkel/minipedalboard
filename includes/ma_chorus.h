#include "miniaudio.h"

typedef struct {
    ma_node_config nodeConfig;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 bufferSizeInFrames;
    ma_uint32 voices;
    float rate;
    float depth;
    float dryWet;
} ma_chorus_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_chorus_node_config config;
} ma_chorus_node;