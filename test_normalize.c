#include "ma_normalize.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CHANNELS 2
#define SAMPLERATE 48000

int main() {
    ma_result result;
    ma_engine engine;
    ma_sound sound;
    ma_normalize_node norm_node;

    printf("Welcome! Initialising...\n");

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.channels   = 2;
    engineConfig.sampleRate = 48000;

    if ((result = ma_engine_init(&engineConfig, &engine)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise engine! Error: %d\n", result);
        return 1;
    }

    if ((result = ma_sound_init_from_file(&engine, "test_tone_2.wav", MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, NULL, NULL, &sound)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise sound from file! Error: %d\n", result);
        return 2;
    }

    ma_normalize_node_config dist_conf = ma_normalize_node_config_init(CHANNELS, SAMPLERATE, .4f, .1f, 100.f);

    if ((result = ma_normalize_node_init(&engine.nodeGraph, &dist_conf, NULL, &norm_node)) != MA_SUCCESS) {
        if (result == MA_INVALID_ARGS) {
            fprintf(stderr, "Normalizer node initialised with incorrect values!\n");
            return 3;
        }
        fprintf(stderr, "Failed to initialise normalizer node! Error: %d\n", result);
        return 3;
    }

    if ((result = ma_node_attach_output_bus(&sound, 0, &norm_node, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound to normalizer node! Error: %d\n", result);
        return 4;
    }

    if ((result = ma_node_attach_output_bus(&norm_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach normalizer node to graph endpoint! Error: %d\n", result);
        return 5;
    }

    ma_sound_set_looping(&sound, true);
    ma_sound_start(&sound);

    bool norm_bypassed = false;
    bool shouldClose = false;
    char inLine[64];

    printf("Initialised. Type '?' for commands.\n");

    while (!shouldClose) {
        fflush(stdin);
        printf("> ");
        fgets(inLine, sizeof inLine, stdin);
        if (strchr(inLine, '?') == inLine) {
            printf("==== Node Tester application.\n-- Normalizer node\n");
            printf("\t'toggle' - bypass node\n");
            printf("\t'fade [x]' - set fade in/out time to x (floating point value in range %f - %f)\n", MIN_FADE_TIME, MAX_FADE_TIME);
            printf("\t'threshold [x]' - set threshold from which to trigger gain to x (floating point value in range 0.0 - 1.0)\n");
            printf("\t'max amp [x]' - set maximum gain to x (floating point value in range -%f - %f)\n", MIN_MAX_AMP, MAX_MAX_AMP);
            printf("\t'EXIT' - close the application\n");
            printf("==========\n");
        } else if (strstr(inLine,"toggle") == inLine) {
            if (norm_bypassed) {
                printf("Enabling normalizer node...\n");
                if (ma_node_attach_output_bus(&sound, 0, &norm_node, 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle node connection!\n");
                    return 6;
                }
                printf("Normalizer node enabled.\n");
            } else {
                printf("Disabling normalizer node...\n");
                if (ma_node_attach_output_bus(&sound, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle node connection!\n");
                    return 7;
                }
                printf("Normalizer node disabled.\n");
            }
            norm_bypassed = !norm_bypassed;
        } else if (strstr(inLine, "fade") == inLine) {
            ma_normalize_set_fade_time(&norm_node.normalize, atof(inLine + 5));
            printf("Fade time is now: %f\n", ma_normalize_get_fade_time(&norm_node.normalize));
        } else if (strstr(inLine, "threshold") == inLine) {
            ma_normalize_set_threshold(&norm_node.normalize, atof(inLine + 10));
            printf("Threshold is now: %f\n", ma_normalize_get_threshold(&norm_node.normalize));
        } else if (strstr(inLine, "max amp") == inLine) {
            ma_normalize_set_max_amp(&norm_node.normalize, atof(inLine + 8));
            printf("Maximum gain is now: %f\n", ma_normalize_get_max_amp(&norm_node.normalize));
        } else if (strstr(inLine,"EXIT\n") == inLine) {
            printf("Shutting down...\n");
            shouldClose = true;
        } else {
            printf("Unrecognised command. Enter '?' for help.\n");
        }
    }

    ma_sound_uninit(&sound);
    ma_normalize_node_uninit(&norm_node, NULL);
    ma_engine_uninit(&engine);

    printf("Goodbye!");

    return 0;
}