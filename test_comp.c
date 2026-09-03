#include "ma_comp.h"

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
    ma_compress_node comp_node;

    printf("Welcome! Initialising...\n");

    if ((result = ma_engine_init(NULL, &engine)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise engine! Error: %d\n", result);
        return 1;
    }

    if ((result = ma_sound_init_from_file(&engine, "test_tone_2.wav", MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, NULL, NULL, &sound)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise sound from file! Error: %d\n", result);
        return 2;
    }

    ma_compress_node_config comp_conf = ma_compress_node_config_init(CHANNELS, SAMPLERATE, .8f, 2.f, .1f, .2f, 1.f);

    if ((result = ma_compress_node_init(&engine.nodeGraph, &comp_conf, NULL, &comp_node)) != MA_SUCCESS) {
        if (result == MA_INVALID_ARGS) {
            fprintf(stderr, "Compressor node initialised with incorrect values!\n");
            return 3;
        }
        fprintf(stderr, "Failed to initialise compressor node! Error: %d\n", result);
        return 3;
    }

    if ((result = ma_node_attach_output_bus(&sound, 0, &comp_node, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound to comp node! Error: %d\n", result);
        return 4;
    }

    if ((result = ma_node_attach_output_bus(&comp_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach comp node to graph endpoint! Error: %d\n", result);
        return 5;
    }

    ma_sound_set_looping(&sound, true);
    ma_sound_start(&sound);

    bool comp_bypassed = false;
    bool shouldClose = false;
    char inLine[64];

    printf("Initialised. Type '?' for commands.\n");

    while (!shouldClose) {
        fflush(stdin);
        printf("> ");
        fgets(inLine, sizeof inLine, stdin);
        if (strchr(inLine, '?') == inLine) {
            printf("==== Node Tester application.\n-- Compressor node\n");
            printf("\t'toggle' - bypass compressor node\n");
            printf("\t'drywet [x]' - set compressor dry/wet to x (floating point value)\n");
            printf("\t'EXIT' - close the application\n");
            printf("==========\n");
        } else if (strstr(inLine,"toggle") == inLine) {
            if (comp_bypassed) {
                printf("Enabling compressor node...\n");
                if (ma_node_attach_output_bus(&sound, 0, &comp_node, 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle comp node connection!\n");
                    return 6;
                }
                printf("Compressor node enabled.\n");
            } else {
                printf("Disabling compressor node...\n");
                if (ma_node_attach_output_bus(&sound, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle comp node connection!\n");
                    return 7;
                }
                printf("Compressor node disabled.\n");
            }
            comp_bypassed = !comp_bypassed;
        } else if (strstr(inLine, "drywet") == inLine) {
            float inVal = atof(inLine + 7);
            ma_compress_node_set_wet_dry(&comp_node, inVal);
        } else if (strstr(inLine,"EXIT\n") == inLine) {
            printf("Shutting down...\n");
            shouldClose = true;
        } else {
            printf("Unrecognised command. Enter '?' for help.\n");
        }
    }

    ma_sound_uninit(&sound);
    ma_compress_node_uninit(&comp_node, NULL);
    ma_engine_uninit(&engine);

    printf("Goodbye!");

    return 0;
}