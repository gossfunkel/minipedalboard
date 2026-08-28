#include "ma_comp.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define CHANNELS 2
#define SAMPLERATE 48000

int main() {
    ma_result result;
    ma_engine engine;
    ma_sound sound;
    p3d_compress_node comp_node;

    printf("Welcome! Initialising...\n");

    if ((result = ma_engine_init(NULL, &engine)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise engine! Error: %d\n", result);
        return 1;
    }

    if ((result = ma_sound_init_from_file(&engine, "test_tone_2.wav", MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, NULL, NULL, &sound)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialise sound from file! Error: %d\n", result);
        return 2;
    }

    p3d_compress_node_config comp_conf = p3d_compress_node_config_init(CHANNELS, SAMPLERATE, .5f, 4.f, 1.f);

    if ((result = p3d_compress_node_init(&engine.nodeGraph, &comp_conf, NULL, &comp_node)) != MA_SUCCESS) {
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

    printf("Initialised. Type 'toggle' to dis/enable the compressor node. Type 'EXIT' to quit.\n");

    while (!shouldClose) {
        fflush(stdin);
        fgets(inLine, sizeof inLine, stdin);
        if (strstr(inLine,"toggle") == inLine) {
            if (comp_bypassed) {
                printf("Enabling compressor node...\n");
                if (ma_node_attach_output_bus(&comp_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS) {
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
        }
        if (strstr(inLine,"EXIT\n") == inLine) {
            printf("Shutting down...\n");
            shouldClose = true;
        }
    }

    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);

    printf("Goodbye!");

    return 0;
}