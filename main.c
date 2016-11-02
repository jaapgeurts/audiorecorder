#include "gamegui/gamegui.h"

#include "waveformwidget.h"
#include "helpbarwidget.h"

#include "audio.h"

#ifdef GCW0
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    true
#else
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    false
#endif

GGWaveform* wfw;

SDL_Color   dark_gray = {0x2f, 0x2f, 0x2f, 0xff};

void render_bg(SDL_Renderer* renderer)
{
    // printf("Rendering background\n");
    SDL_SetRenderDrawColor(renderer, dark_gray.r, dark_gray.g, dark_gray.b, dark_gray.a);
    SDL_RenderClear(renderer);
}

bool on_exit_click(GGWidget* widget, SDL_Event* event)
{
    GGButton* button1 = (GGButton*)widget;

    printf("Exit was called\n");
    return true;
}

bool on_record_click(GGWidget* widget, SDL_Event* event)
{
    printf("Start recording\n");

    int16_t* data = recordsound();

    printf("Recording done\n");

    GGWaveformSetData(wfw, data, 1024 * 500);

    return true;
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

    GGButton* button1 = GGButtonCreate(screen, "Exit", 10, 10, 70, 30);
    GGButtonSetOnClickFunc(button1, on_exit_click);

    GGButton* button2 = GGButtonCreate(screen, "Record", 90, 10, 70, 30);
    GGButtonSetOnClickFunc(button2, on_record_click);

    GGButton*      button3 = GGButtonCreate(screen, "Play", 170, 10, 70, 30);

    GGImageButton* imgbtn1 = GGImageButtonCreate(screen, "assets/mic.png", 30, 50, 30, 30);
    GGImageButton* imgbtn2 = GGImageButtonCreate(screen, "assets/record.png", 70, 50, 30, 30);
    GGImageButton* imgbtn3 = GGImageButtonCreate(screen, "assets/play.png", 110, 50, 30, 30);
    GGImageButton* imgbtn4 = GGImageButtonCreate(screen, "assets/btn_dpad.png", 150, 50, 30, 30);
    GGImageButton* imgbtn5 = GGImageButtonCreate(screen, "assets/replay.png", 190, 50, 30, 30);
    GGImageButton* imgbtn6 = GGImageButtonCreate(screen, "assets/stop.png", 230, 50, 30, 30);

    wfw = GGWaveformCreate(screen, 10, 90, 300, 50);

    GGHelpBar* helpbar1 = GGHelpBarCreate(screen);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_A, "Record");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD, "Change button");
    
    // playsound();

    GGStart(screen);

    GGScreenDestroy(screen);

    return 0;
}