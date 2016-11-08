#include <time.h>

#include "gamegui/gamegui.h"

#include "waveformwidget.h"
#include "vumeter.h"

#include "audio.h"

#ifdef GCW0

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    true

#else  // NOT GCW0

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    false

#endif // GCW0

enum
{
    ERR_NO_AUDIO = 1,
    ERR_NO_MIXER,
    ERR_INIT_SDL
}
app_error_codes;

/* Define a few globals */
#define OUTPUT_FILENAME_SIZE 50
char output_filename[OUTPUT_FILENAME_SIZE];

/* AUDIO RELATED variables */
PCM_Play*    play;
PCM_Capture* capture;

// Default recording settings
unsigned int sample_rate = 44100;
int          bitdepth    = 16;
Mixer*       mixer_mic   = NULL;
Mixer*       mixer_pcm   = NULL;
#define VOLUME_STEP 0.1

// Count of frames to record at each call
// for now equal to the sample freq
#define FRAME_COUNT 44100
#define CHANNELS    1
int16_t* data[FRAME_COUNT * CHANNELS];
int      audio_frames_sent = 0;
float    current_volume    = 0.8;
bool     playing           = false;
bool     recording         = false;
int      frame_count       = 16;

/* UI Related variables */
GGScreen*      screen = NULL;
GGWaveform*    wfw;
GGLabel*       lbl_title  = NULL;
GGImageButton* btn_exit   = NULL;
GGLabel*       lbl_file   = NULL;
GGButton*      btn_new    = NULL;
GGImageButton* btn_record = NULL;
GGImageButton* btn_play   = NULL;
GGImageButton* btn_stop   = NULL;
GGImageButton* btn_replay = NULL;
GGImageButton* btn_mic    = NULL;
GGVUMeter*     vumeter    = NULL;
GGHelpBar*     helpbar1   = NULL;

SDL_Color      dark_gray = {0x2f, 0x2f, 0x2f, 0xff};

/** UI FUNCTIONS **/
static void render_bg(SDL_Renderer* renderer)
{
    // printf("Rendering background\n");
    SDL_SetRenderDrawColor(renderer, dark_gray.r, dark_gray.g, dark_gray.b, dark_gray.a);
    SDL_RenderClear(renderer);
}

static bool on_exit_click(GGWidget* widget, SDL_Event* event)
{
    GGStop();

    return true;
}

static void new_filename(char* buf, int size)
{
    time_t     rawtime;
    struct tm* timeinfo;

    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf, size, "AUD_%Y%m%d_%H%M%S.wav", timeinfo);
}

static bool on_new_click(GGWidget* widget, SDL_Event* event)
{
    GGButton* button = (GGButton*)widget;

    if (data)
    {
        free(data);
        data = NULL;
    }
    GGWidgetSetEnabled((GGWidget*)btn_replay, true);
    GGWidgetSetEnabled((GGWidget*)btn_new, true);

    new_filename(output_filename, OUTPUT_FILENAME_SIZE);
    GGLabelSetLabel(lbl_file, output_filename);
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_record);

    return true;
}

static void vumeter_focus_gained(GGWidget* widget)
{
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, "Change level");
}

static void vumeter_focus_lost(GGWidget* widget)
{
    // ↔ ↕  : ⇔ ⇕ : ⬄ ⇳
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, NULL);
}

static void vumeter_grab_dpad(GGWidget* widget)
{
    // ↔ ↕  : ⇔ ⇕ : ⬄ ⇳
    GGHelpBarSetHelp(helpbar1, GCW_BTN_X, "Release");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_ALL, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_UD, "Inc/Dec Level");
}

static void vumeter_release_dpad(GGWidget* widget)
{
    GGHelpBarSetHelp(helpbar1, GCW_BTN_X, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, "Change level");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_ALL, "Navigate");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_UD, NULL);
}

/** AUDIO FUNCTIONS **/

static bool on_record_stop(GGWidget* widget, SDL_Event* event)
{
    GGWidgetSetVisible((GGWidget*)btn_play, true);
    GGWidgetSetVisible((GGWidget*)btn_stop, false);

    GGWidgetSetEnabled((GGWidget*)btn_new, false);
    GGWidgetSetEnabled((GGWidget*)btn_play, true);
    GGWidgetSetEnabled((GGWidget*)btn_mic, true);

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_play);
    return true;
}

static bool on_record_click(GGWidget* widget, SDL_Event* event)
{
    capture_prepare(capture);

    printf("Start recording\n");
    recording = true;

    GGWidgetSetVisible((GGWidget*)btn_play, false);
    GGWidgetSetVisible((GGWidget*)btn_stop, true);
    GGWidgetSetEnabled((GGWidget*)btn_record, false);
    GGWidgetSetEnabled((GGWidget*)btn_mic, false);
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_stop);
    GGImageButtonSetOnClickFunc(btn_stop, on_record_stop);

    int actual = capture_record(PCM_Capture * capture, (void*)data, FRAME_COUNT );

    printf("Recording done\n");

    on_record_stop(widget, event);

    GGWaveformSetData(wfw, data, 1024 * 500);

    return true;
}

static bool on_play_stop(GGWidget* widget, SDL_Event* event)
{
    GGWidgetSetVisible((GGWidget*)btn_play, true);
    GGWidgetSetVisible((GGWidget*)btn_stop, false);

    GGWidgetSetEnabled((GGWidget*)btn_new, true);
    GGWidgetSetEnabled((GGWidget*)btn_mic, true);

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_play);

    playback_stop(play);

    return true;
}

static void send_audio(PCM_Play* play,  int16_t* buf, int cnt)
{
    printf("Send next batch\n");

    if (playback_play(play, buf, cnt))
        audio_frames_sent++;
}

static bool on_play_click(GGWidget* widget, SDL_Event* event)
{
    playback_prepare(play);

    printf("Start playback\n");
    playing = true;

    audio_frames_sent = 0;
    GGWidgetSetVisible((GGWidget*)btn_play, false);
    GGWidgetSetVisible((GGWidget*)btn_stop, true);
    GGImageButtonSetOnClickFunc(btn_stop, on_play_stop);

    GGWidgetSetEnabled((GGWidget*)btn_mic, false);
    GGWidgetSetEnabled((GGWidget*)btn_replay, true);

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_stop);

    // Send inital frame
    send_audio(play, &data[audio_frames_sent * play->frames], play->frames );
    send_audio(play, &data[audio_frames_sent * play->frames], play->frames );

    return true;
}

void check_audio(GGScreen* screen)
{
    if (!playing)
        return;

    if (!playback_ready(play))
    {
        printf("Play not ready for next batch\n");
        return;
    }

    if (audio_frames_sent  < (1024 * 500) / play->frames)
    {
        send_audio(play, &data[audio_frames_sent * play->frames], play->frames );
    }
    else
    {
        if (snd_pcm_avail(play->playback_handle) <= 0)
        {
            printf("Audio drained..... done\n");
            on_play_stop(NULL, NULL);
            playing = false;
        }
    }
}

static void vumeter_volume_up(GGWidget* widget)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;

    current_volume += VOLUME_STEP;

    if (current_volume > 1)
        current_volume = 1;
    mixer_set_volume(mixer_mic, current_volume);
    GGVUMeterSetVolume(vumeter, current_volume);
}

static void vumeter_volume_down(GGWidget* widget)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;

    current_volume -= VOLUME_STEP;

    if (current_volume < 0)
        current_volume = 0;
    mixer_set_volume(mixer_mic, current_volume);
    GGVUMeterSetVolume(vumeter, current_volume);
}

int main(int argc, char** argv)
{
    // Open all audio interfaces
    play = playback_open(PLAYBACK_DEVICE, sample_rate, bitdepth);

    if (play == NULL)
    {
        printf("Can't open audio device %s for playback\n", PLAYBACK_DEVICE);
        return ERR_NO_AUDIO;
    }
    mixer_mic = mixer_open(MIXER_MIC);

    // mixer_pcm = mixer_open(MIXER_PCM);
    if (mixer_mic == NULL)
    {
        printf("Can't open mixer %s for playback\n", PLAYBACK_DEVICE);
        return ERR_NO_MIXER;
    }

    capture = capture_open(AUDIO_DEVICE, sample_rate, bitdepth);

    if (capture == NULL)
    {
        printf("Can't open device '%s' for capture\n", CAPTURE_DEVICE);
        return ERR_NO_AUDIO;
    }

    // Correct volume input for the microphone
    mixer_enable_capture(mixer_mic);
    current_volume = mixer_volume(mixer_mic);

    // Setup UI
    if (!GGInit(&argc, &argv))
    {
        fprintf(stderr, "Can't initialize (%s)\n", GGLastError());
        return ERR_INIT_SDL;
    }

    screen = GGScreenCreate("Audio Recorder", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);

    // Setup callbacks and hooks
    GGScreenSetBackgroundRenderFunc(screen, render_bg);

    GGScreenSetPreEventFunc(screen, check_audio);

    lbl_title = GGLabelCreate(screen, "GCW0 Audio Recorder", 10, 0, 70, 30);
    TTF_Font* font = TTF_OpenFont("assets/DroidSans.ttf", 16);
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    GGWidgetSetFont((GGWidget*)lbl_title, font);

    btn_exit = GGImageButtonCreate(screen, "assets/exit.png", 285, 5, 25, 25);
    GGImageButtonSetOnClickFunc(btn_exit, on_exit_click);

    new_filename(output_filename, OUTPUT_FILENAME_SIZE);
    lbl_file = GGLabelCreate(screen, output_filename, 10, 40, 260, 20);

    btn_new = GGButtonCreate(screen, "new", 230, 40, 40, 20);
    GGButtonSetOnClickFunc(btn_new, on_new_click);

    wfw = GGWaveformCreate(screen, 10, 70, 260, 50);

    btn_record = GGImageButtonCreate(screen, "assets/record.png", 10, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_record, on_record_click);

    btn_play = GGImageButtonCreate(screen, "assets/play.png", 55, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_play, on_play_click);

    btn_stop = GGImageButtonCreate(screen, "assets/stop.png", 55, 130, 30, 30);
    GGWidgetSetVisible((GGWidget*)btn_stop, false);

    btn_replay = GGImageButtonCreate(screen, "assets/replay.png", 95, 130, 30, 30);
    btn_mic    = GGImageButtonCreate(screen, "assets/mic.png", 280, 130, 30, 30);

    // VU Meter
    vumeter                           = GGVUMeterCreate(screen, 280, 40, 30, 80);
    vumeter->widget.focus_gained_func = vumeter_focus_gained;
    vumeter->widget.focus_lost_func   = vumeter_focus_lost;
    vumeter->volume_up_func           = vumeter_volume_up;
    vumeter->volume_down_func         = vumeter_volume_down;
    GGScreenSetGrabDPadCallBack(screen, vumeter_grab_dpad);
    GGScreenSetReleaseDPadCallBack(screen, vumeter_release_dpad);
    float vol = mixer_volume(mixer_mic);
    GGVUMeterSetVolume(vumeter, vol);

    GGVUMeterSetCurrent(vumeter, 1);

    // Helpbar
    helpbar1 = GGHelpBarCreate(screen);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_A, "Record");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_ALL, "Navigate");
    //    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELET, "Exit");

    // Set initial widget states
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_record);
    GGWidgetSetEnabled((GGWidget*)btn_play, false);
    GGWidgetSetEnabled((GGWidget*)btn_replay, false);
    GGWidgetSetEnabled((GGWidget*)btn_new, false);

    // playsound();

    GGStart(screen);

    GGScreenDestroy(screen);

    GGQuit();

    // These three can't be NULL or the program won't start
    capture_close(capture);
    mixer_close(mixer_mic);
    playback_close(play);

    if (data)
        free(data);

    return 0;
}