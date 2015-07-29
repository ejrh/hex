#include "common.h"

#include "hex/game/game.h"

Level::Level(int width, int height): width(width), height(height), tiles(width, height) {
    visibility.set_level(this);
    discovered.set_level(this);
}

void Level::resize(int width, int height) {
    this->width = width;
    this->height = height;
    tiles.resize(width, height);
    visibility.resize(width, height);
    discovered.resize(width, height);
}

Level::~Level() {
}

std::ostream& operator<<(std::ostream &strm, const Level &level) {
    return strm << "Level(" << level.width << ", " << level.height << ")" << std::endl;
}
