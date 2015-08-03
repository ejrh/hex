#include "common.h"

#include "hex/basics/error.h"
#include "hex/graphics/graphics.h"

Image::Image(int id, SDL_Texture *texture):
    id(id), x_offset(0), y_offset(0), texture(texture) {
        Uint32 format;
        int access;
        SDL_QueryTexture(texture, &format, &access, &width, &height);
}

Image::~Image() {
    if (texture != NULL)
        SDL_DestroyTexture(texture);
}

void Graphics::start() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Hex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    SDL_DisplayMode display_mode;
    SDL_GetWindowDisplayMode(window, &display_mode);
    width = display_mode.w;
    height = display_mode.h;

    renderer = SDL_CreateRenderer(window, -1, 0);

    IMG_Init(IMG_INIT_PNG);
}

void Graphics::stop() {
    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::blit(Image *im, int x, int y, SDL_BlendMode mode, int alpha_mod, int mod_r, int mod_g, int mod_b) {
    x += im->x_offset;
    y += im->y_offset;

    SDL_SetTextureBlendMode(im->texture, mode);
    SDL_SetTextureAlphaMod(im->texture, alpha_mod);
    SDL_SetTextureColorMod(im->texture, mod_r, mod_g, mod_b);

    SDL_Rect destrect = { x, y, im->width, im->height };
    if (SDL_RenderCopy(renderer, im->texture, NULL, &destrect) != 0)
        warn("SDL error: %s", SDL_GetError());
}

void Graphics::draw_lines(Uint8 R, Uint8 G, Uint8 B, SDL_Point *points, int count) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_RenderDrawLines(renderer, points, count);
}

void Graphics::draw_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer, &rect);
}

void Graphics::fill_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void Graphics::update() {
    SDL_RenderPresent(renderer);
}
