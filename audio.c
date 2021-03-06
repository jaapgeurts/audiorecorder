#include "audio.h"

PCM_Play* playback_open(char* name, unsigned int rate, int depth)
{
    int                  err;
    snd_pcm_hw_params_t* hw_params;
    PCM_Play*            play = calloc(1, sizeof(PCM_Play));

#ifdef GCW0
    int channels = 2;
#else
    int channels = 1;
#endif

    int mode = SND_PCM_NONBLOCK;

    if ((err = snd_pcm_open (&(play->playback_handle), name, SND_PCM_STREAM_PLAYBACK, mode)) < 0)
    {
        fprintf (stderr, "cannot open audio device %s (%s)\n", name, snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_any (play->playback_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
        return NULL;
    }

    // Access type if always interleaved
    // It doens't affect mono recordings.
#ifdef GCW0
    snd_pcm_access_t access = SND_PCM_ACCESS_RW_NONINTERLEAVED;
#else
    snd_pcm_access_t access = SND_PCM_ACCESS_RW_INTERLEAVED;
#endif

    if ((err = snd_pcm_hw_params_set_access (play->playback_handle, hw_params, access)) < 0)
    {
        fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err));
        return NULL;
    }

    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    if (depth == 8)
        format = SND_PCM_FORMAT_U8;
    else if (depth == 16)
        format = SND_PCM_FORMAT_S16_LE;
    else
    {
        fprintf(stderr, "Incorrect both depth. Legal values: 8 or 16\n");
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_format (play->playback_handle, hw_params, format)) < 0)
    {
        fprintf (stderr, "cannot set sample format (%s)\n", snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_rate_near (play->playback_handle, hw_params, &rate, 0)) < 0)
    {
        fprintf (stderr, "cannot set sample rate %d, (%s)\n", rate, snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_channels (play->playback_handle, hw_params, channels)) < 0)
    {
        fprintf (stderr, "cannot set channel count to %d (%s)\n", channels, snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params (play->playback_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot set parameters (%s)\n",
            snd_strerror (err));
        return NULL;
    }

    snd_pcm_hw_params_get_period_size(hw_params, &(play->frames), 0);

    play->frames *= 32;  //multiply by 16 it so that the buffer always has enough space

    snd_pcm_hw_params_free (hw_params);

    snd_pcm_nonblock(play->playback_handle, 1);

    return play;
}

void playback_prepare(PCM_Play* play)
{
    int err;

    if ((err = snd_pcm_prepare (play->playback_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror (err));
        return;
    }

    // Setup poll descriptors
    play->poll_count = snd_pcm_poll_descriptors_count(play->playback_handle);
    play->poll_desc  = calloc(play->poll_count, sizeof(struct pollfd));
    snd_pcm_poll_descriptors(play->playback_handle, play->poll_desc, play->poll_count);

    
 //   snd_pcm_start(play->playback_handle);
    
}

void playback_close(PCM_Play* play)
{
    //    snd_pcm_drain(play->playback_handle);
    snd_pcm_drop(play->playback_handle);

    snd_pcm_close (play->playback_handle);

    free(play->poll_desc);

    free(play);
}

bool playback_ready(PCM_Play* play)
{
    unsigned short revents;

    // Do not block, set timeout to 0
    poll(play->poll_desc, play->poll_count, 0);

    snd_pcm_poll_descriptors_revents(play->playback_handle, play->poll_desc, play->poll_count, &revents);

    return (revents & POLLOUT) == POLLOUT;
}

int playback_play(PCM_Play* play, int16_t* data, int count)
{
    int amt = 0;

    printf("before play: count: %d\n", count);

    //         printf(".");
    //         fflush(stdout);

    //  GCW0 Driver doesn't take mono data, must send stereo, so send non interleaved
    // and point both channels to the same array

    short* ptr = &data[0];
#ifdef GCW0
    short* buf[2];
    buf[0] = buf[1] = ptr;

    amt = snd_pcm_writen (play->playback_handle, (void**)buf, count);
#else

    amt = snd_pcm_writei (play->playback_handle, ptr, count);
#endif

    if (amt == -EPIPE)
    {
        printf("Underrun \n");
        snd_pcm_recover (play->playback_handle, amt, 0);
        //snd_pcm_prepare (play->playback_handle);
    }
    else if (amt == -EAGAIN)
    {
        return 0;
    }
    else if (amt < 0)
    {
        fprintf (stderr, "write to audio interface failed (%s)\n", snd_strerror (amt));
        return 0;
    }

    printf("Wrote: %d shorts\n", amt);
    return amt;
}

void playback_stop(PCM_Play* play)
{
    snd_pcm_drop(play->playback_handle);
}

PCM_Capture* capture_open(char* name, unsigned int rate,  int depth)
{
    int                  err;
    PCM_Capture*         capture = calloc(1, sizeof(PCM_Capture));
    snd_pcm_hw_params_t* hw_params;

    int                  channels = 1;

    if ((err = snd_pcm_open (&(capture->capture_handle), name, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        fprintf (stderr, "cannot open audio device %s (%s)\n", name, snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_any (capture->capture_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
        return NULL;
    }

    // Access type if always interleaved
    // It doens't affect mono recordings.
    if ((err = snd_pcm_hw_params_set_access (capture->capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err));
        return NULL;
    }

    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    if (depth == 8)
        format = SND_PCM_FORMAT_U8;
    else if (depth == 16)
        format = SND_PCM_FORMAT_S16_LE;
    else
    {
        fprintf(stderr, "Incorrect both depth. Legal values: 8 or 16\n");
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_format (capture->capture_handle, hw_params, format)) < 0)
    {
        fprintf (stderr, "cannot set sample format (%s)\n", snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_rate_near (capture->capture_handle, hw_params, &rate, 0)) < 0)
    {
        fprintf (stderr, "cannot set sample rate %d (%s)\n", rate, snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_channels (capture->capture_handle, hw_params, channels)) < 0)
    {
        fprintf (stderr, "cannot set channel count %d (%s)\n", channels, snd_strerror (err));
        return NULL;
    }
    
//     snd_pcm_uframes_t period_size = 1024;
//     if ((err == snd_pcm_hw_params_set_period_size_near(capture->capture_handle,hw_params,&period_size,0)) < 0)
//     {
//         fprintf (stderr, "cannot set period size %d, actual: %lu (%s)\n", 1024, period_size, snd_strerror (err));
//         return NULL;
//     }

//      snd_pcm_uframes_t bufsize = 2097152;
//      
//      if ((err = snd_pcm_hw_params_set_buffer_size_near(capture->capture_handle, hw_params, &bufsize)) < 0)
//      {
//          fprintf(stderr, "Can't set recording buffer of %lu bytes\n", bufsize);
//          return NULL;
//      }
//       printf("Recording buffer size %d, actual: %lu\n",2097152, bufsize);

// set buffersize to 2 seconds at 44.1kHz
    static unsigned int time = 5000000; 
    int dir; 
    if ((err == snd_pcm_hw_params_set_buffer_time_near(capture->capture_handle,hw_params,&time, &dir)) < 0)
    {
        fprintf (stderr, "cannot set buffer time %ul, actual: %ul (%s)\n", 5000000, time, snd_strerror (err));
        return NULL;
    }
    
    /*
     snd_pcm_uframes_t bsize;                      *
     snd_pcm_hw_params_get_buffer_size(hw_params,&bsize); */
    
    if ((err = snd_pcm_hw_params (capture->capture_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot set parameters (%s)\n", snd_strerror (err));
        return NULL;
    }


    snd_pcm_hw_params_free (hw_params);

    snd_pcm_nonblock(capture->capture_handle, 1);

    return capture;
}

void capture_close(PCM_Capture* capture)
{
    snd_pcm_drop(capture->capture_handle);
    snd_pcm_close (capture->capture_handle);
    free(capture->poll_desc);
    free(capture);
}

void capture_prepare(PCM_Capture* capture)
{
    int err;

    if ((err = snd_pcm_prepare (capture->capture_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror (err));
        return;
    }


    // Setup poll descriptors
    capture->poll_count = snd_pcm_poll_descriptors_count(capture->capture_handle);
    capture->poll_desc  = calloc(capture->poll_count, sizeof(struct pollfd));
    snd_pcm_poll_descriptors(capture->capture_handle, capture->poll_desc, capture->poll_count);

    snd_pcm_start(capture->capture_handle);
    
}

bool capture_ready(PCM_Capture* capture)
{
    unsigned short revents;


    // Do not block, set timeout to 0
    poll(capture->poll_desc, capture->poll_count, 0);

    snd_pcm_poll_descriptors_revents(capture->capture_handle, capture->poll_desc, capture->poll_count, &revents);

    printf("Poll: %x\n",revents);
    return (revents & POLLIN) == POLLIN;
}

void capture_stop(PCM_Capture* capture)
{
    snd_pcm_drop(capture->capture_handle);
}

/*
 * Count = number of frames
 * buf size should be samplesize * channels * frames
 */
int capture_record(PCM_Capture* capture, int16_t* buf, int count)
{
    int amt;

    snd_pcm_avail_update(capture->capture_handle);
   
    amt = snd_pcm_readi (capture->capture_handle, buf, count);
    
    if (amt == -EPIPE)
    {
        printf("Overrun \n");
        snd_pcm_recover (capture->capture_handle,amt,0);
        //snd_pcm_prepare(capture->capture_handle);
    }
    else if (amt == -EAGAIN)
    {
        return 0;
    }
    else if (amt < 0)
    {
        //snd_pcm_recover (capture->capture_handle,amt,0);
        //snd_pcm_resume(capture->capture_handle);
        fprintf (stderr, "Read from audio interface failed (%s)\n", snd_strerror (amt));
        return 0;
    }
    printf("Read: %d bytes\n",amt);
    return amt;
}

Mixer* mixer_open(const char* name)
{
    Mixer*      mixer = calloc(1, sizeof(Mixer));

    const char* card = "default";

    snd_mixer_open(&(mixer->handle), 0 );
    snd_mixer_attach(mixer->handle, card);
    snd_mixer_selem_register(mixer->handle, NULL, NULL);
    snd_mixer_load(mixer->handle);

    snd_mixer_selem_id_malloc(&(mixer->sid));
    snd_mixer_selem_id_set_index(mixer->sid, 0);
    snd_mixer_selem_id_set_name(mixer->sid, name);
    mixer->elem = snd_mixer_find_selem(mixer->handle, mixer->sid);

    if (mixer->elem == NULL)
    {
        fprintf(stderr, "Can't find mixer %s\n", name);
        return NULL;
    }

    if (snd_mixer_selem_has_capture_channel(mixer->elem, SND_MIXER_SCHN_MONO))
        snd_mixer_selem_get_capture_volume_range(mixer->elem, &mixer->volume_min, &mixer->volume_max);
    else if (snd_mixer_selem_has_playback_channel(mixer->elem, SND_MIXER_SCHN_MONO))
        snd_mixer_selem_get_playback_volume_range(mixer->elem, &mixer->volume_min, &mixer->volume_max);

    return mixer;
}

void mixer_close(Mixer* mixer)
{
    snd_mixer_selem_id_free(mixer->sid);
    snd_mixer_close(mixer->handle);
    free(mixer);
}

void mixer_enable_capture(Mixer* mixer)
{
    snd_mixer_selem_set_capture_switch(mixer->elem, SND_MIXER_SCHN_MONO, 1);
}

void mixer_set_volume(Mixer* mixer, float value)
{
    long int val = (mixer->volume_max - mixer->volume_min) * value + mixer->volume_min;

    //printf("Volume: %lu-%lu-%lu\n",mixer->volume_min,val,mixer->volume_max);

    if (snd_mixer_selem_has_capture_channel(mixer->elem, SND_MIXER_SCHN_MONO))
        snd_mixer_selem_set_capture_volume(mixer->elem, SND_MIXER_SCHN_MONO, val);
    else if (snd_mixer_selem_has_playback_channel(mixer->elem, SND_MIXER_SCHN_MONO))
        snd_mixer_selem_set_playback_volume(mixer->elem, SND_MIXER_SCHN_MONO, val);
}

float mixer_volume(Mixer* mixer)
{
    long int val;

    if (snd_mixer_selem_has_capture_channel(mixer->elem, SND_MIXER_SCHN_MONO))
        snd_mixer_selem_get_capture_volume(mixer->elem, SND_MIXER_SCHN_MONO, &val);
    else if (snd_mixer_selem_has_playback_channel(mixer->elem, SND_MIXER_SCHN_MONO))
        snd_mixer_selem_get_playback_volume(mixer->elem, SND_MIXER_SCHN_MONO, &val);

    //  printf("Volume: %lu-%lu-%lu\n",mixer->volume_min,val,mixer->volume_max);
    return (val - mixer->volume_min) / (float)(mixer->volume_max - mixer->volume_min);
}
