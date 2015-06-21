#ifndef NOISE_H
#define NOISE_H

typedef float float_vector[2];

class PerlinNoise {
public:
    PerlinNoise(int width, int height);
    ~PerlinNoise();
    float value(float x, float y);

private:
    float dot_product(int ix, int iy, float x, float y);

    float_vector **gradients;

    int grid_width, grid_height;
};

#endif
