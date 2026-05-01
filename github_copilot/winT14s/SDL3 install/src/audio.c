#include "audio.h"
#include <math.h>

#define AUDIO_FREQ   440.0
#define SAMPLE_RATE  44100
#define CHANNELS     2
#define VOLUME       0.25f

/* Called by SDL when it wants more audio data from the stream. */
static void SDLCALL audio_get_callback(void *userdata,
                                       SDL_AudioStream *stream,
                                       int additional_amount,
                                       int total_amount)
{
    AudioState *a = (AudioState *)userdata;
    (void)total_amount;



    if (additional_amount <= 0) return;

    int    num_frames = additional_amount / (int)(sizeof(float) * CHANNELS);
    float *buf        = (float *)SDL_malloc((size_t)additional_amount);
    if (!buf) return;

    for (int i = 0; i < num_frames; i++) {
        float sample       = VOLUME * sinf((float)a->phase);
        buf[i * 2 + 0]    = sample; /* left  */
        buf[i * 2 + 1]    = sample; /* right */
        a->phase          += 2.0 * SDL_PI_D * AUDIO_FREQ / SAMPLE_RATE;
        if (a->phase >= 2.0 * SDL_PI_D)
            a->phase -= 2.0 * SDL_PI_D;
    }

    SDL_PutAudioStreamData(stream, buf, additional_amount);
    SDL_free(buf);
}

bool Audio_Init(AudioState *a)
{
    a->phase   = 0.0;
    a->playing = false;

    SDL_AudioSpec spec;
    spec.format   = SDL_AUDIO_F32;
    spec.channels = CHANNELS;
    spec.freq     = SAMPLE_RATE;

    a->stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
        audio_get_callback, a);

    if (!a->stream) {
        SDL_Log("[audio] SDL_OpenAudioDeviceStream failed: %s", SDL_GetError());
        return false;
    }

    /* Start paused — user presses Space to toggle */
    return true;
}

void Audio_Toggle(AudioState *a)
{
    if (!a->stream) return;

    if (a->playing) {
        SDL_PauseAudioStreamDevice(a->stream);
        a->playing = false;
    } else {
        SDL_ResumeAudioStreamDevice(a->stream);
        a->playing = true;
    }
}

void Audio_Quit(AudioState *a)
{
    if (a->stream) {
        SDL_DestroyAudioStream(a->stream);
        a->stream = NULL;
    }
    a->playing = false;
}
