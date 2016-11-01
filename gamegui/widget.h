#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

typedef struct GGWidget GGWidget;

typedef bool (*GGEventFunc)(GGWidget* widget, SDL_Event* event);

struct GGWidget
{
    int left;
    int top;
    int width;
    int height;

    bool has_focus;
    bool accepts_focus;

    void (* render_func)(GGWidget* widget, SDL_Renderer* renderer);
    GGEventFunc handle_event_func;
};

void GGWidgetInit(GGWidget* widget, int left, int top, int width, int height);

void GGWidgetSetFocus(GGWidget* widget, bool has_focus);

#endif // WIDGET_H_INCLUDED