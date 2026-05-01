#ifndef SPRITE_H
#define SPRITE_H

#include <SDL3/SDL.h>

typedef struct {
    float x, y;       /* top-left position (pixels) */
    float vx, vy;     /* velocity (pixels/sec)       */
    float w, h;       /* width and height (pixels)   */
    SDL_Color color;
} Sprite;

void Sprite_Init(Sprite *s, float x, float y, float w, float h,
                 float vx, float vy, SDL_Color color);

/* Move sprite by dt seconds; bounce off window edges [0,bw) x [0,bh). */
void Sprite_Update(Sprite *s, float dt, float bounds_w, float bounds_h);

void Sprite_Render(Sprite *s, SDL_Renderer *ren);

#endif /* SPRITE_H */
