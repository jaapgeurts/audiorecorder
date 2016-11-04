#include <time.h>

#include "gamegui/gamegui.h"

#include "waveformwidget.h"
#include "vumeter.h"
#include "helpbarwidget.h"

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

/* Define a few globals */

GGWaveform* wfw;

SDL_Color   dark_gray = {0x2f, 0x2f, 0x2f, 0xff};

/**
 * Default recording settings
 */
unsigned int sample_rate = 44100;
int          bitdepth    = 16;

int16_t*     data = NULL;

GGHelpBar*   helpbar1 = NULL;

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

static bool on_record_click(GGWidget* widget, SDL_Event* event)
{
    PCM_Capture* capture = capture_open(AUDIO_DEVICE, sample_rate, bitdepth);

    if (capture == NULL)
    {
        printf("Can't open device '%s' for capture\n", CAPTURE_DEVICE);
        return true;
    }

    printf("Start recording\n");

    if (data)
        free(data);

    data = recordsound(capture);

    printf("Recording done\n");

    capture_close(capture);

    GGWaveformSetData(wfw, data, 1024 * 500);

    return true;
}

static bool on_play_click(GGWidget* widget, SDL_Event* event)
{
    PCM_Play* play = playback_open(PLAYBACK_DEVICE, sample_rate, bitdepth);

    if (play == NULL)
    {
        printf("Can't open device %s for playback\n", PLAYBACK_DEVICE);
        return true;
    }

    printf("Start playback\n");

    playsound(play, data, 1024 * 500);

    printf("playback done\n");

    playback_close(play);

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
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD, "^v Inc/Dec Level");
}

static void vumeter_release_dpad(GGWidget* widget)
{
    GGHelpBarSetHelp(helpbar1, GCW_BTN_X, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, "Change level");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD, "Navigate");
}

int main(int argc, char** argv)
{
    GGScreen* screen;

    if (!GGInit(&argc, &argv))
    {
        fprintf(stderr, "Can't initialize (%s)\n", GGLastError());
        return -1;
    }

    screen = GGScreenCreate("Audio Recorder", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);

    GGScreenSetBackgroundRenderFunc(screen, render_bg);

    GGLabel*  lbl_title = GGLabelCreate(screen, "GCW0 Audio Recorder", 10, 0, 70, 30);
    TTF_Font* font      = TTF_OpenFont("assets/DroidSans.ttf", 16);
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    GGWidgetSetFont((GGWidget*)lbl_title, font);

    GGImageButton* btn_exit = GGImageButtonCreate(screen, "assets/exit.png", 285, 5, 25, 25);
    GGImageButtonSetOnClickFunc(btn_exit, on_exit_click);

    char      filename[50];
    new_filename(filename, 50);
    GGLabel*  lbl_file = GGLabelCreate(screen, filename, 10, 40, 260, 20);

    GGButton* btn_new = GGButtonCreate(screen, "new", 230, 40, 40, 20);

    wfw = GGWaveformCreate(screen, 10, 70, 260, 50);

    GGImageButton* btn_record = GGImageButtonCreate(screen, "assets/record.png", 10, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_record, on_record_click);

    GGImageButton* btn_play = GGImageButtonCreate(screen, "assets/play.png", 55, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_play, on_play_click);

    GGImageButton* btn_replay = GGImageButtonCreate(screen, "assets/replay.png", 95, 130, 30, 30);
    GGImageButton* btn_mic    = GGImageButtonCreate(screen, "assets/mic.png", 280, 130, 30, 30);

    GGVUMeter*     vumeter = GGVUMeterCreate(screen, 280, 40, 30, 80);
    vumeter->widget.focus_gained_func = vumeter_focus_gained;
    vumeter->widget.focus_lost_func   = vumeter_focus_lost;
    GGScreenSetGrabDPadCallBack(screen, vumeter_grab_dpad);
    GGScreenSetReleaseDPadCallBack(screen, vumeter_release_dpad);

    helpbar1 = GGHelpBarCreate(screen);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_A, "Record");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD, "Navigate");
    //    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELET, "Exit");

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_record);

    // playsound();

    Mixer* mixer = mixer_open(MIXER_MIC);

    if (mixer != NULL)
    {
        mixer_enable_capture(mixer);
        mixer_set_volume(mixer, 25);
    }

    GGStart(screen);

    if (mixer)
        mixer_close(mixer);

    GGScreenDestroy(screen);

    GGQuit();

    if (data)
        free(data);

    return 0;
}