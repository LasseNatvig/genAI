/*
 * test_audio.c — pure-C unit tests for sine-wave PCM generation.
 * No SDL dependency: the maths are tested in isolation.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define AUDIO_FREQ   440.0
#define SAMPLE_RATE  44100
#define CHANNELS     2
#define VOLUME       0.25f

/* ---- Shared PCM generator (mirrors audio.c logic) ---- */

static void generate_sine(double *phase_inout, float *buf, int num_frames)
{
    double phase = *phase_inout;
    for (int i = 0; i < num_frames; i++) {
        float sample    = VOLUME * sinf((float)phase);
        buf[i * 2 + 0] = sample; /* left  */
        buf[i * 2 + 1] = sample; /* right */
        phase += 2.0 * M_PI * AUDIO_FREQ / SAMPLE_RATE;
        if (phase >= 2.0 * M_PI)
            phase -= 2.0 * M_PI;
    }
    *phase_inout = phase;
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

static void test_sine_at_zero_phase(void)
{
    /* sin(0) == 0 → sample amplitude ≈ 0 */
    double phase = 0.0;
    float  buf[2];
    generate_sine(&phase, buf, 1);
    CHECK(fabsf(buf[0]) < 1e-5f, "sin(phase=0) produces near-zero sample");
}

static void test_sine_at_quarter_period(void)
{
    /* sin(π/2) == 1.0 → sample == VOLUME */
    double phase = M_PI / 2.0;
    float  buf[2];
    generate_sine(&phase, buf, 1);
    CHECK(fabsf(buf[0] - VOLUME) < 1e-5f,
          "sin(phase=pi/2) produces sample == VOLUME");
}

static void test_deterministic_from_same_phase(void)
{
    /* Two runs starting from the same phase must produce identical output */
    double phase_a = 1.2345, phase_b = 1.2345;
    float  buf_a[32], buf_b[32];
    generate_sine(&phase_a, buf_a, 16);
    generate_sine(&phase_b, buf_b, 16);

    bool identical = true;
    for (int i = 0; i < 32; i++) {
        if (fabsf(buf_a[i] - buf_b[i]) > 1e-8f) { identical = false; break; }
    }
    CHECK(identical, "same initial phase yields identical output (deterministic)");
}

static void test_both_channels_equal(void)
{
    /* Sine wave is mono-panned equally to L and R */
    double phase = 0.5;
    float  buf[32];
    generate_sine(&phase, buf, 16);

    bool equal = true;
    for (int i = 0; i < 16; i++) {
        if (fabsf(buf[i * 2 + 0] - buf[i * 2 + 1]) > 1e-8f) {
            equal = false;
            break;
        }
    }
    CHECK(equal, "left channel == right channel (mono sine)");
}

static void test_amplitude_within_range(void)
{
    /* Every sample must lie in [-VOLUME, +VOLUME] */
    double phase   = 0.0;
    int    frames  = 1024;
    float *buf     = (float *)malloc((size_t)(frames * 2) * sizeof(float));
    if (!buf) { printf("  SKIP: allocation failed\n"); return; }

    generate_sine(&phase, buf, frames);

    bool in_range = true;
    for (int i = 0; i < frames * 2; i++) {
        if (buf[i] < -VOLUME - 1e-5f || buf[i] > VOLUME + 1e-5f) {
            in_range = false;
            break;
        }
    }
    free(buf);
    CHECK(in_range, "all samples within [-VOLUME, +VOLUME]");
}

int main(void)
{
    printf("=== test_audio ===\n");
    test_sine_at_zero_phase();
    test_sine_at_quarter_period();
    test_deterministic_from_same_phase();
    test_both_channels_equal();
    test_amplitude_within_range();
    printf("Results: %d / %d passed\n", s_run - s_failed, s_run);
    return s_failed > 0 ? 1 : 0;
}
