#include <sndfile.h>
#include <sys/stat.h>

#include "audio.h"

char* device = "default";

void playsound(int16_t* data, int count)
{
    int                  err;
    short*               buf;
    snd_pcm_t*           playback_handle;
    snd_pcm_hw_params_t* hw_params;

    /*    struct stat          st;

        stat("test.raw", &st);
        unsigned long len = st.st_size / sizeof(short);
        printf("File %s is %lu bytes long, %lu shorts\n", "test.raw", (unsigned long)st.st_size, len);

        FILE* fp = fopen("test.raw", "r");
     */
    if ((err = snd_pcm_open (&playback_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        fprintf (stderr, "cannot open audio device %s (%s)\n",
            device,
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf (stderr, "cannot set access type (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        fprintf (stderr, "cannot set sample format (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    unsigned int rate = 44100;

    if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0)
    {
        fprintf (stderr, "cannot set sample rate (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0)
    {
        fprintf (stderr, "cannot set channel count (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot set parameters (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    snd_pcm_uframes_t frames;
    snd_pcm_hw_params_get_period_size(hw_params, &frames, 0);

    frames *= 16;  //multiply by 8 it so that the buffer always has enough
    //  snd_pcm_hw_params_get_buffer_size(hw_params,&frames);

    printf("Settin buffer: %lu\n", frames);

    if (frames == 0)
    {
        fprintf(stderr, "Frames = 0");
        exit(1);
    }

    buf = (short*)malloc(sizeof(short) * frames * 2);

    snd_pcm_hw_params_free (hw_params);

    if ((err = snd_pcm_prepare (playback_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    printf("before play\n");

    int  actual;
    long total = 0;
    int  start = 0;
    int  loop  = 0;
    do
    {
        start = loop * frames;
        int i;

        // copy into the buffer so we may interleave the data
        for (i = 0; i < frames; i++)
        {
            if (start + i >= count)
                break;
            buf[i * 2]     = data[start + i];
            buf[i * 2 + 1] = data[start + i];
        }
        loop++;
        actual = i;
        total += actual;

        if (actual > 0)
        {
            printf(".");
            fflush(stdout);

            if ((err = snd_pcm_writei (playback_handle, buf, actual)) == -EPIPE)
            {
                printf("Underrun \n");
                snd_pcm_prepare (playback_handle);
            }
            else if (err < 0)
            {
                fprintf (stderr, "write to audio interface failed (%s)\n",
                    snd_strerror (err));
                exit (1);
            }
        }
    }
    while (actual != 0);
    printf("Wrote: %ld shorts\n", total);

    free(buf);

    snd_pcm_drain(playback_handle);

    snd_pcm_close (playback_handle);
}

int16_t* recordsound()
{
    int                  i;
    int                  err;
    short*               buf;
    int16_t*             result;

    snd_pcm_t*           capture_handle;
    snd_pcm_hw_params_t* hw_params;

    if ((err = snd_pcm_open (&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        fprintf (stderr, "cannot open audio device %s (%s)\n",
            device,
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf (stderr, "cannot set access type (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        fprintf (stderr, "cannot set sample format (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    unsigned int rate = 44100;

    if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0)
    {
        fprintf (stderr, "cannot set sample rate (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0)
    {
        fprintf (stderr, "cannot set channel count (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0)
    {
        fprintf (stderr, "cannot set parameters (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    snd_pcm_hw_params_free (hw_params);

    if ((err = snd_pcm_prepare (capture_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
            snd_strerror (err));
        exit (1);
    }

    snd_pcm_uframes_t frames   = 1024;
    int               buf_size = sizeof(short) * frames * 2;
    buf    = malloc(buf_size);
    result = (int16_t*)malloc(sizeof(int16_t) * frames * 500);

    SF_INFO  info = {
        .frames     = frames * 500 * 2, // two channels
        .samplerate = 44100,
        .channels   = 2,
        .format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE,
        .sections   = 0,
        .seekable   = 0
    };

    SNDFILE* fp = sf_open("rec.wav", SFM_WRITE, &info);

    for (i = 0; i < 500; ++i)
    {
        if ((err = snd_pcm_readi (capture_handle, buf, frames)) != frames)
        {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                snd_strerror (err));
            exit (1);
        }
        sf_write_short(fp, buf, frames * 2);

        for (int j = 0; j < buf_size / 2; j++)
        {
            result[i * frames + j] = buf[j * 2];
        }
    }

    sf_close(fp);

    snd_pcm_close (capture_handle);
    return result;
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
    return mixer;
}

void mixer_enable_capture(Mixer* mixer)
{
    snd_mixer_selem_set_capture_switch(mixer->elem, SND_MIXER_SCHN_MONO, 1);
}

void mixer_close(Mixer* mixer)
{
    snd_mixer_selem_id_free(mixer->sid);
    snd_mixer_close(mixer->handle);
    free(mixer);
}