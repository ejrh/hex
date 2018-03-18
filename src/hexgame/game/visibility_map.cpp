#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"
#include "hexgame/game/visibility_map.h"


namespace hex {

VisibilityMap::VisibilityMap(const Level& level) {
    resize(level.width, level.height);
}

VisibilityMap::VisibilityMap(int width, int height) {
    resize(width, height);
}

void VisibilityMap::resize(int width, int height) {
    visibility.resize(width, height);
}

void VisibilityMap::clear() {
    visibility.fill(false);
}

void VisibilityMap::fill() {
    visibility.fill(true);
}

void VisibilityMap::rebuild(const Level& level) {
    visibility.resize(level.tiles.width, level.tiles.height);

    visibility.fill(false);

    for (int i = 0; i < level.tiles.height; i++)
        for (int j = 0; j < level.tiles.width; j++) {
            UnitStack::pointer stack = level.tiles[i][j].stack;
            if (!stack)
                continue;
            apply(*stack, true);
        }
}

void VisibilityMap::apply(const UnitStack& stack, bool visible)
{
    draw(stack.position, stack.sight(), visible);
}

void VisibilityMap::draw(const Point& point, int sight, bool visible) {
    hexgrid_circle circle(point, sight);
    for (auto iter = circle.begin(); iter != circle.end(); iter++) {
        const Point& point = *iter;
        if (point.x < 0 || point.y < 0 || point.x >= visibility.width || point.y >= visibility.height)
            continue;
        visibility[point] = visible;
    }
}

bool VisibilityMap::check(const Point &tile_pos) const {
    return visibility[tile_pos];
}

void VisibilityMap::load(int row, const std::vector<std::string>& data) {
    std::vector<bool>& visibility_row = visibility[row];

    for (unsigned int j = 0; j < visibility_row.size(); j++) {
        const std::string& bit_str = data[j / 8];
        int ch = bit_str[(j % 8) + 1];
        visibility_row[j] = (ch == '1');
    }
}

void VisibilityMap::save(int row, std::vector<std::string>& data) const {
    const std::vector<bool>& visibility_row = visibility[row];

    for (unsigned int j = 0; j < visibility_row.size(); j += 8) {
        std::ostringstream bit_str;
        bit_str << 'b';
        for (int k = 0; k < 8; k++) {
            if (j + k >= visibility_row.size())
                break;
            bit_str << (visibility_row[j + k] ? '1' : '0');
        }
        data.push_back(bit_str.str());
    }
}

void VisibilityMapUnion::clear() {
    maps.clear();
}

void VisibilityMapUnion::add(const VisibilityMap *map) {
    maps.push_back(map);
}

void VisibilityMapUnion::remove(const VisibilityMap *map) {
    auto found = std::find(maps.begin(), maps.end(), map);
    if (found != maps.end())
        maps.erase(found);
}

bool VisibilityMapUnion::check(const Point& tile_pos) const {
    for (auto iter = maps.begin(); iter != maps.end(); iter++) {
        if ((*iter)->check(tile_pos))
            return true;
    }

    return false;
}

};
