#define MINIAUDIO_IMPLEMENTATION

// We are only using miniaudio for decoding audio files.
#define MA_NO_ENCODING

// We are not using miniaudio's built-in audio generation features.
#define MA_NO_GENERATION

#include "miniaudio.h"
