#ifndef HEXGRID_H
#define HEXGRID_H

#include "hexutil/basics/point.h"


namespace hex {

typedef std::array<Point, 6> PointNeighbours;

Point get_neighbour(const Point point, int dir);
PointNeighbours get_neighbours(const Point point);
int get_direction(const Point point1, const Point point2);
int distance_between(const Point point1, const Point point2);
std::vector<int> get_circle_scanlines(const Point point, int radius);
std::vector<Point> get_circle_points(const Point point, int radius, int width, int height);
Point pixel_to_point(int px, int py, int x_spacing, int y_spacing, int slope_width, int slope_height);
void point_to_pixel(const Point point, int x_spacing, int y_spacing, int *px, int *py);

/**
 * An iterable set of points in the shape of a "circle".
 */
//TODO omit points that are outside of a given area
class hexgrid_circle {
public:
    class iterator: public std::iterator<std::bidirectional_iterator_tag, const Point>
    {
    public:
        iterator(hexgrid_circle& parent):
                parent(parent) {
            y_pos = 0;
            x_pos = -parent.scanlines[y_pos];
        }
        iterator(hexgrid_circle& parent, int x_pos, int y_pos):
                parent(parent), x_pos(x_pos), y_pos(y_pos) { }
        iterator(const iterator& it) : parent(it.parent), x_pos(it.x_pos), y_pos(it.y_pos) { }

        iterator& operator++(int) {
            x_pos++;
            if (x_pos > parent.scanlines[y_pos]) {
                y_pos++;
                if (y_pos >= static_cast<int>(parent.scanlines.size()))
                    x_pos = 0;
                else
                    x_pos = -parent.scanlines[y_pos];
            }
            return *this;
        }
        iterator& operator--(int) {
            x_pos--;
            if (x_pos < -parent.scanlines[y_pos]) {
                y_pos--;
                x_pos = parent.scanlines[y_pos];
            }
            return *this;
        }

        bool operator==(const iterator& rhs) const {
            return x_pos == rhs.x_pos && y_pos == rhs.y_pos;
        }

        bool operator!=(const iterator& rhs) const {
            return x_pos != rhs.x_pos || y_pos != rhs.y_pos;
        }

        const Point operator*() { return Point(x_pos + parent.centre.x, y_pos - parent.radius + parent.centre.y); }

    private:
        hexgrid_circle& parent;
        int x_pos;
        int y_pos;
    };

public:
    hexgrid_circle(const Point centre, int radius):
            centre(centre), radius(radius) {
        scanlines = get_circle_scanlines(centre, radius);
    }
    iterator begin() { return iterator(*this); }
    iterator end() { return iterator(*this, 0, scanlines.size()); }

private:
    Point centre;
    int radius;
    std::vector<int> scanlines;
};

};

#endif
