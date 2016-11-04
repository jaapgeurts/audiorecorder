#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#define AUDIO_DEVICE "default"
#ifdef GCW0
#define MIXER_MIC        "Mic"
#define MIXER_MASTER     "PCM"
#define MIXER_HEADPHONES "Headphones"
#else
#define MIXER_MIC        "Capture"
#define MIXER_MASTER     "Master"
#define MIXER_HEADPHONES "Master"

#endif
#define PLAYBACK_DEVICE AUDIO_DEVICE
#define CAPTURE_DEVICE  AUDIO_DEVICE

typedef struct
{
    snd_mixer_t*          handle;
    snd_mixer_selem_id_t* sid;
    snd_mixer_elem_t*     elem;
} Mixer;

typedef struct
{
    snd_pcm_t*        playback_handle;
    snd_pcm_uframes_t frames;
} PCM_Play;

typedef struct
{
    snd_pcm_t* capture_handle;
} PCM_Capture;

PCM_Play* playback_open(char* name, unsigned int rate, int depth);
void playback_close(PCM_Play* play);

PCM_Capture* capture_open(char* name, unsigned int rate,int depth);
void capture_close(PCM_Capture* capture);

void playsound(PCM_Play* play, int16_t* data, int count);
int16_t* recordsound(PCM_Capture* capture);

Mixer* mixer_open(const char* name);
void mixer_close(Mixer* mixer);
void mixer_enable_capture(Mixer* mixer);
void mixer_set_volume(Mixer* mixer, int value);

#endif