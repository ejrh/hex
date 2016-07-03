#include "common.h"

#include "hex/basics/hexgrid.h"


void get_neighbour(const Point point, int dir, Point *neighbour) {
    int side_adjustment1 = (point.x % 2) ? 0 : -1;
    int side_adjustment2 = side_adjustment1 + 1;
    switch (dir) {
        case 0:
            *neighbour = Point(point.x, point.y - 1);
            break;
        case 1:
            *neighbour = Point(point.x + 1, point.y + side_adjustment1);
            break;
        case 2:
            *neighbour = Point(point.x + 1, point.y + side_adjustment2);
            break;
        case 3:
            *neighbour = Point(point.x, point.y + 1);
            break;
        case 4:
            *neighbour = Point(point.x - 1, point.y + side_adjustment2);
            break;
        case 5:
            *neighbour = Point(point.x - 1, point.y + side_adjustment1);
            break;
        default:
            *neighbour = point;
    }
}

void get_neighbours(const Point point, Point *neighbour) {
    int side_adjustment1 = (point.x % 2) ? 0 : -1;
    int side_adjustment2 = side_adjustment1 + 1;
    neighbour[0] = Point(point.x, point.y - 1);
    neighbour[1] = Point(point.x + 1, point.y + side_adjustment1);
    neighbour[2] = Point(point.x + 1, point.y + side_adjustment2);
    neighbour[3] = Point(point.x, point.y + 1);
    neighbour[4] = Point(point.x - 1, point.y + side_adjustment2);
    neighbour[5] = Point(point.x - 1, point.y + side_adjustment1);
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

void get_circle(const Point point, int radius, std::vector<int>& scanlines) {
    int num_scanlines = 2*radius + 1;
    scanlines.resize(num_scanlines);

    int short_scanlines = (radius + 1) / 2;
    int adj = (point.x % 2 == 0) ? 1 : 0;

    for (int i = 0; i < short_scanlines; i++) {
        scanlines[i] = 2*i + adj;
        scanlines[num_scanlines-1-i] = 2*i + (1 - adj);
    }

    for (int i = short_scanlines; i < num_scanlines - short_scanlines; i++) {
        scanlines[i] = radius;
    }
}

void get_circle_points(const Point point, int radius, std::vector<Point>& points, int width, int height) {
    int num_scanlines = 2 * radius + 1;
    std::vector<int> scanlines(num_scanlines);
    get_circle(point, radius, scanlines);
    for (int i = 0; i < num_scanlines; i++) {
        int row = point.y - radius + i;
        if (row < 0 || row >= height)
            continue;
        int x1 = point.x - scanlines[i];
        int x2 = point.x + scanlines[i];
        if (x1 < 0)
            x1 = 0;
        if (x2 >= width)
            x2 = width - 1;
        for (int j = x1; j <= x2; j++) {
            points.push_back(Point(j, row));
        }
    }
}
