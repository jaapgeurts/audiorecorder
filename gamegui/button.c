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

    char*        label;
    SDL_Texture* label_texture;
    int          label_w;
    int          label_h;
    
    GGEventFunc  on_click;
};

static bool GGButtonHandleEvent(GGWidget* widget, SDL_Event* event);

GGButton* GGButtonCreate(GGScreen* screen, const char* label, int left, int top,  int width, int height)
{
    GGButton* btn = (GGButton*)calloc(1,sizeof(GGButton));

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

    SDL_Color white = { 0xff, 0xff, 0xff, 0xff };
    SDL_Color gray = { 0xa0, 0xa0, 0xa0, 0xa0 };
    SDL_Color red   = {0xff, 0x00, 0x00, 0xff};

    SDL_Color color = widget->has_focus ? red : gray;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {
        button->widget.left,
        button->widget.top,
        button->widget.width,
        button->widget.height
    };
    SDL_RenderDrawRect(renderer, &rect);

    if (button->label_texture == NULL)
    {
        // render only once
        // cache these variables
        SDL_Surface* surface = TTF_RenderText_Blended(GGScreenSystemFont(button->screen), button->label, white);
        button->label_w = surface->w;
        button->label_h = surface->h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        button->label_texture = texture;
    }
    SDL_Rect textrect = {
        button->widget.left + (rect.w - button->label_w) / 2,
        button->widget.top + (rect.h - button->label_h) / 2,
        button->label_w,
        button->label_h
    };
    SDL_RenderCopy(renderer, button->label_texture, NULL, &textrect);
}

void GGButtonDestroy(GGButton* button)
{
    if (button->label_texture)
        SDL_DestroyTexture(button->label_texture);
    free(button->label);
    free(button);
}

static bool GGButtonHandleEvent(GGWidget* widget, SDL_Event* event)
{
    GGButton* button  = (GGButton*)widget;
    bool      handled = false;

    if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
            case PRIMARY_ACTION:
                if (button->on_click != NULL)
                    handled = button->on_click(widget,event);
        }
    }

    return handled;
}

void GGButtonSetOnClickFunc(GGButton* button, GGEventFunc on_click)
{
    button->on_click = on_click;
}