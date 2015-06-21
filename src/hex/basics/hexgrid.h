#ifndef HEXGRID_H
#define HEXGRID_H

#include "point.h"

extern void get_neighbours(const Point point, Point *points);
extern int get_direction(const Point point1, const Point point2);
extern int distance_between(const Point point1, const Point point2);

#endif
