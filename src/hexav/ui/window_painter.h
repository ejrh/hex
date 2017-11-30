#ifndef WINDOW_PAINTER_H
#define WINDOW_PAINTER_H

#include "hexutil/scripting/scripting.h"

#include "hexav/graphics/graphics.h"
#include "hexav/graphics/font.h"
#include "hexav/ui/ui.h"

#include "hexav/resources/paint.h"

class Resources;


class WindowPainter {
public:
    WindowPainter(Resources *resources);
    virtual ~WindowPainter() { }

    void repaint_window(UiWindow *window);

private:
    Resources *resources;

    Counter window_paint_counter;
    Counter window_paint_time;
    Counter window_paint_error;
};


void register_window_interpreters();


#endif
