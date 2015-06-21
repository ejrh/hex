#ifndef VECTOR2_H
#define VECTOR2_H

#include "point.h"

template <typename T>
struct Vector2 {
    typedef std::vector<std::vector<T> > V2;

    Vector2(): width(0), height(0) { };
    int width, height;
    V2 data;

    void resize(int new_width, int new_height) {
        data.resize(new_height);
        for (typename V2::iterator iter = data.begin(); iter != data.end(); iter++) {
            iter->resize(new_width);
        }

        width = new_width;
        height = new_height;
    };

    bool contains(const Point point) const { return point.x >= 0 && point.y >= 0 && point.x < width && point.y < height; }

    std::vector<T>& operator[](int i) { return data[i]; }
    T& operator[](const Point& point) { return data[point.y][point.x]; }
};

#endif
