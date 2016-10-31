/*
   Copyright © 2016 Jaap Geurts

   Permission to use, copy, modify, distribute, and sell this software and its
   documentation for any purpose is hereby granted without fee, provided that
   the above copyright notice appear in all copies and that both that
   copyright notice and this permission notice appear in supporting
   documentation.  No representations are made about the suitability of this
   software for any purpose.  It is provided "as is" without express or
   implied warranty.

   This file is part of the gamegui library.

   This is a standard push button.
 */

#include "button.h"
#include "keymapping.h"

struct GGButton
{
    GGWidget  widget;
    GGScreen* screen;

    char* label;
};

static bool GGButtonHandleEvent(GGWidget* widget, SDL_Event* event);

GGButton* GGButtonCreate(GGScreen* screen, const char* label, int left, int top,  int width, int height)
{
    GGButton* btn = (GGButton*)malloc(sizeof(GGButton));

    GGWidgetInit(&btn->widget, left, top, width, height);
    btn->label                    = strdup(label);
    btn->widget.render_func       = GGButtonRender;
    btn->widget.handle_event_func = GGButtonHandleEvent;
    btn->screen                   = screen;

    // add it to the screen

    GGScreenAddWidget(screen, &btn->widget);

    return btn;
}

void GGButtonRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGButton* button = (GGButton*)widget;

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_Rect rect = {
        button->widget.top,
        button->widget.left,
        button->widget.width,
        button->widget.height
    };
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Color    white = { 0xff, 0xff, 0xff, 0xff };
    // cache these variables
    SDL_Surface* surface = TTF_RenderText_Blended(GGScreenSystemFont(button->screen), button->label, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void GGButtonDestroy(GGButton* button)
{
    free(button->label);
    free(button);
}

static bool GGButtonHandleEvent(GGWidget* widget, SDL_Event* event)
{
    GGButton* button = (GGButton*)widget;
    bool handled = false;
    
    if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
            case PRIMARY_ACTION:
                handled = true;
                printf("Button: %s, A button pressed\n",button->label);
                break;
        }
    }
    
    return handled;
}