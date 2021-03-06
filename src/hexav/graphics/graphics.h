#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "hexutil/basics/statistics.h"


namespace hex {

class Image {
public:
    Image(int id, SDL_Texture *texture);
    ~Image();

public:
    int id;
    int clip_x_offset, clip_y_offset;
    int clip_width, clip_height;
    int width, height;
    SDL_Texture *texture;
};


typedef std::map<std::string, Image *> ImageMap;


class Graphics {
public:
    Graphics(const std::string& title, int width, int height, bool fullscreen);
    ~Graphics();

    void blit(Image *im, int x, int y, SDL_BlendMode mode = SDL_BLENDMODE_NONE, int alpha_mod = 255, int mod_r = 255, int mod_g = 255, int mod_b = 255);
    void blit(SDL_Surface *surface, Image *im, int x, int y);
    void draw_lines(Uint8 R, Uint8 G, Uint8 B, SDL_Point *points, int count);
    void draw_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h);
    void fill_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h);
    void fill_rectangle(Uint8 R, Uint8 G, Uint8 B, SDL_Rect& rect);
    void update();
    int get_width() { return width; }
    int get_height() { return height; }
    void set_target_image(Image *target);
    void unset_target_image();
    void save_image(Image *image, const std::string& filename);
    void save_screenshot(const std::string& filename);
    void set_clip_rect(int x, int y, int w, int h);
    void clear_clip_rect();
    SDL_Rect set_clip_rect(const SDL_Rect& clip_rect);
    SDL_Rect push_clip_rect(const SDL_Rect& clip_rect);

public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
    SDL_Texture *target_texture;

private:
    Counter blit_counter;
    Counter blit_pixel_counter;
    Counter draw_counter;
    Counter frame_counter;
    Counter font_render_counter;
    Counter ilb_image_load_counter;

    friend class TextFormat;
    friend class ILBReader;
};

inline bool rect_contains(const SDL_Rect& rect, int px, int py) {
    return px >= rect.x && py >= rect.y && px < rect.x + rect.w && py < rect.y + rect.h;
}

};

#endif
