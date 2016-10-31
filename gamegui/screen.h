#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct GGScreen GGScreen;

GGScreen* GGScreenCreate(const char* title, int width, int height, bool fullscreen);
void GGScreenDestroy(GGScreen* screen);
void GGScreenRender(GGScreen* screen);
void GGScreenClear(GGScreen* screen);
void GGScreenSetBackground(GGScreen* screen, SDL_Color* color);


#endif // SCREEN_H_INCLUDED
