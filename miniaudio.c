#define MINIAUDIO_IMPLEMENTATION

// We are only using miniaudio for decoding audio files.
#define MA_NO_ENCODING

// We are not using miniaudio's built-in audio generation features.
#define MA_NO_GENERATION

// We are not using miniaudio's built-in file I/O.
// Instead, we are using Panda3D's MovieCursor that supports the same audio formats as the rest of Panda3D.
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3

#include "miniaudio.h"
