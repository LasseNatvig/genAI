/*
 * SDL3 Demo — main.c
 *
 * Features demonstrated:
 *   - Window + 2D hardware renderer
 *   - Keyboard & mouse input
 *   - Bouncing sprite (color rect) with delta-time physics
 *   - Audio: 440 Hz sine-wave via SDL_AudioStream
 *
 * Controls:
 *   ESC / close    Quit
 *   SPACE          Toggle audio (green indicator = on, red = off)
 *   R              Randomise sprite colour
 *   Left click     Teleport sprite to mouse position
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

#include "sprite.h"
#include "audio.h"

#define WINDOW_TITLE "SDL3 Demo"
#define WINDOW_W     800
#define WINDOW_H     600

#define SPRITE_W      60.0f
#define SPRITE_HEIGHT 60.0f
#define SPRITE_VX     200.0f
#define SPRITE_VY    150.0f

/* Maximum allowed delta-time to avoid spiral-of-death on long frames */
#define DT_MAX       0.1f

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SDL_SetMainReady();

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    /* Seed C rand() for sprite colour randomisation */
    srand((unsigned int)SDL_GetTicks());

    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_W, WINDOW_H,
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    /* ---- Sprite ---- */
    SDL_Color sprite_color = { 230, 80, 80, 255 };
    Sprite sprite;
    Sprite_Init(&sprite,
                (WINDOW_W - SPRITE_W) / 2.0f,
                (WINDOW_H - SPRITE_HEIGHT) / 2.0f,
                SPRITE_W, SPRITE_HEIGHT,
                SPRITE_VX, SPRITE_VY,
                sprite_color);

    /* ---- Audio ---- */
    AudioState audio;
    bool has_audio = Audio_Init(&audio);

    /* ---- Window size (updated on SDL_EVENT_WINDOW_RESIZED) ---- */
    float win_w = (float)WINDOW_W;
    float win_h = (float)WINDOW_H;

    /* ---- Main loop ---- */
    bool    running    = true;
    Uint64  last_ticks = SDL_GetTicks();

    while (running) {
        /* --- Events --- */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                switch (e.key.key) {
                case SDLK_ESCAPE:
                    running = false;
                    break;

                case SDLK_SPACE:
                    if (has_audio) Audio_Toggle(&audio);
                    break;

                case SDLK_R: {
                    /* Randomise sprite colour, keep full alpha */
                    sprite.color.r = (Uint8)(rand() % 256);
                    sprite.color.g = (Uint8)(rand() % 256);
                    sprite.color.b = (Uint8)(rand() % 256);
                    break;
                }
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                /* Teleport sprite centre to click position */
                sprite.x = e.button.x - sprite.w / 2.0f;
                sprite.y = e.button.y - sprite.h / 2.0f;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                win_w = (float)e.window.data1;
                win_h = (float)e.window.data2;
                break;
            }
        }

        /* --- Update --- */
        Uint64 now = SDL_GetTicks();
        float  dt  = (float)(now - last_ticks) / 1000.0f;
        if (dt > DT_MAX) dt = DT_MAX;
        last_ticks = now;

        Sprite_Update(&sprite, dt, win_w, win_h);

        /* --- Render --- */
        /* Background */
        SDL_SetRenderDrawColor(renderer, 28, 28, 40, 255);
        SDL_RenderClear(renderer);

        /* Bouncing sprite */
        Sprite_Render(&sprite, renderer);

        /* Audio status indicator (top-right corner)
           Green = audio playing, Red = audio off / unavailable  */
        {
            SDL_FRect indicator = { win_w - 34.0f, 10.0f, 24.0f, 24.0f };
            if (has_audio && audio.playing)
                SDL_SetRenderDrawColor(renderer, 60, 210, 90, 255);
            else
                SDL_SetRenderDrawColor(renderer, 200, 55, 55, 255);
            SDL_RenderFillRect(renderer, &indicator);
        }

        /* HUD: small label rects showing controls */
        /* SPACE hint */
        {
            SDL_FRect bar = { 10.0f, win_h - 18.0f, 120.0f, 8.0f };
            SDL_SetRenderDrawColor(renderer, 80, 80, 110, 200);
            SDL_RenderFillRect(renderer, &bar);
        }

        SDL_RenderPresent(renderer);
    }

    /* --- Cleanup --- */
    if (has_audio) Audio_Quit(&audio);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
