#include "ma_comp.h"

#include <stdio.h>
#include <stdbool.h>

#define CHANNELS 2
#define SAMPLERATE 48000

int main() {
    ma_engine engine;
    ma_sound sound;
    p3d_compress_node comp_node;

    printf("Welcome! Initialising...\n");

    if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
        return 1;

    if (ma_sound_init_from_file(&engine, "test_tone_2.wav", 0, NULL, NULL, &sound) != MA_SUCCESS)
        return 2;

    p3d_compress_node_config comp_conf = p3d_compress_node_config_init(CHANNELS, SAMPLERATE, .5f, 4.f, 1.f);

    if (p3d_compress_node_init(&engine.nodeGraph, &comp_conf, NULL, &comp_node))
        return 3;

    if (ma_node_attach_output_bus(&sound, 0, &comp_node, 0) != MA_SUCCESS)
        return 4;

    if (ma_node_attach_output_bus(&comp_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS)
        return 5;

    bool comp_bypassed = false;
    bool shouldClose = false;
    char inLine[64];

    printf("Initialised. Type 'toggle' to dis/enable the compressor node. Type 'EXIT' to quit.\n");

    while (!shouldClose) {
        fgets(inLine, sizeof inLine, stdin);
        if (strCmp(inLine,"toggle") == 0) {
            if (comp_bypassed) {
                printf("Enabling compressor node...\n");
                if (ma_node_attach_output_bus(&comp_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS)
                    return 6;
                printf("Compressor node enabled.\n");
            } else {
                printf("Disabling compressor node...\n");
                if (ma_node_attach_output_bus(&sound, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0) != MA_SUCCESS)
                    return 7;
                printf("Compressor node disabled.\n");
            }
            comp_bypassed = !comp_bypassed;
        }
        if (strCmp(inLine,"EXIT\n") == 0) {
            printf("Shutting down...\n");
            shouldClose = true;
        }
    }

    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);

    printf("Goodbye!");

    return 0;
}