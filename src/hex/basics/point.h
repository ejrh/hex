#ifndef POINT_H
#define POINT_H


struct Point {
    int x, y;

    Point(): x(0), y(0) { }
    Point(int x, int y): x(x), y(y) { }

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "(" << p.x << "," << p.y << ")";
}


typedef std::vector<Point> Path;


/*inline std::ostream& operator<<(std::ostream& os, const std::vector<Point> &path) {
    os << "[";
    bool first = true;
    for (std::vector<Point>::const_iterator i = path.begin(); i != path.end(); i++) {
        if (!first)
            os << ", ";
        else
            first = false;
        os << *i;
    }
    return os << "]";
}*/


#endif
