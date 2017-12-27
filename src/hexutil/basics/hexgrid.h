#ifndef HEXGRID_H
#define HEXGRID_H

#include "hexutil/basics/point.h"


typedef std::array<Point, 6> PointNeighbours;

Point get_neighbour(const Point point, int dir);
PointNeighbours get_neighbours(const Point point);
int get_direction(const Point point1, const Point point2);
int distance_between(const Point point1, const Point point2);
std::vector<int> get_circle_scanlines(const Point point, int radius);
std::vector<Point> get_circle_points(const Point point, int radius, int width, int height);
Point pixel_to_point(int px, int py, int x_spacing, int y_spacing, int slope_width, int slope_height);
void point_to_pixel(const Point point, int x_spacing, int y_spacing, int *px, int *py);

#endif
