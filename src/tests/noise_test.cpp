#include "common.h"

#include "hex/noise.h"
#include "hex/basics/error.h"
#include "hex/graphics/graphics.h"


void draw_noise(Graphics &graphics, PerlinNoise &noise) {
    float min_value = 0.0, max_value = 0.0;

    for (int i = 0; i < graphics.height; i++)
        for (int j = 0; j < graphics.width; j++) {
            float px = j / (float) graphics.width;
            float py = i / (float) graphics.height;
            float value = noise.value(px, py);
            if (value < min_value)
                min_value = value;
            if (value > max_value)
                max_value = value;
        }

    std::cerr << "min_value " << min_value << "   max_value " << max_value << std::endl;

    for (int i = 0; i < graphics.height; i++)
        for (int j = 0; j < graphics.width; j++) {
            float px = j / (float) graphics.width;
            float py = i / (float) graphics.height;
            float value = noise.value(px, py);
            int pixel_value = (int) ((value - min_value) / (max_value - min_value) * 255.0);
            if (pixel_value < 0)
                pixel_value = 0;
            if (pixel_value > 255)
                pixel_value = 255;
            //graphics.draw_pixel(pixel_value,pixel_value,pixel_value,j,i);
        }

    graphics.update();
}

void run() {
    PerlinNoise noise(8, 6);
    Graphics graphics;
    graphics.start("Noise test", 800, 600, false);

    draw_noise(graphics, noise);

    SDL_Event evt;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT
                || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE))
                running = false;
        }
    }

    graphics.stop();
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
