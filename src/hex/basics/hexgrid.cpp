#include "common.h"

#include "hexgrid.h"

void get_neighbours(const Point point, Point *points) {
    int side_adjustment1 = (point.x % 2) ? 0 : -1;
    int side_adjustment2 = side_adjustment1 + 1;
    points[0] = Point(point.x, point.y - 1);
    points[1] = Point(point.x + 1, point.y + side_adjustment1);
    points[2] = Point(point.x + 1, point.y + side_adjustment2);
    points[3] = Point(point.x, point.y + 1);
    points[4] = Point(point.x - 1, point.y + side_adjustment2);
    points[5] = Point(point.x - 1, point.y + side_adjustment1);
}

int get_direction(const Point point1, const Point point2) {
    if (point1.x == point2.x) {
        if (point1.y < point2.y)
            return 0;
        else
            return 3;
    }

    int adj = (point1.x % 2 == 0);

    if (point1.x < point2.x) {
        if (point1.y < point2.y + adj) {
            return 5;
        } else {
            return 4;
        }
    } else {
        if (point1.y < point2.y + adj) {
            return 1;
        } else {
            return 2;
        }
    }
}

int distance_between(const Point point1, const Point point2) {
    int y1 = (point1.x % 2 == 1) ? (2 * point1.y + 1) : (2 * point1.y);
    int y2 = (point2.x % 2 == 1) ? (2 * point2.y + 1) : (2 * point2.y);

    int dx = 2*abs(point2.x - point1.x);
    int dy = abs(y2 - y1);

    if (dy >= dx/2)
        return (dy + dx/2) / 2;
    else
        return dx/2;
}
