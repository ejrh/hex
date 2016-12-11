#include "common.h"

#include "hexutil/basics/error.h"

#include "hexav/graphics/graphics.h"

Image::Image(int id, SDL_Texture *texture):
    id(id), clip_x_offset(0), clip_y_offset(0), texture(texture) {
        Uint32 format;
        int access;
        SDL_QueryTexture(texture, &format, &access, &clip_width, &clip_height);
        width = clip_width;
        height = clip_height;
}

Image::~Image() {
    if (texture != NULL)
        SDL_DestroyTexture(texture);
}


Graphics::Graphics():
    blit_counter("graphics.blit"), blit_pixel_counter("graphics.blit.pixels"), draw_counter("graphics.draw"),
    frame_counter("graphics.frame"),
    font_render_counter("graphics.font.render"),
    ilb_image_load_counter("graphics.image.ilb") { }

void Graphics::start(const std::string& title, int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw Error() << "SDL error while initialising SDL: " << SDL_GetError();

    if (width == 0 || height == 0) {
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    }
    if (!window)
        throw Error() << "SDL error while creating window: " << SDL_GetError();

    SDL_GetWindowSize(window, &this->width, &this->height);

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
        throw Error() << "SDL error while creating renderer: " << SDL_GetError();

    IMG_Init(IMG_INIT_PNG);

    BOOST_LOG_TRIVIAL(info) << boost::format("Window size: %d by %d") % this->width % this->height;
}

void Graphics::stop() {
    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::blit(Image *im, int x, int y, SDL_BlendMode mode, int alpha_mod, int mod_r, int mod_g, int mod_b) {
    x += im->clip_x_offset;
    y += im->clip_y_offset;

    SDL_SetTextureBlendMode(im->texture, mode);
    SDL_SetTextureAlphaMod(im->texture, alpha_mod);
    SDL_SetTextureColorMod(im->texture, mod_r, mod_g, mod_b);

    SDL_Rect destrect = { x, y, im->clip_width, im->clip_height };
    if (SDL_RenderCopy(renderer, im->texture, NULL, &destrect) != 0)
        BOOST_LOG_TRIVIAL(error) << "SDL error while blitting image: " << SDL_GetError();

    ++blit_counter;
    blit_pixel_counter += im->clip_width * im->clip_height;
}

void Graphics::draw_lines(Uint8 R, Uint8 G, Uint8 B, SDL_Point *points, int count) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_RenderDrawLines(renderer, points, count);

    ++draw_counter;
}

void Graphics::draw_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer, &rect);

    ++draw_counter;
}

void Graphics::fill_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);

    ++draw_counter;
}

void Graphics::fill_rectangle(Uint8 R, Uint8 G, Uint8 B, SDL_Rect& rect) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_RenderFillRect(renderer, &rect);

    ++draw_counter;
}

void Graphics::update() {
    SDL_RenderPresent(renderer);

    ++frame_counter;
}

void Graphics::set_target_image(Image *target) {
    target_texture = target->texture;
    SDL_SetRenderTarget(renderer, target_texture);
    width = target->width;
    height = target->height;
}

void Graphics::unset_target_image() {
    target_texture = NULL;
    SDL_SetRenderTarget(renderer, NULL);
    SDL_GetWindowSize(window, &width, &height);
}

void Graphics::save_image(Image *image, const std::string& filename) {
    set_target_image(image);
    save_screenshot(filename);
    unset_target_image();
}

void Graphics::save_screenshot(const std::string& filename) {
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    SDL_SaveBMP(sshot, filename.c_str());
    SDL_FreeSurface(sshot);
}
