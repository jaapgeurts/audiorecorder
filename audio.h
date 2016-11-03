#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#define GCW_MIXER_MIC "Mic"
#define GCW_MIXER_MASTER  "PCM"
#define GCW_MIXER_HEADPHONES  "Headphones"

typedef struct {
    snd_mixer_t*          handle;
    snd_mixer_selem_id_t* sid;
    snd_mixer_elem_t*     elem;
} Mixer;

void playsound(int16_t* data, int count);
int16_t* recordsound();

Mixer* mixer_open(const char* name);
void mixer_enable_capture(Mixer* mixer);
void mixer_close(Mixer* mixer);

#endif