#include "common.h"

#include "hexutil/basics/noise.h"


namespace hex {

static float interpolate(float x1, float x2, float w) {
    return (1.0f - w) * x1 + w * x2;
}

static float fade(float t) {
    // Approximate sigmoid function:  6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

PerlinNoise::PerlinNoise(int width, int height): grid_width(width), grid_height(height) {
    gradients = new float_vector *[height + 1];
    for (int i = 0; i <= height; i++) {
        gradients[i] = new float_vector[width + 1];
    }

    for (int i = 0; i <= height; i++) {
        for (int j = 0; j <= width; j++) {
            float dx, dy;
            float a;

            do {
                dx = ((rand() % 2000) - 1000) / 1000.0f;
                dy = ((rand() % 2000) - 1000) / 1000.0f;
                a = dx*dx + dy*dy;
            } while (a > 1.0f || a < 0.9f);

            float a2 = sqrtf(a);

            gradients[i][j][0] = dx / a2;
            gradients[i][j][1] = dy / a2;
        }
    }
}

PerlinNoise::~PerlinNoise() {
    for (int i = 0; i <= grid_height; i++) {
        delete[] gradients[i];
    }
    delete[] gradients;
}

float PerlinNoise::dot_product(int ix, int iy, float x, float y) {
     float dx = x - ix;
     float dy = y - iy;

     return dx * gradients[iy][ix][0] + dy * gradients[iy][ix][1];
 }

float PerlinNoise::value(float x, float y) {
     x *= grid_width;
     y *= grid_height;
     int x0 = (int) x;
     int x1 = x0 + 1;
     int y0 = (int) y;
     int y1 = y0 + 1;

     if (x0 < 0 || x1 > grid_width || y0 < 0 || y1 > grid_height)
         return 0.0f;

     float sx = x - (float) x0;
     float sy = y - (float) y0;

     sx = fade(sx);
     sy = fade(sy);

     // Interpolate between grid point gradients
     float n0, n1, ix0, ix1, value;
     n0 = dot_product(x0, y0, x, y);
     n1 = dot_product(x1, y0, x, y);
     ix0 = interpolate(n0, n1, sx);
     n0 = dot_product(x0, y1, x, y);
     n1 = dot_product(x1, y1, x, y);
     ix1 = interpolate(n0, n1, sx);
     value = interpolate(ix0, ix1, sy);

     return value;
}

};
