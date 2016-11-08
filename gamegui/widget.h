#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct GGWidget GGWidget;

typedef bool (*GGEventFunc)(GGWidget* widget, SDL_Event* event);

typedef void (*GGWidgetChangeFunc)(GGWidget* widget);

struct GGWidget
{
    int left;
    int top;
    int width;
    int height;

    bool has_focus;
    bool accepts_focus;
    
    bool is_dirty;
    bool is_enabled;
    bool is_visible;
    
    TTF_Font* font;
    
    SDL_Color color_white;
    SDL_Color color_gray;
    SDL_Color color_dark_gray;
    SDL_Color color_red;  

    void (* render_func)(GGWidget* widget, SDL_Renderer* renderer);
    GGEventFunc handle_event_func;
    
    GGWidgetChangeFunc focus_gained_func;
    GGWidgetChangeFunc focus_lost_func;
};

void GGWidgetInit(GGWidget* widget, int left, int top, int width, int height);

void GGWidgetSetFont(GGWidget* widget, TTF_Font* font);
void GGWidgetSetEnabled(GGWidget* widget,bool state);
void GGWidgetSetVisible(GGWidget* widget, bool state);


void GGWidgetRepaint(GGWidget* widget);

#endif // WIDGET_H_INCLUDED