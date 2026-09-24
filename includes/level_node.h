/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file frameRateMeter.I
 * @author drose
 * @date 2003-12-23
 * 
 * @file levelNode.I
 * @author katie
 * @date 2026-09-23
 */

#ifndef LEVEL_METER_H
#define LEVEL_METER_H

#include "pandaNode.h"
#include "lvecBase2.h"
#include "camera.h"
#include "displayRegion.h"
#include "orthographicLens.h"
#include "clockObject.h"
#include "depthTestAttrib.h"
#include "depthWriteAttrib.h"
#include "nodePath.h"
#include "graphicsOutput.h"
#include "pointerTo.h"
#include "cardMaker.h"
#include "levelMeter.h"

class GraphicsChannel;
class ClockObject;

typedef std::pair<PT(PandaNode), PT(PandaNode)> LevelDisplay;

/* Miniaudio node finds peak and RMS values for each channel,
 	and writes to a public field in a ma_level_data. Panda node 
 	reads from ma_level_meter_node and changes meter appearance 
 	to display levels.
 */
class LevelMeter : public PandaNode {
protected:
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

	/*
	INLINE void enable();
	INLINE void disable();
	*/

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

#include "level_node.I"

#endif
