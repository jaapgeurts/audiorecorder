#ifndef GAMEGUI_H_INCLUDED
#define GAMEGUI_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "screen.h"
#include "button.h"
#include "label.h"
#include "helpbar.h"
#include "imagebutton.h"
#include "keymapping.h"

#define UNUSED(x) (void)(x)

bool GGInit(int* argc, char*** argv);
void GGQuit();
const char* GGLastError();
void GGSetLastError(char* fmt, ...);
int GGStart(GGScreen* screen);
void GGStop();

#endif // GAMEGUI_H_INCLUDED