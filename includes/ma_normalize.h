#include "miniaudio.h"

typedef struct {
    ma_node_config nodeConfig;
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 bufferSizeInFrames;
    float fade_time;
    float threshold;
    float max_amp;
} ma_normalize_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_normalize_node_config config;
} ma_normalize_node;