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

#define FRAMERATE   5
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
    return true;
}

void GGQUit()
{
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

        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
                case SDL_QUIT:
                    done = true;
                    break;
            } // end switch

            // dispatch events
        } // end of message processing

        // clear the screen
        GGScreenClear(screen);

        // render all

        ticks = SDL_GetTicks() - ticks;

        if (ticks < 1000 / FRAMERATE)
        {
            SDL_Delay(1000 / FRAMERATE - ticks);
        }

        GGScreenRender(screen);
    } // end main loop

    return 0;
}