#ifndef PAINT_H
#define PAINT_H

#include "hexutil/scripting/scripting.h"

#include "hexav/graphics/graphics.h"

class Resources;

class PaintInstructionCompiler: public InstructionCompiler {
    virtual Instruction *compile(Message *message, Compiler *compiler);
};

class PaintItem {
public:
    int offset_x, offset_y;
    std::vector<Image *> frames;
    int frame_rate;
    int blend_addition;
};

class Paint {
public:
    void render(int x, int y, int phase, Graphics *graphics);

    void clear() {
        items.clear();
    }

public:
    std::vector<PaintItem> items;
};

class PaintExecution: public Execution {
public:
    PaintExecution(StrMap<Script> *scripts, Resources *resources, Paint *paint): Execution(scripts), resources(resources), paint(paint) {
        paint_library_atom = AtomRegistry::atom("paint_library");
        paint_offset_x_atom = AtomRegistry::atom("paint_offset_x");
        paint_offset_y_atom = AtomRegistry::atom("paint_offset_y");
        paint_blend_addition_atom = AtomRegistry::atom("paint_blend_addition");
    }

    void paint_frame(Atom image_libary, int frame_num, int offset_x, int offset_y, int blend_addition);
    void paint_animation(Atom image_libary, int frame_rate, std::vector<int>& frame_nums, int offset_x, int offset_y, int blend_addition);

public:
    Resources *resources;
    Paint *paint;

    Atom paint_library_atom;
    Atom paint_offset_x_atom;
    Atom paint_offset_y_atom;
    Atom paint_blend_addition_atom;
};

#endif
