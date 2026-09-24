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

 #include "level_node.h"

LevelMeter::LevelMeter(MiniAudioManager *mgr, LVecBase2 pos, float w = 25.f, float h = 100.f) : 
		   _engine {mgr->get_engine()}, _sound_group {mgr->get_all_sounds()}, _position {pos}, 
		   _width {w}, _height {-h} {
	_channels = _sound_group->config.channels;
	float barWidth = ((_width-5.f)/_channels) - .1f;
	// temporary value for near bottom of frame
	pos.y += _height - .1f;
	_bar_base = pos.y;
	CardMaker cm = CardMaker();
	for (size_t iChannel = 0; iChannel < channels; ++iChannel) {
		_bars.emplace_back(LevelDisplay(cm.generate(), cm.generate()));
		pos.x += barWidth;
		_bars.at(iChannel).first.set_x(pos.x);
		_bars.at(iChannel).first.set_y(_bar_base);
		_bars.at(iChannel).first.set_width(barWidth);
		_bars.at(iChannel).first.set_height(0.f);
		_bars.at(iChannel).second.set_x(pos.x);
		_bars.at(iChannel).second.set_y(_bar_base);
		_bars.at(iChannel).second.set_width(barWidth);
		_bars.at(iChannel).second.set_height(0.1f);
	}
	// connect level meter node to group and graph endpoint
	ma_result result;
    ma_level_meter_node_config meter_conf = 
    	ma_level_meter_node_config_init(CHANNELS, SAMPLERATE);
    if ((result = ma_level_meter_node_init(&_engine->nodeGraph, &meter_conf, NULL, _level_meter)) != MA_SUCCESS) {
        if (result == MA_INVALID_ARGS) {
            fprintf(stderr, "Level meter node initialised with incorrect values!\n");
            return;
        }
        fprintf(stderr, "Failed to initialise level meter node! Error: %d\n", result);
        return;
    }
    if ((result = ma_node_attach_output_bus(_sound_group, 0, _level_meter, 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach sound group to level meter node! Error: %d\n", result);
        return;
    }
    if ((result = ma_node_attach_output_bus(_level_meter, 0, ma_node_graph_get_endpoint(&_engine->nodeGraph), 0)) != MA_SUCCESS) {
        fprintf(stderr, "Failed to attach level meter node to graph endpoint! Error: %d\n", result);
        return;
    }
	
	_update_interval = level_meter_update_interval;
	_last_update = 0.0f;
	_clock_object = ClockObject::get_global_clock();

	set_align(A_left);
	set_card_color(0.0f, 0.0f, 0.0f, 0.4);
	set_card_as_margin(level_meter_side_margins, level_meter_side_margins, 0.1f, 0.0f);
}

LevelMeter::~LevelMeter() {
	// disconnect level meter node from group and graph endpoint
	//disable();
    if (ma_node_attach_output_bus(_sound_group, 0, ma_node_graph_get_endpoint(&engine->nodeGraph), 0) != MA_SUCCESS) {
        fprintf(stderr, "Failed to toggle node connection!\n");
    }
	ma_level_meter_node_uninit(_level_meter);
	clear_window();
}

void LevelMeter::setup_window(GraphicsOutput *window) {
  clear_window();

  _window = window;

  _root = NodePath("level_meter_root");
  _root.attach_new_node(this);

  CPT(RenderAttrib) dt = DepthTestAttrib::make(DepthTestAttrib::M_none);
  CPT(RenderAttrib) dw = DepthWriteAttrib::make(DepthWriteAttrib::M_off);
  _root.node()->set_attrib(dt, 1);
  _root.node()->set_attrib(dw, 1);
  _root.set_material_off(1);
  _root.set_two_sided(1, 1);

  // If we don't set this explicitly, Panda will cause it to be rendered in a
  // back-to-front cull bin, which will cause the bounding volume to be
  // computed unnecessarily.  Saves a little bit of overhead.
  _root.set_bin("unsorted", 0);

  // Create a display region that covers the entire window.
  _display_region = _window->make_mono_display_region();
  _display_region->set_sort(frame_rate_meter_layer_sort);

  // Finally, we need a camera to associate with the display region.
  PT(Camera) camera = new Camera("level_meter_camera");
  NodePath camera_np = _root.attach_new_node(camera);

  PT(Lens) lens = new OrthographicLens;

  // We choose these values such that we can place the node against (0, 0).
  static const PN_stdfloat left = 0.0f;
  static const PN_stdfloat right = -2.0f;
  static const PN_stdfloat bottom = -2.0f;
  static const PN_stdfloat top = 0.0f;
  lens->set_film_size(right - left, top - bottom);
  lens->set_film_offset((right + left) * 0.5, (top + bottom) * 0.5);
  lens->set_near_far(-1000, 1000);

  camera->set_lens(lens);
  camera->set_scene(_root);
  _display_region->set_camera(camera_np);
}

/**
 * Undoes the effect of a previous call to setup_window().
 */
void LevelMeter::
clear_window() {
  if (_window != nullptr) {
    _window->remove_display_region(_display_region);
    _window = nullptr;
    _display_region = nullptr;
  }
  _root = NodePath();
}

/**
 * This function will be called during the cull traversal to perform any
 * additional operations that should be performed at cull time.  This may
 * include additional manipulation of render state or additional
 * visible/invisible decisions, or any other arbitrary operation.
 *
 * Note that this function will *not* be called unless set_cull_callback() is
 * called in the constructor of the derived class.  It is necessary to call
 * set_cull_callback() to indicated that we require cull_callback() to be
 * called.
 *
 * By the time this function is called, the node has already passed the
 * bounding-volume test for the viewing frustum, and the node's transform and
 * state have already been applied to the indicated CullTraverserData object.
 *
 * The return value is true if this node should be visible, or false if it
 * should be culled.
 */
bool LevelMeter::
cull_callback(CullTraverser *trav, CullTraverserData &data) {
  // This triggers when you try to parent a frame rate meter into the scene
  // graph yourself.  Instead, use setup_window().
  nassertr(_display_region != nullptr, false);

  Thread *current_thread = trav->get_current_thread();

  // Statistics
  //PStatTimer timer(_show_fps_pcollector, current_thread);

  // This is probably a good time to check if the aspect ratio on the window
  // has changed.
  int width = _display_region->get_pixel_width();
  int height = _display_region->get_pixel_height();
  PN_stdfloat aspect_ratio = 1;
  if (width != 0 && height != 0) {
    aspect_ratio = (PN_stdfloat)height / (PN_stdfloat)width;
  }

  // Scale the transform by the calculated aspect ratio.
  if (aspect_ratio != _last_aspect_ratio) {
    _aspect_ratio_transform = 
    	TransformState::make_scale(LVecBase3(aspect_ratio, 1, 1));
    _last_aspect_ratio = aspect_ratio;
  }
  data._net_transform = 
  	data._net_transform->compose(_aspect_ratio_transform);

  // Check to see if it's time to update.
  double now = _clock_object->get_frame_time(current_thread);
  double elapsed = now - _last_update;
  if (elapsed < 0.0 || elapsed >= _update_interval) {
    do_update(current_thread);
  }

  return PandaNode::cull_callback(trav, data);
}

/**
 * Resets the text according to the current frame rate.
 */
void LevelMeter::
do_update(Thread *current_thread) {
	for (auto& [bar,line] : _bars) {
		// TODO resize bars/lines to LUFS-scaled values?
		line.set_y(_bar_base - 
					   _height * _level_meter.pData[ch]->peak);
		bar.set_height(_height * _level_meter.pData[ch]->rms);
	}
}

/*
void LevelMeter::enable() {
	// TODO add to task manager
	setup_window(_window);
}

void LevelMeter::disable() {
	// TODO remove from task manager
	clear_window();
}
*/