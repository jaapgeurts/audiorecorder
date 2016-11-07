#ifndef LABEL_H_INCLUDED
#define LABEL_H_INCLUDED

#include "screen.h"
#include "widget.h"

typedef struct GGLabel GGLabel;


GGLabel* GGLabelCreate(GGScreen* screen, const char* label, int left, int top,  int width, int height);
void GGLabelDestroy(GGLabel* button);

void GGLabelRender(GGWidget* widget, SDL_Renderer* renderer);
void GGLabelSetLabel(GGLabel* label, const char* text);

#endif // BUTTON_H_INCLUDED