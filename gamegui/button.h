#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include "screen.h"
#include "widget.h"

typedef struct GGButton GGButton;


GGButton* GGButtonCreate(GGScreen* screen, const char* label, int left, int top,  int width, int height);
void GGButtonDestroy(GGButton* button);

void GGButtonRender(GGWidget* widget, SDL_Renderer* renderer);
void GGButtonSetLabel(GGButton* button, const char* label);

void GGButtonSetOnClickFunc(GGButton* button, GGEventFunc on_click);

#endif // BUTTON_H_INCLUDED