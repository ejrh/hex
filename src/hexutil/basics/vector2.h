#ifndef VECTOR2_H
#define VECTOR2_H

#include "hexutil/basics/point.h"


namespace hex {

template <typename T>
struct Vector2 {
    typedef std::vector<T> V1;
    typedef std::vector<std::vector<T> > V2;

    Vector2(): width(0), height(0) { };
    Vector2(const int width, const int height) { resize(width, height); };
    int width, height;
    V2 data;

    void resize(int new_width, int new_height) {
        data.resize(new_height);
        for (auto iter = data.begin(); iter != data.end(); iter++) {
            iter->resize(new_width);
        }

        width = new_width;
        height = new_height;
    };

    bool contains(const Point point) const { return point.x >= 0 && point.y >= 0 && point.x < width && point.y < height; }
    void fill(const T& val) {
        for (auto iter = data.begin(); iter != data.end(); iter++) {
            std::fill(iter->begin(), iter->end(), val);
        }
    }

    typename V2::reference operator[](int i) { return data[i]; }
    typename V2::const_reference operator[](int i) const { return data[i]; }
    typename V1::reference operator[](const Point& point) { return data[point.y][point.x]; }
    typename V1::const_reference operator[](const Point& point) const { return data[point.y][point.x]; }
};

template <>
struct Vector2<bool> {
    typedef std::vector<bool> V1;
    typedef std::vector<std::vector<bool> > V2;

    Vector2(): width(0), height(0) { };
    Vector2(const int width, const int height) { resize(width, height); };
    int width, height;
    V2 data;

    void resize(int new_width, int new_height) {
        data.resize(new_height);
        for (auto iter = data.begin(); iter != data.end(); iter++) {
            iter->resize(new_width);
        }

        width = new_width;
        height = new_height;
    };

    bool contains(const Point point) const { return point.x >= 0 && point.y >= 0 && point.x < width && point.y < height; }
    void fill(const bool val) {
        for (auto iter = data.begin(); iter != data.end(); iter++) {
            std::fill(iter->begin(), iter->end(), val);
        }
    }

    V2::reference operator[](int i) { return data[i]; }
    V2::const_reference operator[](int i) const { return data[i]; }
    V1::reference operator[](const Point& point) { return data[point.y][point.x]; }
    V1::const_reference operator[](const Point& point) const { return data[point.y][point.x]; }
};

};

#endif
