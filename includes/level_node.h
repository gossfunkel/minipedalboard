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

typedef std::pair<Box, Box> LevelDisplay;

/* Miniaudio node finds peak and RMS values for each channel,
 	and writes to a public field in a ma_level_data. Panda node 
 	reads from ma_level_meter_node and changes meter appearance 
 	to display levels.
 */
class LevelMeter : public PandaNode {
	LVecBase2 _position;
	float _width;
	float _height;
	float _bar_base;
	PT(GraphicsOutput) _window;
	PT(DisplayRegion) _display_region;
	NodePath _root;
	ma_engine *_engine;
	ma_node *_sound_group;
	ma_level_meter _level_meter;
	size_t _channels;
	std::vector<LevelDisplay> _bars;
	ClockObject *_clock_object;
	double _update_interval;
	double _last_update;

	PN_stdfloat _last_aspect_ratio;
	CPT(TransformState) _aspect_ratio_transform;
PUBLISHED:
	// can we just get the engine and group from the MiniAudioManager?
	LevelMeter(MiniAudioManager *mgr, LVecBase2 pos, float w = 25.f, float h = 100.f);
	~LevelMeter();

	void setup_window(GraphicsOutput *window);
	void clear_window();

	INLINE GraphicsOutput *get_window() const;
	INLINE DisplayRegion *get_display_region() const;

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

	INLINE void set_clock_object(ClockObject *clock_object);
	INLINE ClockObject *get_clock_object() const;

	INLINE void set_update_interval(double update_interval);
	INLINE double get_update_interval() const;

	INLINE void update();
protected:
	virtual bool cull_callback(CullTraverser *trav, CullTraverserData &data);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    PandaNode::init_type();
    register_type(_type_handle, "LevelMeter",
                  PandaNode::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
}

#endif
