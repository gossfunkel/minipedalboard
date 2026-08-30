#include "p3d_distort.h"

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
    p3d_distort_node dist_node;

    printf("Welcome! Initialising...\n");

    if ((result = ma_engine_init(NULL, &engine)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise engine! Error: %d\n", result);
        return 1;
    }

    if ((result = ma_sound_init_from_file(&engine, "test_tone_2.wav", MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, NULL, NULL, &sound)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise sound from file! Error: %d\n", result);
        return 2;
    }

    p3d_distort_node_config dist_conf = p3d_distort_node_config_init(CHANNELS, SAMPLERATE, 10.f, 1.f);

    if ((result = p3d_distort_node_init(&engine.nodeGraph, &dist_conf, NULL, &dist_node)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise distortion node! Error: %d\n", result);
        return 3;
    }

    if ((result = ma_node_attach_output_bus(&sound, 0, &dist_node, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound to distortion node! Error: %d\n", result);
        return 4;
    }

    if ((result = ma_node_attach_output_bus(&dist_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach distortion node to graph endpoint! Error: %d\n", result);
        return 5;
    }

    ma_sound_set_looping(&sound, true);
    ma_sound_start(&sound);

    bool dist_bypassed = false;
    bool shouldClose = false;
    char inLine[64];

    printf("Initialised. Type '?' for commands.\n");

    while (!shouldClose) {
        fflush(stdin);
        printf("> ");
        fgets(inLine, sizeof inLine, stdin);
        if (strchr(inLine, '?') == inLine) {
            printf("==== Node Tester application.\n-- Distortion node\n");
            printf("\t'toggle' - bypass node\n");
            printf("\t'drywet [x]' - set dry/wet to x (floating point value in range 0.0 - 1.0)\n");
            printf("\t'drive [x]' - set drive to x (floating point value in range %f - %f)\n", MIN_DRIVE, MAX_DRIVE);
            printf("\t'EXIT' - close the application\n");
            printf("==========\n");
        } else if (strstr(inLine,"toggle") == inLine) {
            if (dist_bypassed) {
                printf("Enabling distortion node...\n");
                if (ma_node_attach_output_bus(&sound, 0, &dist_node, 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle node connection!\n");
                    return 6;
                }
                printf("Distortion node enabled.\n");
            } else {
                printf("Disabling distortion node...\n");
                if (ma_node_attach_output_bus(&sound, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS) {
                    fprintf(stderr, "Failed to toggle node connection!\n");
                    return 7;
                }
                printf("Distortion node disabled.\n");
            }
            dist_bypassed = !dist_bypassed;
        } else if (strstr(inLine, "drywet") == inLine) {
            float inVal = atof(inLine + 7);
            p3d_distort_set_wet_dry(&dist_node.distort, inVal);
            printf("Wet/dry mix is now: %f\n", p3d_distort_get_wet_dry(&dist_node.distort));
        } else if (strstr(inLine, "drive") == inLine) {
            float inVal = atof(inLine + 6);
            p3d_distort_set_drive(&dist_node.distort, inVal);
            printf("Drive is now: %f\n", p3d_distort_get_drive(&dist_node.distort));
        } else if (strstr(inLine,"EXIT\n") == inLine) {
            printf("Shutting down...\n");
            shouldClose = true;
        } else {
            printf("Unrecognised command. Enter '?' for help.\n");
        }
    }

    ma_sound_uninit(&sound);
    p3d_distort_node_uninit(&dist_node, NULL);
    ma_engine_uninit(&engine);

    printf("Goodbye!");

    return 0;
}