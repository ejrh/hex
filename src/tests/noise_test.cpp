#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/noise.h"

#include "hex/graphics/graphics.h"
#include "hex/graphics/font.h"

static bool show_gradients;

void draw_noise(Graphics &graphics, PerlinNoise &noise) {
    graphics.fill_rectangle(0, 0, 0, 0, 0, graphics.width, graphics.height);

    TextFormat tf(SmallFont10, true, 0, 200, 200);

    float min_value = 0.0, max_value = 0.0;

    for (int i = 0; i < 512; i++)
        for (int j = 0; j < 512; j++) {
            float px = j / 512.0f;
            float py = i / 512.0f;
            float value = noise.value(px, py);
            if (value < min_value)
                min_value = value;
            if (value > max_value)
                max_value = value;
        }
    std::ostringstream ss;
    ss << boost::format("min = %0.2f, max = %0.2f") % min_value % max_value;
    tf.write_text(&graphics, ss.str(), graphics.width / 2, graphics.height - 10);

    int offset_x = (graphics.width - 512) / 2;
    int offset_y = (graphics.height - 512) / 2;
    for (int i = 0; i < 512; i++)
        for (int j = 0; j < 512; j++) {
            float px = j / 512.0f;
            float py = i / 512.0f;
            float value = noise.value(px, py);
            int pixel_value = (int) ((value - min_value) / (max_value - min_value) * 255.0);
            if (pixel_value < 0)
                pixel_value = 0;
            if (pixel_value > 255)
                pixel_value = 255;
            graphics.fill_rectangle(pixel_value, pixel_value, pixel_value, j + offset_x, i + offset_y, 1, 1);
        }

    graphics.draw_rectangle(150, 0, 0, offset_x - 1, offset_y - 1, 512 + 2, 512 + 2);

    if (show_gradients) {
        for (int i = 0; i <= 8; i++)
            for (int j = 0; j <= 8; j++) {
                float gx, gy;
                noise.get_gradient(j, i, &gx, &gy);
                std::ostringstream ss;
                ss << boost::format("%0.2f, %0.2f") % gx % gy;
                tf.write_text(&graphics, ss.str(), j * 64 + offset_x, i * 64 + offset_y);
            }
    }

    graphics.update();
}

void run() {
    PerlinNoise noise(8, 8);
    Graphics graphics;
    graphics.start("Noise test", 800, 600, false);

    show_gradients = false;

    SDL_Event evt;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT
                || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE))
                running = false;
            else if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_g)
                show_gradients = !show_gradients;
        }
        draw_noise(graphics, noise);
        SDL_Delay(25);
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
