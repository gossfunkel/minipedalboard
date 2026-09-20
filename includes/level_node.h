#ifndef LEVEL_METER_H
#define LEVEL_METER_H

#include "pandaNode.h"
#include "lvecBase2.h"
#include "levelMeter.h"

typedef struct {
	LVecBase2 pos;
	float w;
	float h;
} Box;

/* Miniaudio node finds peak and RMS values for each channel,
 	and writes to a public field in a ma_level_data. Panda node 
 	reads from ma_level_meter_node and changes meter appearance 
 	to display levels.
 */
class LevelMeter : PandaNode {
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
	LevelMeter(ma_engine *eng, ma_node *grp, LVecBase2 pos, float w = 25.f, float h = 100.f);
	~LevelMeter();

	INLINE void enable();
	INLINE void disable();

	INLINE LVecBase2 get_scale() const;
	INLINE void set_scale(LVecBase2 scale);
	INLINE void set_scale(float width, float height);

	INLINE float get_width() const;
	INLINE float get_height() const;
	INLINE void set_width(float width);
	INLINE void set_height(float height);

	LVecBase2 get_position() const;
	void set_position(LVecBase2 pos);

	INLINE void update();

}

#endif
