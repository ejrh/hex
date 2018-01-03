#include "common.h"

#include "hexutil/basics/hexgrid.h"


namespace hex {

Point get_neighbour(const Point point, int dir) {
    int side_adjustment1 = (point.x % 2) ? 0 : -1;
    int side_adjustment2 = side_adjustment1 + 1;
    switch (dir) {
        case 0:
            return Point (point.x, point.y - 1);
        case 1:
            return Point(point.x + 1, point.y + side_adjustment1);
        case 2:
            return Point(point.x + 1, point.y + side_adjustment2);
        case 3:
            return Point(point.x, point.y + 1);
        case 4:
            return Point(point.x - 1, point.y + side_adjustment2);
        case 5:
            return Point(point.x - 1, point.y + side_adjustment1);
        default:
            return point;
    }
}

PointNeighbours get_neighbours(const Point point) {
    int side_adjustment1 = (point.x % 2) ? 0 : -1;
    int side_adjustment2 = side_adjustment1 + 1;
    PointNeighbours neighbours;
    neighbours[0] = Point(point.x, point.y - 1);
    neighbours[1] = Point(point.x + 1, point.y + side_adjustment1);
    neighbours[2] = Point(point.x + 1, point.y + side_adjustment2);
    neighbours[3] = Point(point.x, point.y + 1);
    neighbours[4] = Point(point.x - 1, point.y + side_adjustment2);
    neighbours[5] = Point(point.x - 1, point.y + side_adjustment1);
    return neighbours;
}

int get_direction(const Point point1, const Point point2) {
    if (point1.x == point2.x) {
        if (point1.y < point2.y)
            return 3;
        else
            return 0;
    }

    int adj = (point1.x % 2 == 0);

    if (point1.x < point2.x) {
        if (point1.y < point2.y + adj) {
            return 2;
        } else {
            return 1;
        }
    } else {
        if (point1.y < point2.y + adj) {
            return 4;
        } else {
            return 5;
        }
    }
}

int distance_between(const Point point1, const Point point2) {
    int y1 = (point1.x % 2 == 1) ? (2 * point1.y + 1) : (2 * point1.y);
    int y2 = (point2.x % 2 == 1) ? (2 * point2.y + 1) : (2 * point2.y);

    int dx = 2*abs(point2.x - point1.x);
    int dy = abs(y2 - y1);

    if (dy >= dx/2)
        return (dy + dx/2) / 2;
    else
        return dx/2;
}

std::vector<int> get_circle_scanlines(const Point point, int radius) {
    int num_scanlines = 2*radius + 1;
    std::vector<int> scanlines(num_scanlines);

    int short_scanlines = (radius + 1) / 2;
    int adj = (point.x % 2 == 0) ? 1 : 0;

    for (int i = 0; i < short_scanlines; i++) {
        scanlines[i] = 2*i + adj;
        scanlines[num_scanlines-1-i] = 2*i + (1 - adj);
    }

    for (int i = short_scanlines; i < num_scanlines - short_scanlines; i++) {
        scanlines[i] = radius;
    }

    return scanlines;
}

std::vector<Point> get_circle_points(const Point point, int radius, int width, int height) {
    std::vector<int> scanlines = get_circle_scanlines(point, radius);
    std::vector<Point> points;

    for (unsigned int i = 0; i < scanlines.size(); i++) {
        int row = point.y - radius + i;
        if (row < 0 || row >= height)
            continue;
        int x1 = point.x - scanlines[i];
        int x2 = point.x + scanlines[i];
        if (x1 < 0)
            x1 = 0;
        if (x2 >= width)
            x2 = width - 1;
        for (int j = x1; j <= x2; j++) {
            points.push_back(Point(j, row));
        }
    }

    return points;
}

Point pixel_to_point(int px, int py, int x_spacing, int y_spacing, int slope_width, int slope_height) {
    int x_mod = px % (2*x_spacing);
    int x_div = px / (2*x_spacing);

    int segment = -99;
    if (x_mod < slope_width)
        segment = 0;
    else if (x_mod < x_spacing)
        segment = 1;
    else if (x_mod < slope_width + x_spacing)
        segment = 2;
    else
        segment = 3;

    Point point;

    if (segment == 0) {
        int y_mod = py % y_spacing;
        int y_div = py / y_spacing;
        if (y_mod*slope_width < slope_height*slope_width - x_mod*slope_height) {
            point.x = 2*x_div - 1;
            point.y = y_div - 1;
        } else if (y_mod*slope_width < slope_height*slope_width + x_mod*slope_height) {
            point.x = 2*x_div;
            point.y = y_div;
        } else {
            point.x = 2*x_div - 1;
            point.y = y_div;
        }
    } else if (segment == 1) {
        point.x = 2 * x_div;
        point.y = py / y_spacing;
    } else if (segment == 2) {
        x_mod -= x_spacing;
        int y_mod = py % y_spacing;
        int y_div = py / y_spacing;
        if (y_mod*slope_width < x_mod*slope_height) {
            point.x = 2*x_div + 1;
            point.y = y_div - 1;
        } else if (y_mod*slope_width < 2*slope_height*slope_width - x_mod*slope_height) {
            point.x = 2*x_div;
            point.y = y_div;
        } else {
            point.x = 2*x_div + 1;
            point.y = y_div;
        }
    } else if (segment == 3) {
        // Note: C99 annoyingly makes integer division round toward zero, which means it does the
        // wrong thing for negative numbers; so this bit of code will be erroneous for negative py.
        // Luckily we don't tend to map negative coordinates to the non-existent tiles that they
        // should map to.
        point.x = 2 * x_div + 1;
        point.y = (py - slope_height) / y_spacing;
    }
    //std::cerr << "px " << px << " py " << py << " segment " << segment << " point " << *point << "\n";

    return point;
}

void point_to_pixel(const Point point, int x_spacing, int y_spacing, int *px, int *py) {
    *px = point.x * x_spacing;
    if (point.x % 2 == 0) {
        *py = point.y * y_spacing;
    } else {
        *py = point.y * y_spacing + y_spacing / 2;
    }
}

};
