

static ma_node_vtable ma_repitch_node_vtable = {
    ma_repitch_node_process_pcm_frames,
    NULL,
    1, // input bus
    1, // output bus
    0  // default flags
};