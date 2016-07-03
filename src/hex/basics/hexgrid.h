#ifndef HEXGRID_H
#define HEXGRID_H

#include "hex/basics/point.h"

extern void get_neighbour(const Point point, int dir, Point *neighbour);
extern void get_neighbours(const Point point, Point *neighbours);
extern int get_direction(const Point point1, const Point point2);
extern int distance_between(const Point point1, const Point point2);
extern void get_circle(const Point point, int radius, std::vector<int>& scanlines);
extern void get_circle_points(const Point point, int radius, std::vector<Point>& points, int width, int height);

#endif
