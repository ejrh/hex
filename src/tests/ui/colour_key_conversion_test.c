// TO COMPILE:   gcc -o test colour_key_conversion_test.c -lSDL2
// AND RUN:      ./test

#include <stdio.h>

#include <SDL2/SDL.h>

// smiley face image with transparency
unsigned short pixel_data[] = {
    0xB54A, 0xB54A, 0xB54A, 0xB54A, 0xB54A,
    0xB54A, 0x0000, 0xB54A, 0x0000, 0xB54A,
    0xB54A, 0xB54A, 0xB54A, 0xB54A, 0xB54A,
    0x0000, 0xB54A, 0xB54A, 0xB54A, 0x0000,
    0xB54A, 0x0000, 0x0000, 0x0000, 0xB54A
};

int main(int argc, char *argv[]) {
    int w = 5, h = 5;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *background;
    SDL_Rect bg_rect = { 0, 0, 12, 12 };
    SDL_Surface *surface;
    SDL_Rect tgt_rect = { 0, 0, 5, 5 };
    Uint32 pf;
    SDL_Surface *new_surface;
    SDL_Rect tgt_rect2 = { 6, 0, 5, 5 };
    SDL_Texture *bg_tex;
    Uint32 first_pixel;
    Uint8 r,g,b,a;
    Uint32 colour_key;
    int waiting;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Color key conversion test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetScale(renderer, 8, 8);

    // create green background surface
    background = SDL_CreateRGBSurface(0, 12, 12, 32, 0,0,0,0);
    SDL_FillRect(background, &bg_rect, SDL_MapRGB(background->format, 100,200,100));

    // create 16-bit surface
    surface = SDL_CreateRGBSurfaceFrom(pixel_data, w, h, 16, w*2, 0x0000f800,0x000007e0,0x0000001f,0);
    SDL_SetColorKey(surface, SDL_TRUE, 0xB54A);

    // blit original surface to left hand side of background
    SDL_BlitSurface(surface, NULL, background, &tgt_rect);

    // convert surface to 32-bit (suitable for window)
    pf = SDL_GetWindowPixelFormat(window);
    new_surface = SDL_ConvertSurfaceFormat(surface, pf, 0);

    // blit converted surface to right hand side
    SDL_BlitSurface(new_surface, NULL, background, &tgt_rect2);

    // show everything
    bg_tex = SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, bg_tex, NULL, &bg_rect);
    SDL_RenderPresent(renderer);

    // print value of first pixel
    first_pixel = ((Uint32 *) new_surface->pixels)[0];
    SDL_GetRGBA(first_pixel, new_surface->format, &r,&g,&b,&a);
    printf("First pixel = %08x (%d,%d,%d alpha %d)\n", first_pixel, r,g,b,a);
    // print value of colour key
    SDL_GetColorKey(new_surface, &colour_key);
    SDL_GetRGB(colour_key, new_surface->format, &r,&g,&b);
    printf("colour key  = %08x (%d,%d,%d)\n", colour_key, r,g,b);

    waiting = 1;
    while (waiting) {
        SDL_Event evt;
        SDL_WaitEvent(&evt);
        if (evt.type == SDL_QUIT)
            waiting = 0;
    }

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(bg_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
