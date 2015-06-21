#include "common.h"

#include "hex/basics/error.h"
#include "hex/graphics/graphics.h"

#define FONT_PATH "c:/windows/fonts/arial.ttf"
#define FONT_PATH_2 "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"


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

    TTF_Init();
    small_font = TTF_OpenFont(FONT_PATH, 10);
    if (!small_font)
        small_font = TTF_OpenFont(FONT_PATH_2, 10);
}

void Graphics::stop() {
    TTF_CloseFont(small_font);
    TTF_Quit();

    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::blit(Image *im, int x, int y, int alpha) {
    x += im->x_offset;
    y += im->y_offset;

    SDL_SetTextureAlphaMod(im->texture, alpha);
    SDL_SetTextureBlendMode(im->texture, SDL_BLENDMODE_BLEND);

    SDL_Rect destrect = { x, y, im->width, im->height };
    if (SDL_RenderCopy(renderer, im->texture, NULL, &destrect) != 0)
        warn("SDL error: %s", SDL_GetError());
}

void Graphics::draw_lines(Uint8 R, Uint8 G, Uint8 B, SDL_Point *points, int count) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_RenderDrawLines(renderer, points, count);
}

void Graphics::write_text(Uint8 R, Uint8 G, Uint8 B, std::string text, int x, int y) {
    SDL_Color colour = {R, G, B};
    SDL_Surface *text_surface = TTF_RenderText_Solid(small_font, text.c_str(), colour);
    if (text_surface == NULL) {
        warn("TTF error while rendering text: %s", TTF_GetError());
        return;
    }
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_Rect dest_rect = { x - text_surface->w / 2, y - text_surface->h / 2, text_surface->w, text_surface->h };
    SDL_RenderCopy(renderer, text_texture, NULL, &dest_rect);

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

void Graphics::update() {
    SDL_RenderPresent(renderer);
}
