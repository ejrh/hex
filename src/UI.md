Hex UI Notes
============

UI is a module of Hex that provides a *very* basic windowing system.


UI Windows
----------

All components in the UI inherit the `UIWindow` base class.  Each UI Window has:

  - a parent (unless it's the root window)
  - an x,y position (relative to its parent)
  - a width and height
  - a list of children
  - several flags:
      - WindowVisible (whether it and its children should be drawn)
      - WindowActive (whether it wants to be sent events)
      - WindowWantsMouse (send mouse events to it if they occur within its area)
      - WindowWantsKeys (send keyboard events to it if not already processed)
      - WindowWantsGlobal (send global events to it)

A component may override the `draw` method.  This is called with a UIContext that has been set up
for the window's position on the screen and provides several drawing methods that will accept
coordinates in terms of the window's relative position.  A window does not need to call the draw
methods of its children; this is handled automatically.

Note that an invisible window can still receive events.

A component may also override methods `receive_mouse_event`, `receive_keyboard_event`, and
`receive_global_event`.  Each of these is passed an `SDL_Event` object.

The mouse and keyboard handling methods can return true to indicate that the event has been
handled.  In this case, the event will not be passed to any subsequent windows.

### Mouse events

Mouse events are accompanied by the position of the event relative the window's position.  They
are delivered only if the event occurred has not already been handled by one of the window's
children.

Drag events occur when the mouse is moved while a button is held down.  They are delivered
to the window that first returns a "true" response to the preceding button down event, regardless
of whether the mouse is over that window when the movement occurs.  When the mouse button is
released, the window is forgotten.

### Keyboard events

Keyboard events are delivered to all windows in order from front to back, and to all children of
a window before the window itself.

### Global events

SDL events that are neither mouse nor keyboard events are considered global, including any
user-defined events.

Global events are passed to all windows in the same order as keyboard events.  All windows
with the appropriate flag will receive the event.


UI Loop
-------

The UI loop runs until stopped.  It collects events from SDL, and forwards them to the
appropriate windows.  It periodically draws the screen.
