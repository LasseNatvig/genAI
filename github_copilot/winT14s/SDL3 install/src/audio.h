#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_AudioStream *stream;
    bool             playing;
    double           phase;   /* accumulated sine phase (radians) */
} AudioState;

/* Open the default playback device and set up a 440 Hz sine-wave stream.
   Returns true on success.  audio->playing starts as false. */
bool Audio_Init(AudioState *a);

/* Toggle audio playback on/off. */
void Audio_Toggle(AudioState *a);

/* Stop and destroy the audio stream. */
void Audio_Quit(AudioState *a);

#endif /* AUDIO_H */
