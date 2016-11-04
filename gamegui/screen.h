#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "widget.h"

typedef struct GGScreen GGScreen;

GGScreen* GGScreenCreate(const char* title, int width, int height, bool fullscreen);
void GGScreenDestroy(GGScreen* screen);
void GGScreenRender(GGScreen* screen);
void GGScreenClear(GGScreen* screen);
void GGScreenSetBackgroundRenderFunc(GGScreen* screen, void (* render_func)(SDL_Renderer* renderer));

void GGScreenAddWidget(GGScreen* screen, GGWidget* widget);
int GGScreenGetHeight(GGScreen* screen);
int GGScreenGetWidth(GGScreen* screen);
int GGScreenGetDepth(GGScreen* screen);

void GGScreenSetFocusWidget(GGScreen* screen, GGWidget* widget);
void GGScreenGrabDPad(GGScreen* screen, GGWidget* widget);
void GGScreenReleaseDPad(GGScreen* screen, GGWidget* widget);

void GGScreenSetGrabDPadCallBack(GGScreen* screen, GGWidgetChangeFunc callback);
void GGScreenSetReleaseDPadCallBack(GGScreen* screen, GGWidgetChangeFunc callback);

TTF_Font* GGScreenSystemFont(GGScreen* screen);

bool GGScreenHandleEvent(GGScreen* screen, SDL_Event* event);

#endif // SCREEN_H_INCLUDED