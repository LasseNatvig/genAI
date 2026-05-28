#include "sprite.h"

void Sprite_Init(Sprite *s, float x, float y, float w, float h,
                 float vx, float vy, SDL_Color color)
{
    s->x     = x;
    s->y     = y;
    s->w     = w;
    s->h     = h;
    s->vx    = vx;
    s->vy    = vy;
    s->color = color;
}

void Sprite_Update(Sprite *s, float dt, float bounds_w, float bounds_h)
{
    s->x += s->vx * dt;
    s->y += s->vy * dt;

    /* Bounce off left / right edges */
    if (s->x < 0.0f) {
        s->x  = 0.0f;
        s->vx = -s->vx;
    } else if (s->x + s->w > bounds_w) {
        s->x  = bounds_w - s->w;
        s->vx = -s->vx;
    }

    /* Bounce off top / bottom edges */
    if (s->y < 0.0f) {
        s->y  = 0.0f;
        s->vy = -s->vy;
    } else if (s->y + s->h > bounds_h) {
        s->y  = bounds_h - s->h;
        s->vy = -s->vy;
    }
}

void Sprite_Render(Sprite *s, SDL_Renderer *ren)
{
    SDL_FRect rect = { s->x, s->y, s->w, s->h };
    SDL_SetRenderDrawColor(ren, s->color.r, s->color.g, s->color.b, s->color.a);
    SDL_RenderFillRect(ren, &rect);
}
