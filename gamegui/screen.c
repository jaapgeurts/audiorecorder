/*
 Copyright © 2016 Jaap Geurts

 Permission to use, copy, modify, distribute, and sell this software and its
 documentation for any purpose is hereby granted without fee, provided that
 the above copyright notice appear in all copies and that both that
 copyright notice and this permission notice appear in supporting
 documentation.  No representations are made about the suitability of this
 software for any purpose.  It is provided "as is" without express or
 implied warranty.

 This file is part of the gamegui library.

 This file is the "root" window of the display. The main window is created here.
 
 A screen has only one window. There can only be one screen during the application's run.
  
 Multiple screens/windows monitors are not supported

*/

#include <glib.h>

#include "gamegui.h"
#include "screen.h"


struct GGScreen
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Color bg_color;

};

/*
 Creates the screen for the app. Setups up the SDL window and renderer
 */
GGScreen* GGScreenCreate(const char* title, int width, int height, bool fullscreen)
{

    GGScreen* screen = calloc(1,sizeof(GGScreen));
    // create a new window
    screen->window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width,height,
                                      fullscreen?SDL_WINDOW_FULLSCREEN |
                                      SDL_WINDOW_OPENGL:SDL_WINDOW_OPENGL);
    if (!screen->window) {
        free(screen);
        GGSetLastError("%s",SDL_GetError());
        return NULL;
    }

    /* create the renderer */
    screen->renderer = SDL_CreateRenderer(screen->window,-1,SDL_RENDERER_ACCELERATED);
    if (!screen->renderer)
    {
        SDL_DestroyWindow(screen->window);
        GGSetLastError("%s",SDL_GetError());
        free(screen);
        return NULL;
    }

    return screen;
}

void GGScreenDestroy(GGScreen* screen)
{
    SDL_DestroyRenderer(screen->renderer);
    SDL_DestroyWindow(screen->window);
    free(screen);
}

void GGScreenRender(GGScreen* screen)
{
    SDL_RenderPresent(screen->renderer);
}

void GGScreenSetBackground(GGScreen* screen, SDL_Color* color)
{
    screen->bg_color = *color;
}

void GGScreenClear(GGScreen* screen)
{
    SDL_SetRenderDrawColor(screen->renderer,screen->bg_color.r,screen->bg_color.g,screen->bg_color.b,screen->bg_color.a);
    SDL_RenderClear(screen->renderer);
}
