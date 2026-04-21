#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_TITLE  "SDL2 Demo – Bouncing Rectangle"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define TARGET_FPS    60

/* ── rectangle state ──────────────────────────────────────────────── */
typedef struct {
    float x, y;     /* position (top-left corner)  */
    float vx, vy;   /* velocity in pixels/second   */
    int   w, h;     /* size                        */
    Uint8 r, g, b;  /* colour                      */
} Rect;

static void rect_update(Rect *rect, float dt)
{
    rect->x += rect->vx * dt;
    rect->y += rect->vy * dt;

    /* bounce off left/right walls */
    if (rect->x < 0) {
        rect->x  = 0;
        rect->vx = -rect->vx;
    } else if (rect->x + rect->w > WINDOW_WIDTH) {
        rect->x  = (float)(WINDOW_WIDTH - rect->w);
        rect->vx = -rect->vx;
    }

    /* bounce off top/bottom walls */
    if (rect->y < 0) {
        rect->y  = 0;
        rect->vy = -rect->vy;
    } else if (rect->y + rect->h > WINDOW_HEIGHT) {
        rect->y  = (float)(WINDOW_HEIGHT - rect->h);
        rect->vy = -rect->vy;
    }
}

static void rect_draw(SDL_Renderer *renderer, const Rect *rect)
{
    SDL_Rect sdl_rect = {
        .x = (int)rect->x,
        .y = (int)rect->y,
        .w = rect->w,
        .h = rect->h,
    };
    SDL_SetRenderDrawColor(renderer, rect->r, rect->g, rect->b, 255);
    SDL_RenderFillRect(renderer, &sdl_rect);

    /* white border */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &sdl_rect);
}

/* ── main ─────────────────────────────────────────────────────────── */
int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Rect rect = {
        .x  = 100.0f, .y  = 150.0f,
        .vx = 260.0f, .vy = 180.0f,
        .w  = 120,    .h  = 80,
        .r  = 52,     .g  = 152, .b = 219,   /* blue */
    };

    const float frame_time = 1000.0f / TARGET_FPS;  /* ms */
    Uint32 last_tick = SDL_GetTicks();
    bool running = true;

    while (running) {
        /* ── events ── */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        /* ── update ── */
        Uint32 now = SDL_GetTicks();
        float dt = (now - last_tick) / 1000.0f;   /* seconds */
        last_tick = now;
        rect_update(&rect, dt);

        /* ── render ── */
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);  /* dark bg */
        SDL_RenderClear(renderer);

        rect_draw(renderer, &rect);

        SDL_RenderPresent(renderer);

        /* ── cap frame rate (fallback when vsync is unavailable) ── */
        Uint32 elapsed = SDL_GetTicks() - now;
        if (elapsed < (Uint32)frame_time) {
            SDL_Delay((Uint32)frame_time - elapsed);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
