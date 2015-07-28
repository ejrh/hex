#include "common.h"

#include "hex/game/game.h"

Level::Level(int width, int height): width(width), height(height), tiles(width, height), visibility(this), discovered(this) {
}

Level::~Level() {
}

std::ostream& operator<<(std::ostream &strm, const Level &level) {
    return strm << "Level(" << level.width << ", " << level.height << ")" << std::endl;
}
