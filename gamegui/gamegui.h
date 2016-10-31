#ifndef GAMEGUI_H_INCLUDED
#define GAMEGUI_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "screen.h"
#include "button.h"

#define UNUSED(x) (void)(x)

bool GGInit(int* argc, char*** argv);
void GGQuit();
const char* GGLastError();
void GGSetLastError(char* fmt, ...);
int GGStart(GGScreen* screen);

#endif // GAMEGUI_H_INCLUDED