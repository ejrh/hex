#ifndef HEXGRID_H
#define HEXGRID_H

#include "hexutil/basics/point.h"

extern void get_neighbour(const Point point, int dir, Point *neighbour);
extern void get_neighbours(const Point point, Point *neighbours);
extern int get_direction(const Point point1, const Point point2);
extern int distance_between(const Point point1, const Point point2);
extern void get_circle(const Point point, int radius, std::vector<int>& scanlines);
extern void get_circle_points(const Point point, int radius, std::vector<Point>& points, int width, int height);
extern void pixel_to_point(int px, int py, int x_spacing, int y_spacing, int slope_width, int slope_height, Point *point);
extern void point_to_pixel(const Point point, int x_spacing, int y_spacing, int *px, int *py);

#endif
