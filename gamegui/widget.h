#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "gamegui.h"

typedef struct
{
    int left;
    int top;
    int width;
    int height;

    bool has_focus;
    bool is_dirty;
    
    void (*render_func)(SDL_Renderer* renderer);
} GGWidget;

void GGWidgetInit(GGWidget* widget, int left, int top, int width, int height);

#endif // WIDGET_H_INCLUDED