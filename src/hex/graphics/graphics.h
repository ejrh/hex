#ifndef GRAPHICS_H
#define GRAPHICS_H

class Image {
public:
    Image(int id, SDL_Texture *texture);
    ~Image();

public:
    int id;
    int clip_x_offset, clip_y_offset;
    int clip_width, clip_height;
    int width, height;

private:
    SDL_Texture *texture;

    friend class Graphics;
};


typedef std::map<std::string, Image *> ImageMap;


class Graphics {
public:
    void start(const std::string& title, int width, int height, bool fullscreen);
    void stop();
    void blit(Image *im, int x, int y, SDL_BlendMode mode = SDL_BLENDMODE_NONE, int alpha_mod = 255, int mod_r = 255, int mod_g = 255, int mod_b = 255);
    void blit(SDL_Surface *surface, Image *im, int x, int y);
    void draw_lines(Uint8 R, Uint8 G, Uint8 B, SDL_Point *points, int count);
    void draw_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h);
    void fill_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h);
    void fill_rectangle(Uint8 R, Uint8 G, Uint8 B, SDL_Rect& rect);
    void update();
    int get_width() { return width; }
    int get_height() { return height; }

public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
};

inline bool rect_contains(const SDL_Rect& rect, int px, int py) {
    return px >= rect.x && py >= rect.y && px < rect.x + rect.w && py < rect.y + rect.h;
}

#endif
