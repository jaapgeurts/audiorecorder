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

   This is a simple gui system for SDL2. It doesn't do much except create regions for
   controls and pass events. The regions can keep a dirty state so that the system knows
   when to call paint on what controls.

   This file contains the Initialization code and the main event loop.

   Window/screen management is in screen.c

 */

#include <stdarg.h>
#include "gamegui.h"

#define FRAMERATE   15
#define ERR_BUF_LEN 255

static char lasterror[ERR_BUF_LEN] = {0};

bool GGInit(int* argc, char*** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        GGSetLastError("%s", SDL_GetError());
        return false;
    }

    if (TTF_Init() != 0)
    {
        GGSetLastError("%s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        TTF_Quit();
        SDL_Quit();
        GGSetLastError("%s", SDL_GetError());
        return false;
    }

    return true;
}

void GGQuit()
{
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

const char* GGLastError()
{
    return lasterror;
}

void GGSetLastError(char* fmt, ...)
{
    va_list argp;

    va_start(argp, fmt);
    vsnprintf(lasterror, ERR_BUF_LEN, fmt, argp);
    va_end(argp);
}

/*
   The main event loop is here.
 */
int GGStart(GGScreen* screen)
{
    // program main loop
    bool done = false;

    while (!done)
    {
        Uint32    ticks = SDL_GetTicks();
        // message processing loop
        SDL_Event event;

        GGScreenHandlePreEventFunc(screen);

        while (SDL_PollEvent(&event))
        {
            // deliver the event to object that has focus
            if (!GGScreenHandleEvent(screen, &event))
            {
                // check for messages
                switch (event.type)
                {
                    // exit if the window is closed
                    case SDL_QUIT:
                        done = true;
                        break;
                } // end switch
            }
            // dispatch events
        } // end of message processing

        GGScreenHandlePostEventFunc(screen);

        GGScreenHandlePreRenderFunc(screen);

        // render all
        GGScreenRender(screen);

        GGScreenHandlePostRenderFunc(screen);

        ticks = SDL_GetTicks() - ticks;

        // printf("Rendering & Event processing took %d milliseconds.\n",ticks);

        if (ticks < 1000 / FRAMERATE)
        {
            SDL_Delay(1000 / FRAMERATE - ticks);
        }
        else
        {
            printf("Warning: framerate of %d not achieved. Actual: %d\n", FRAMERATE, 1000 / ticks);
        }
    } // end main loop

    return 0;
}

void GGStop()
{
    SDL_Event event;

    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}