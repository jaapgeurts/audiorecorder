#include "gamegui/gamegui.h"

#include "audio.h"

#ifdef GCW0
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    true
#else
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480
#define FULLSCREEN    false
#endif

void render_bg(SDL_Renderer* renderer)
{
    printf("Rendering background\n");
    SDL_SetRenderDrawColor(renderer, 0x2f, 0x2f, 0x2f, 0xff);
    SDL_RenderClear(renderer);
}

int main(int argc, char** argv)
{
    GGScreen* screen;

    screen = GGScreenCreate("Audio Recorder", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);

    GGScreenSetBackgroundRenderFunc(screen, render_bg);

    GGButton* button1 = GGButtonCreate(screen, "Exit", 10, 10, 100, 30);
    GGButton* button2 = GGButtonCreate(screen, "Record", 120, 10, 100, 30);
    GGButton* button3 = GGButtonCreate(screen, "Play", 230, 10, 100, 30);
    GGButton* button4 = GGButtonCreate(screen, "Back", 10, 50, 100, 30);
    
    // playsound();

    //recordsound();

    printf("recording done!\n");

    GGStart(screen);

    GGScreenDestroy(screen);

    return 0;
}