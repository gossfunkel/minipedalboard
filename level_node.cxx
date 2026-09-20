#include "level_node.h"

LevelMeter::LevelMeter(ma_engine *eng, ma_node *grp, LVecBase2 pos, float w = 25.f, float h = 100.f) : 
		   engine {eng}, sound_group {grp}, position {pos}, width {w}, height {h} {
	size_t channels = sound_group->config.channels;
	float barWidth = ((width-5.f)/channels) - .1f;
	// temporary value for near bottom of frame
	pos.y += height - .1f;
	bar_base = pos.y;
	for (size_t iChannel = 0; iChannel < channels; ++iChannel) {
		pos.x += barWidth;
		bars.emplace_back(Box{pos, barWidth, 0.f});
		lines.emplace_back(Box{pos, barWidth, 0.1f});
	}
	// connect level meter node to group and graph endpoint
	ma_result result;
    ma_level_meter_node_config meter_conf = ma_level_meter_node_config_init(CHANNELS, SAMPLERATE);
    if ((result = ma_level_meter_node_init(&engine->nodeGraph, &meter_conf, NULL, level_meter)) != MA_SUCCESS) {
        if (result == MA_INVALID_ARGS) {
            fprintf(stderr, "Level meter node initialised with incorrect values!\n");
            return;
        }
        fprintf(stderr, "Failed to initialise level meter node! Error: %d\n", result);
        return;
    }
    if ((result = ma_node_attach_output_bus(sound_group, 0, level_meter, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound group to level meter node! Error: %d\n", result);
        return;
    }
    if ((result = ma_node_attach_output_bus(level_meter, 0, ma_node_graph_get_endpoint(&engine->nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach level meter node to graph endpoint! Error: %d\n", result);
        return;
    }
}

LevelMeter::~LevelMeter() {
	// disconnect level meter node from group and graph endpoint
    if (ma_node_attach_output_bus(sound_group, 0, ma_node_graph_get_endpoint(&engine->nodeGraph), 0) != MA_SUCCESS) {
        fprintf(stderr, "Failed to toggle node connection!\n");
    }
	ma_level_meter_node_uninit(level_meter);
}

void LevelMeter::enable() {
	// TODO add to task manager
}

void LevelMeter::disable() {
	// TODO remove from task manager
}

void LevelMeter::update() {
	for (size_t ch = 0; ch < this->channels; ++ch) {
		// TODO resize bars/lines to LUFS-scaled values?
		this->lines.at(ch).pos.y = this->bar_base - 
								   this->height * this->level_meter.pData[ch]->peak;
		this->bars.at(ch).height = this->height * this->level_meter.pData[ch]->rms;
	}
}

void LevelMeter::set_scale(float width, float height) {
	this->width = width;
	this->height = height;
}

void LevelMeter::set_width(float width) {
	this->width = width;
}

void LevelMeter::set_height(float height) {
	this->height = height;
}

void LevelMeter::set_scale(LVecBase2 scale) {
	this->width = scale.x;
	this->height = scale.y;
}

float LevelMeter::get_width() const {
	return this->width;
}

float LevelMeter::get_height() const {
	return this->height;
}

LVecBase2 LevelMeter::get_scale() const {
	return LVecBase2{this->width, this->height};
}

void LevelMeter::set_position(LVecBase2 pos) {
	this->pos = pos;
}

LVecBase2 LevelMeter::get_position() const {
	return this->pos;
}