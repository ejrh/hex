#ifndef POINT_H
#define POINT_H


struct Point {
    int x, y;

    Point(): x(0), y(0) { }
    Point(int x, int y): x(x), y(y) { }

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};


typedef std::vector<Point> Path;


#endif
