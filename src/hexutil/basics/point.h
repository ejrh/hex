#ifndef POINT_H
#define POINT_H

namespace hex {

struct Point {
    int x, y;

    Point(): x(0), y(0) { }
    Point(int x, int y): x(x), y(y) { }

    Point constrain(int x1, int y1, int x2, int y2) const {
        Point rv(*this);
        if (rv.x < x1)
            rv.x = x1;
        else if (rv.x > x2)
            rv.x = x2;
        if (rv.y < y1)
            rv.y = y1;
        else if (rv.y > y2)
            rv.y = y2;
        return rv;
    }

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Point& other) const { return x != other.x || y != other.y; }
};

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "(" << p.x << "," << p.y << ")";
}


typedef std::vector<Point> Path;


inline std::ostream& operator<<(std::ostream& os, const std::vector<Point> &path) {
    os << "[";
    bool first = true;
    for (auto iter = path.begin(); iter != path.end(); iter++) {
        if (!first)
            os << ", ";
        else
            first = false;
        os << *iter;
    }
    return os << "]";
}

};

#endif
