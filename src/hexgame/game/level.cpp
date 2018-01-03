#include "common.h"

#include "hexgame/game/game.h"


namespace hex {

Level::Level(int width, int height): width(width), height(height), tiles(width, height) {
}

void Level::resize(int width, int height) {
    this->width = width;
    this->height = height;
    tiles.resize(width, height);
}

Level::~Level() {
}

std::ostream& operator<<(std::ostream &strm, const Level &level) {
    return strm << "Level(" << level.width << ", " << level.height << ")" << std::endl;
}

};
