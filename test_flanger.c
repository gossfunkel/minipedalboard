#include "ma_flanger.h"

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
    ma_flanger_node flange_node;

    printf("Welcome! Initialising...\n");

    if ((result = ma_engine_init(NULL, &engine)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise engine! Error: %d\n", result);
        return 1;
    }

    if ((result = ma_sound_init_from_file(&engine, "test_tone_2.wav", MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, NULL, NULL, &sound)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise sound from file! Error: %d\n", result);
        return 2;
    }

    ma_flanger_node_config dist_conf = ma_flanger_node_config_init(CHANNELS, SAMPLERATE, .2f, 3.f, 1.f);

    if ((result = ma_flanger_node_init(&engine.nodeGraph, &dist_conf, NULL, &flange_node)) != MA_SUCCESS) {
        if (result == MA_INVALID_ARGS) {
            fprintf(stderr, "flanger node initialised with incorrect values!\n");
            return 3;
        }
        fprintf(stderr, "Failed to initialise flanger node! Error: %d\n", result);
        return 3;
    }

    if ((result = ma_node_attach_output_bus(&sound, 0, &flange_node, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound to flanger node! Error: %d\n", result);
        return 4;
    }

    if ((result = ma_node_attach_output_bus(&flange_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach flanger node to graph endpoint! Error: %d\n", result);
        return 5;
    }

    ma_sound_set_looping(&sound, true);
    ma_sound_start(&sound);

    bool flange_bypassed = false;
    bool shouldClose = false;
    char inLine[64];

    printf("Initialised. Type '?' for commands.\n");

    while (!shouldClose) {
        fflush(stdin);
        printf("> ");
        fgets(inLine, sizeof inLine, stdin);
        if (strchr(inLine, '?') == inLine) {
            printf("==== Node Tester application.\n-- Flanger node\n");
            printf("\t'toggle' - bypass node\n");
            printf("\t'drywet [x]' - set dry/wet to x (floating point value in range 0.0 - 1.0)\n");
            printf("\t'rate [x]' - set flanger modulation rate to x (floating point value in range %f - %f)\n", MIN_RATE, MAX_RATE);
            printf("\t'depth [x]' - set depth of flanging to x (floating point value in range -%f - %f)\n", MIN_DELAY, MAX_DELAY);
            printf("\t'EXIT' - close the application\n");
            printf("==========\n");
        } else if (strstr(inLine,"toggle") == inLine) {
            if (flange_bypassed) {
                printf("Enabling flanger node...\n");
                if (ma_node_attach_output_bus(&sound, 0, &flange_node, 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle node connection!\n");
                    return 6;
                }
                printf("Flanger node enabled.\n");
            } else {
                printf("Disabling flanger node...\n");
                if (ma_node_attach_output_bus(&sound, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle node connection!\n");
                    return 7;
                }
                printf("Flanger node disabled.\n");
            }
            flange_bypassed = !flange_bypassed;
        } else if (strstr(inLine, "drywet") == inLine) {
            ma_flanger_set_dry_wet(&flange_node.flanger, atof(inLine + 7));
            printf("Dry/wet mix is now: %f\n", ma_flanger_get_dry_wet(&flange_node.flanger));
        } else if (strstr(inLine, "rate") == inLine) {
            ma_flanger_set_rate(&flange_node.flanger, atof(inLine + 5));
            printf("Rate is now: %f\n", ma_flanger_get_rate(&flange_node.flanger));
        } else if (strstr(inLine, "depth") == inLine) {
            ma_flanger_set_depth(&flange_node.flanger, atof(inLine + 6));
            printf("Depth is now: %f\n", ma_flanger_get_depth(&flange_node.flanger));
        } else if (strstr(inLine,"EXIT\n") == inLine) {
            printf("Shutting down...\n");
            shouldClose = true;
        } else {
            printf("Unrecognised command. Enter '?' for help.\n");
        }
    }

    ma_sound_uninit(&sound);
    ma_flanger_node_uninit(&flange_node, NULL);
    ma_engine_uninit(&engine);

    printf("Goodbye!\n");

    return 0;
}