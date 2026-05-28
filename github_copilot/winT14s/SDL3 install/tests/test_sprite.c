/*
 * test_sprite.c — pure-C unit tests for bounce physics.
 * No SDL dependency: the Sprite struct and update logic are duplicated here
 * so tests can run headlessly without linking SDL3.
 */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

/* ---- Local copy of the sprite logic (no SDL types needed) ---- */

typedef struct {
    float x, y;
    float vx, vy;
    float w, h;
} TestSprite;

static void test_sprite_update(TestSprite *s, float dt,
                                float bounds_w, float bounds_h)
{
    s->x += s->vx * dt;
    s->y += s->vy * dt;

    if (s->x < 0.0f) {
        s->x  = 0.0f;
        s->vx = -s->vx;
    } else if (s->x + s->w > bounds_w) {
        s->x  = bounds_w - s->w;
        s->vx = -s->vx;
    }

    if (s->y < 0.0f) {
        s->y  = 0.0f;
        s->vy = -s->vy;
    } else if (s->y + s->h > bounds_h) {
        s->y  = bounds_h - s->h;
        s->vy = -s->vy;
    }
}

/* ---- Minimal test harness ---- */

static int s_run = 0, s_failed = 0;

#define CHECK(cond, name) do {                          \
    s_run++;                                            \
    if (!(cond)) {                                      \
        printf("  FAIL: %s\n", (name));                 \
        s_failed++;                                     \
    } else {                                            \
        printf("  PASS: %s\n", (name));                 \
    }                                                   \
} while (0)

/* ---- Tests ---- */

static void test_stays_in_bounds(void)
{
    TestSprite s = { 400.0f, 300.0f, 60.0f, 60.0f, 200.0f, 150.0f };
    const float bw = 800.0f, bh = 600.0f;
    for (int i = 0; i < 10000; i++)
        test_sprite_update(&s, 0.016f, bw, bh);

    CHECK(s.x >= 0.0f,          "x left-edge after 10 000 updates");
    CHECK(s.x + s.w <= bw,      "x right-edge after 10 000 updates");
    CHECK(s.y >= 0.0f,          "y top-edge after 10 000 updates");
    CHECK(s.y + s.h <= bh,      "y bottom-edge after 10 000 updates");
}

static void test_vx_reverses_on_right_edge(void)
{
    const float bw = 800.0f, bh = 600.0f;
    /* Sprite already past the right edge so one update triggers the bounce */
    TestSprite s = { bw - 60.0f + 5.0f, 100.0f, 60.0f, 60.0f, 200.0f, 0.0f };
    float vx_orig = s.vx;

    test_sprite_update(&s, 0.016f, bw, bh);

    CHECK(s.vx == -vx_orig,     "vx reverses when hitting right edge");
    CHECK(s.x + s.w <= bw,      "x clamped to right edge");
}

static void test_vy_reverses_on_bottom_edge(void)
{
    const float bw = 800.0f, bh = 600.0f;
    TestSprite s = { 100.0f, bh - 60.0f + 5.0f, 60.0f, 60.0f, 0.0f, 150.0f };
    float vy_orig = s.vy;

    test_sprite_update(&s, 0.016f, bw, bh);

    CHECK(s.vy == -vy_orig,     "vy reverses when hitting bottom edge");
    CHECK(s.y + s.h <= bh,      "y clamped to bottom edge");
}

static void test_position_advances_without_bounce(void)
{
    /* Place sprite away from all edges so no bounce happens */
    TestSprite s = { 100.0f, 100.0f, 30.0f, 30.0f, 50.0f, 75.0f };
    float expect_x = s.x + s.vx * 0.1f;
    float expect_y = s.y + s.vy * 0.1f;

    test_sprite_update(&s, 0.1f, 800.0f, 600.0f);

    CHECK(fabsf(s.x - expect_x) < 1e-4f, "x = x + vx*dt (no bounce)");
    CHECK(fabsf(s.y - expect_y) < 1e-4f, "y = y + vy*dt (no bounce)");
}

int main(void)
{
    printf("=== test_sprite ===\n");
    test_stays_in_bounds();
    test_vx_reverses_on_right_edge();
    test_vy_reverses_on_bottom_edge();
    test_position_advances_without_bounce();
    printf("Results: %d / %d passed\n", s_run - s_failed, s_run);
    return s_failed > 0 ? 1 : 0;
}
