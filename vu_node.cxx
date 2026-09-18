#include "pandaNode.h"
#include "lvecBase2.h"
#include "levelMeter.h"

/*
 * Probably separate peak meter from UI:
 * - miniaudio node to find peak and RMS or LUFS and save to a public field
 * - panda node to read from public field and change meter appearance
 */

class VU_Node {
	LVecBase2 position = {};
	ma_level_meter level_meter = 
	int channels = {};
	std::vector</*bar*/> bars = {};
	std::vector</*line*/> lines = {};
public:
	VU_Node(int ch, LVecBase2 pos, float w, float h) : 
			channels {ch}, position {pos}, width {w}, height {h} {
		for (auto)
	}

	~VU_Node() {

	}

	void enable() {

	}

	void disable() {

	}

	void update() {
		for (size_t ch = 0; ch < this->channels; ++ch) {
			// FIXME resize bars/lines to LUFS and line to max peak
			this->lines.at(ch).set_position(this->position.x, this->level_meter[ch].peak);
			this->bars.at(ch).set_height(this->level_meter[ch].lufs);
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

	void set_scale(std::pair<float, float> scale) {
		this->width = scale.first;
		this->height = scale.second;
	}

	float get_width() const {
		return this->width;
	}

	float get_height() const {
		return this->height;
	}

	std::pair<float, float> get_scale() const {
		return std::pair<float, float> {this->width, this->height};
	}

	void set_position(LVecBase2 pos) {
		this->pos = pos;
	}

	LVecBase2 get_position() const {
		return this->pos;
	}
}