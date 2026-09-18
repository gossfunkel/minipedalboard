#include <stdio.h>
#include <stdbool.h>
#include "ma_level_meter.h"

#define CHANNELS 2
#define SAMPLERATE 48000

int main() {
    ma_result result;
    ma_engine engine;
    ma_sound sound;
    ma_level_meter_node level_node;

    if ((result = ma_engine_init(NULL, &engine)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise engine! Error: %d\n", result);
        return 1;
    }

    if ((result = ma_sound_init_from_file(&engine, "test_tone_2.wav", MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, NULL, NULL, &sound)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise sound from file! Error: %d\n", result);
        return 2;
    }

    ma_level_meter_node_config level_meter_conf = ma_level_meter_node_config_init(CHANNELS, SAMPLERATE);

    if ((result = ma_level_meter_node_init(&engine.nodeGraph, &level_meter_conf, NULL, &level_node)) != MA_SUCCESS) {
        if (result == MA_INVALID_ARGS) {
            fprintf(stderr, "Level meter node initialised with incorrect values!\n");
            return 3;
        }
        fprintf(stderr, "Failed to initialise level meter node! Error: %d\n", result);
        return 3;
    }

    if ((result = ma_node_attach_output_bus(&sound, 0, &level_node, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound to level meter node! Error: %d\n", result);
        return 4;
    }

    if ((result = ma_node_attach_output_bus(&level_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach level meter node to graph endpoint! Error: %d\n", result);
        return 5;
    }

    ma_sound_set_looping(&sound, true);
    ma_sound_start(&sound);

    //bool meter_bypassed = false;
    bool shouldClose = false;
    //char inLine[64];

    printf("Initialised.\n");
    printf(".\n");

	while (!shouldClose) {
		//printf("\33[2K\r");
		printf("\r");
		for (size_t iChannel = 0; iChannel < CHANNELS; iChannel++)
			printf("Channel %llu peak: %f, RMS: %f.", iChannel,
				level_node.levelMeter.pData[iChannel]->peak, 
				level_node.levelMeter.pData[iChannel]->rms);
	}

    ma_sound_uninit(&sound);
    ma_level_meter_node_uninit(&level_node, NULL);
    ma_engine_uninit(&engine);

    printf("Goodbye!\n");
	return 0;
}
