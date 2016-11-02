#include "common.h"

#include "hex/basics/error.h"
#include "hex/graphics/graphics.h"
#include "hex/graphics/font.h"



#define FONT_PATH "c:/windows/fonts/arial.ttf"
#define FONT_PATH_2 "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"


FontCache font_cache;

FontCache::FontCache() {
    TTF_Init();
}

FontCache::~FontCache() {
    for (auto iter = cache.begin(); iter != cache.end(); iter++) {
        TTF_CloseFont(iter->second);
    }

    TTF_Quit();
}

TTF_Font *FontCache::lookup_font(FontId font_id) {
    if (cache.find(font_id) != cache.end()) {
        return cache[font_id];
    }

    int size;
    if (font_id == SmallFont10)
        size = 10;
    else if (font_id == SmallFont14)
        size = 14;
    else {
        BOOST_LOG_TRIVIAL(error) << "Unknown font: " << font_id;
        return NULL;
    }

    TTF_Font *font = TTF_OpenFont(FONT_PATH, size);
    if (!font)
        font = TTF_OpenFont(FONT_PATH_2, size);
    if (!font) {
        BOOST_LOG_TRIVIAL(error) << "TTF error while opening font: " << TTF_GetError();
        return NULL;
    }
    cache[font_id] = font;
    return font;
}


void TextFormat::write_text(Graphics *graphics, const std::string& text, int x, int y) {
    SDL_Color colour = {R, G, B};
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (text_surface == NULL) {
        BOOST_LOG_TRIVIAL(error) << "TTF error while rendering text: " << TTF_GetError();
        return;
    }
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(graphics->renderer, text_surface);

    SDL_Rect dest_rect = { centered ? (x - text_surface->w / 2) : x, centered ? (y - text_surface->h / 2) : y, text_surface->w, text_surface->h };
    SDL_RenderCopy(graphics->renderer, text_texture, NULL, &dest_rect);

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

Image *TextFormat::write_to_image(Graphics *graphics, const std::string& text) {
    SDL_Color colour = {R, G, B};
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (text_surface == NULL) {
        BOOST_LOG_TRIVIAL(error) << "TTF error while rendering text: " << TTF_GetError();
        return NULL;
    }

    if (outlined) {
        SDL_Color outline_colour = {outline_R, outline_G, outline_B};
        SDL_Surface *outline_surface = TTF_RenderText_Solid(font, text.c_str(), outline_colour);
        if (outline_surface == NULL) {
            BOOST_LOG_TRIVIAL(error) << "TTF error while rendering text: " << TTF_GetError();
            return NULL;
        }

        SDL_Surface *combined_surface = SDL_CreateRGBSurface(0, text_surface->w+2, text_surface->h+2, 32, 0,0,0,0);
        if (combined_surface == NULL) {
            BOOST_LOG_TRIVIAL(error) << "SDL error while rendering text: " << SDL_GetError();
            return NULL;
        }

        SDL_FillRect(combined_surface, NULL, SDL_MapRGBA(combined_surface->format, 0,0,0,0));
        SDL_SetColorKey(combined_surface, SDL_TRUE, SDL_MapRGBA(combined_surface->format, 0,0,0,0));

        for (int y = 0; y <= 2; y++) {
            for (int x = 0; x <= 2; x++) {
                if (x == 1 && y == 1)
                    continue;
                SDL_Rect dest_rect = { x, y, outline_surface->w, outline_surface->h };
                SDL_BlitSurface(outline_surface, NULL, combined_surface, &dest_rect);
            }
        }
        SDL_Rect dest_rect = { 1, 1, text_surface->w, text_surface->h };
        SDL_BlitSurface(text_surface, NULL, combined_surface, &dest_rect);

        SDL_FreeSurface(text_surface);
        text_surface = combined_surface;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(graphics->renderer, text_surface);
    SDL_FreeSurface(text_surface);

    Image *image = new Image(0, text_texture);
    if (centered) {
        image->clip_x_offset -= image->width/2;
        image->clip_y_offset -= image->height/2;
    }
    return image;
}


void TextCache::write_text(const std::string& text, int x, int y) {
    Image *image = NULL;

    auto iter = cache.find(text);
    if (iter != cache.end()) {
        image = iter->second;
    } else {
        //TODO discard least-recently-used entry and free its image

        image = format.write_to_image(graphics, text);
        cache[text] = image;
    }

    graphics->blit(image, x, y, SDL_BLENDMODE_BLEND);
}
