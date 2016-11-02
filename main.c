
#include <time.h>

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

int16_t* data;

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
    printf("Start recording\n");

    data = recordsound();

    printf("Recording done\n");

    GGWaveformSetData(wfw, data, 1024 * 500);

    return true;
}

static bool on_play_click(GGWidget* widget, SDL_Event* event)
{
    printf("Start playback\n");
    
    playsound(data, 1024 * 500);
    
    printf("playback done\n");
        
    return true;
}

static void new_filename(char* buf, int size)
{
    time_t rawtime;
    struct tm* timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,size,"AUD_%Y%m%d_%H%M%S.wav",timeinfo);
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

    GGLabel* lbl_title = GGLabelCreate(screen, "GCW0 Audio Recorder", 10, 0, 70, 30);
    TTF_Font* font = TTF_OpenFont("assets/DroidSans.ttf", 16);
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    GGWidgetSetFont((GGWidget*)lbl_title,font);
    
    GGImageButton* btn_exit = GGImageButtonCreate(screen, "assets/exit.png", 285, 5, 25, 25);
    GGImageButtonSetOnClickFunc(btn_exit, on_exit_click);
    
    char filename[50];
    new_filename(filename,50);
    GGLabel* lbl_file = GGLabelCreate(screen, filename, 10, 40,260 ,20);

    GGButton* btn_new = GGButtonCreate(screen,"new",270,40,40,20);
    
    wfw = GGWaveformCreate(screen, 10, 70, 300, 50);
    
    GGImageButton* btn_record = GGImageButtonCreate(screen, "assets/record.png", 10, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_record, on_record_click);

    GGImageButton* btn_play = GGImageButtonCreate(screen, "assets/play.png", 55, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_play, on_play_click);

    GGImageButton* btn_replay = GGImageButtonCreate(screen, "assets/replay.png", 95, 130, 30, 30);
    GGImageButton* btn_mic = GGImageButtonCreate(screen, "assets/mic.png", 280, 130, 30, 30);

    
    GGHelpBar* helpbar1 = GGHelpBarCreate(screen);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_A, "Record");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD, "Change button");
    
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_record);
    
    // playsound();

    GGStart(screen);

    GGScreenDestroy(screen);
    
    GGQuit();

    return 0;
}