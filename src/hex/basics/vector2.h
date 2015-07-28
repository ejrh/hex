#ifndef VECTOR2_H
#define VECTOR2_H

#include "point.h"

template <typename T>
struct Vector2 {
    typedef std::vector<std::vector<T> > V2;

    Vector2(): width(0), height(0) { };
    Vector2(const int width, const int height) { resize(width, height); };
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
    void fill(const T& val) {
        for (typename V2::iterator iter = data.begin(); iter != data.end(); iter++) {
            std::fill(iter->begin(), iter->end(), val);
        }
    }

    std::vector<T>& operator[](int i) { return data[i]; }
    T& operator[](const Point& point) { return data[point.y][point.x]; }
};

template <>
struct Vector2<bool> {
    typedef std::vector<std::vector<bool> > V2;

    Vector2(): width(0), height(0) { };
    Vector2(const int width, const int height) { resize(width, height); };
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
    void fill(const bool val) {
        for (typename V2::iterator iter = data.begin(); iter != data.end(); iter++) {
            std::fill(iter->begin(), iter->end(), val);
        }
    }

    std::vector<bool>& operator[](int i) { return data[i]; }
    std::vector<bool>::reference operator[](const Point& point) { return data[point.y][point.x]; }
};

#endif
