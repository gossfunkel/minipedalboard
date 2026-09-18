#include "miniaudio.h"

// EFFECT:

typedef struct {
    ma_uint32 channels;
    ma_uint32 sampleRate;
    ma_uint32 voices;
    float rate;
    float depth;
    float dryWet;
} ma_chorus_config;

typedef struct {
    ma_chorus_config config;
    ma_uint32 bufferSizeInFrames;
    float *pBuffer;
} ma_chorus;

// NODE:

typedef struct {
    ma_node_config nodeConfig;
    ma_chorus_config chorusConfig;
} ma_chorus_node_config;

typedef struct {
    ma_node_base baseNode;
    ma_chorus_node chorus;
} ma_chorus_node;

// note: incomplete