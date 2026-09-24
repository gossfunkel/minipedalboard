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

/**
 * Returns the GraphicsOutput that was passed to setup_window(), or NULL if
 * setup_window() has not been called.
 */
INLINE GraphicsOutput *LevelMeter::
get_window() const {
  return _window;
}

/**
 * Returns the DisplayRegion that the meter has created to render itself into
 * the window to setup_window(), or NULL if setup_window() has not been
 * called.
 */
INLINE DisplayRegion *LevelMeter::
get_display_region() const {
  return _display_region;
}

/**
 * Specifies the number of seconds that should elapse between updates to the
 * level meter display. 
 */
INLINE void LevelMeter::
set_update_interval(double update_interval) {
  _update_interval = update_interval;
}

/**
 * Sets the clock that is used to update the display.  The default is
 * the application's global clock (ClockObject::get_global_clock()).
 */
INLINE void LevelMeter::
set_clock_object(ClockObject *clock_object) {
  _clock_object = clock_object;
  _last_update = 0.0f;
}

/**
 * Returns the clock that is used to determine the frame rate.
 */
INLINE ClockObject *LevelMeter::
get_clock_object() const {
  return _clock_object;
}

/**
 * You can call this to explicitly force the LevelMeter to update itself
 * with the latest frame rate information.  Normally, it is not necessary to
 * call this explicitly.
 */
INLINE void LevelMeter::
update() {
  Thread *current_thread = Thread::get_current_thread();
  do_update(current_thread);
}
