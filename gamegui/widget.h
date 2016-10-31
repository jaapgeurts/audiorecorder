#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    int left;
    int top;
    int width;
    int height;

    bool has_focus;
    bool is_dirty;
    
    void (*)(
} GGWidget;

void GGWidgetInit(int left, int top, int width, int height);

#endif // WIDGET_H_INCLUDED