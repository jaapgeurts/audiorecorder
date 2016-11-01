#include "gamegui/gamegui.h"

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

void render_bg(SDL_Renderer* renderer)
{
   // printf("Rendering background\n");
    SDL_SetRenderDrawColor(renderer, 0x2f, 0x2f, 0x2f, 0xff);
    SDL_RenderClear(renderer);
}

bool on_button1_click(GGWidget* widget, SDL_Event* event)
{
    GGButton* button1 = (GGButton*)widget;
    printf("Exit was called\n");
    return true;
}

int main(int argc, char** argv)
{
    GGScreen* screen;
    
    if (!GGInit(&argc,&argv)) {
        fprintf(stderr,"Can't initialize (%s)\n",GGLastError());
        return -1;
    }

    screen = GGScreenCreate("Audio Recorder", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);

    GGScreenSetBackgroundRenderFunc(screen, render_bg);

    GGButton* button1 = GGButtonCreate(screen, "Exit", 10, 10, 100, 30);
    GGButtonSetOnClickFunc(button1,on_button1_click);
    
    GGButton* button2 = GGButtonCreate(screen, "Record", 120, 10, 100, 30);
    GGButton* button3 = GGButtonCreate(screen, "Play", 230, 10, 100, 30);
    GGButton* button4 = GGButtonCreate(screen, "Back", 10, 50, 100, 30);
    
    GGImageButton* imgbtn1 = GGImageButtonCreate(screen,"assets/mic.png",30,100,30,30);
    GGImageButton* imgbtn2 = GGImageButtonCreate(screen,"assets/record.png",70,100,30,30);
    GGImageButton* imgbtn3 = GGImageButtonCreate(screen,"assets/play.png",110,100,30,30);
    GGImageButton* imgbtn4 = GGImageButtonCreate(screen,"assets/dpad.png",150,100,30,30);
    GGImageButton* imgbtn5 = GGImageButtonCreate(screen,"assets/replay.png",190,100,30,30);
    GGImageButton* imgbtn6 = GGImageButtonCreate(screen,"assets/stop.png",230,100,30,30);
    
    // playsound();
    
     

    // recordsound();

    printf("recording done!\n");

    GGStart(screen);

    GGScreenDestroy(screen);

    return 0;
}