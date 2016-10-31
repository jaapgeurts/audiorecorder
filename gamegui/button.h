#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include "screen.h"
#include "widget.h"

typedef struct GGButton GGButton;

GGButton* GGButtonCreate(GGScreen* screen, const char* label, int top, int left, int width, int height);
void GGButtonDestroy(GGButton* button);

void GGButtonRender(GGWidget* widget, SDL_Renderer* renderer);

#endif // BUTTON_H_INCLUDED