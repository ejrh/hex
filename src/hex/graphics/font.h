#ifndef FONT_H
#define FONT_H

#include "hex/graphics/graphics.h"

enum FontId {
    SmallFont10,
    SmallFont14
};


class FontCache {
public:
    FontCache();
    ~FontCache();

    TTF_Font *lookup_font(FontId font_id);

private:
    std::map<FontId, TTF_Font *> cache;
};

extern FontCache font_cache;


class TextFormat {
public:
    TextFormat(Graphics *graphics, FontId font_id, bool centered, Uint8 R, Uint8 G, Uint8 B):
        graphics(graphics), font_id(font_id), font(font_cache.lookup_font(font_id)), centered(centered), R(R), G(G), B(B), outlined(false) { }

    TextFormat(Graphics *graphics, FontId font_id, bool centered, Uint8 R, Uint8 G, Uint8 B, Uint8 outline_R, Uint8 outline_G, Uint8 outline_B):
        graphics(graphics), font_id(font_id), font(font_cache.lookup_font(font_id)), centered(centered), R(R), G(G), B(B),
        outlined(true), outline_R(outline_R), outline_G(outline_G), outline_B(outline_B) { }

    void write_text(const std::string& text, int x, int y);
    Image *write_to_image(const std::string& text);

public:
    Graphics *graphics;
    FontId font_id;
    TTF_Font *font;
    bool centered;
    Uint8 R, G, B;
    bool outlined;
    Uint8 outline_R, outline_G, outline_B;
};

#endif
