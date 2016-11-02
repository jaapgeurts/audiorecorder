#ifndef IMAGEBUTTON_H_INCLUDED
#define IMAGEBUTTON_H_INCLUDED

#include "screen.h"
#include "widget.h"

typedef struct GGImageButton GGImageButton;

GGImageButton* GGImageButtonCreate(GGScreen* screen, const char* filename, int left, int top,  int width, int height);
void GGImageButtonDestroy(GGImageButton* button);

void GGImageButtonRender(GGWidget* widget, SDL_Renderer* renderer);

void GGImageButtonSetOnClickFunc(GGImageButton* button, GGEventFunc on_click);

#endif // IMAGEBUTTON_H_INCLUDED