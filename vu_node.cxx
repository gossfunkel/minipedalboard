#include "pandaNode.h"
#include "lvecBase2.h"
#include "levelMeter.h"

/*
 * - miniaudio node finds peak and RMS values for each channel,
 		 and writes to a public field in a ma_level_data
 * - panda node reads from ma_level_meter_node and changes meter
 		 appearance to display levels
 */

typedef struct {
	LVecBase2 pos;
	float w;
	float h;
} Box;

class VU_Node {
	LVecBase2 position;
	float width    = {0.f};
	float height   = {0.f};
	float bar_base = {0.f};
	ma_engine *engine;
	ma_node *sound_group;
	ma_level_meter level_meter = {};
	std::vector<Box> bars  = {};
	std::vector<Box> lines = {};
public:
	// TODO can i just get the engine and group from the MiniAudioManager?
	VU_Node(ma_engine *eng, ma_node *grp, LVecBase2 pos, float w = 25.f, float h = 100.f) : 
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

	~VU_Node() {
		// disconnect level meter node from group and graph endpoint
        if (ma_node_attach_output_bus(sound_group, 0, ma_node_graph_get_endpoint(&engine->nodeGraph), 0) != MA_SUCCESS) {
            fprintf(stderr, "Failed to toggle node connection!\n");
        }
		ma_level_meter_node_uninit(level_meter);
	}

	void enable() {
		// TODO add to task manager
	}

	void disable() {
		// TODO remove from task manager
	}

	void update() {
		for (size_t ch = 0; ch < this->channels; ++ch) {
			// TODO resize bars/lines to LUFS-scaled values?
			this->lines.at(ch).pos.y = this->bar_base - 
									   this->height * this->level_meter.pData[ch]->peak;
			this->bars.at(ch).height = this->height * this->level_meter.pData[ch]->rms;
		}
	}

	void set_scale(float width, float height) {
		this->width = width;
		this->height = height;
	}

	void set_width(float width) {
		this->width = width;
	}

	void set_height(float height) {
		this->height = height;
	}

	void set_scale(LVecBase2 scale) {
		this->width = scale.x;
		this->height = scale.y;
	}

	float get_width() const {
		return this->width;
	}

	float get_height() const {
		return this->height;
	}

	LVecBase2 get_scale() const {
		return LVecBase2{this->width, this->height};
	}

	void set_position(LVecBase2 pos) {
		this->pos = pos;
	}

	LVecBase2 get_position() const {
		return this->pos;
	}
}