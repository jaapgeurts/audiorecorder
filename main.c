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
    SDL_SetRenderDrawColor(renderer, 0xd6, 0xd2, 0xd0, 0xff);
    SDL_RenderClear(renderer);
}

int main(int argc, char** argv)
{
    GGScreen* screen;

    screen = GGScreenCreate("Audio Recorder", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);

    GGScreenSetBackgroundRenderFunc(screen, render_bg);

    GGButton* button = GGButtonCreate(screen, "Exit", 10, 10, 100, 30);

    // playsound();

    //recordsound();

    printf("recording done!\n");

    GGStart(screen);

    GGScreenDestroy(screen);

    return 0;
}