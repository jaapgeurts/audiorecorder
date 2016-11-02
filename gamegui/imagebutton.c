/*
 *  Copyright © 2016 Jaap Geurts
 * 
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appear in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation.  No representations are made about the suitability of this
 *  software for any purpose.  It is provided "as is" without express or
 *  implied warranty.
 * 
 *  This file is part of the gamegui library.
 * 
 *  This is a standard push button.
 */

#include <SDL2/SDL_image.h>

#include "imagebutton.h"
#include "keymapping.h"

struct GGImageButton
{
    GGWidget  widget;
    GGScreen* screen;
    
    SDL_Surface* icon;
    SDL_Texture* icon_texture;
    int          icon_w;
    int          icon_h;
    
    GGEventFunc  on_click;
};

static bool GGImageButtonHandleEvent(GGWidget* widget, SDL_Event* event);

GGImageButton* GGImageButtonCreate(GGScreen* screen, const char* filename, int left, int top,  int width, int height)
{
    GGImageButton* btn = (GGImageButton*)calloc(1,sizeof(GGImageButton));
    
    GGWidgetInit(&btn->widget, left, top, width, height);
    btn->widget.render_func       = GGImageButtonRender;
    btn->widget.handle_event_func = GGImageButtonHandleEvent;
    btn->screen                   = screen;
    btn->icon = IMG_Load(filename);
    if (!btn->icon) {
        fprintf(stderr,"Can't load icon %s: (%s)\n",filename, SDL_GetError());
        free(btn);
        return NULL;
    }
    btn->icon_w = btn->icon->w;
    btn->icon_h = btn->icon->h;
    // add it to the screen
    
    GGScreenAddWidget(screen, &btn->widget);
    
    return btn;
}

void GGImageButtonRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGImageButton* button = (GGImageButton*)widget;
    
    SDL_Color white = { 0xff, 0xff, 0xff, 0xff };
    SDL_Color red   = {0xff, 0x00, 0x00, 0xff};
    
    SDL_Color color = widget->has_focus ? red : white;
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {
        button->widget.left,
        button->widget.top,
        button->widget.width,
        button->widget.height
    };
    SDL_RenderDrawRect(renderer, &rect);
    
    if (button->icon_texture == NULL)
    {
        // render only once
        // cache these variables
        button->icon_texture = SDL_CreateTextureFromSurface(renderer,button->icon);
        SDL_FreeSurface(button->icon);
        button->icon = NULL;
    }
    SDL_Rect iconrect = {
        button->widget.left + (rect.w - button->icon_w) / 2,
        button->widget.top + (rect.h - button->icon_h) / 2,
        button->icon_w,
        button->icon_h
    };
    SDL_RenderCopy(renderer, button->icon_texture, NULL, &iconrect);
}

void GGImageButtonDestroy(GGImageButton* button)
{
    if (button->icon)
        SDL_FreeSurface(button->icon);
    if (button->icon_texture)
        SDL_DestroyTexture(button->icon_texture);
    free(button);
}

static bool GGImageButtonHandleEvent(GGWidget* widget, SDL_Event* event)
{
    GGImageButton* button  = (GGImageButton*)widget;
    bool      handled = false;
    
    if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
            case PRIMARY_ACTION:
                if (button->on_click != NULL)
                    handled = button->on_click(widget,event);
                break;
        }
    }
    
    return handled;
}

void GGImageButtonSetOnClickFunc(GGImageButton* button, GGEventFunc on_click)
{
    button->on_click = on_click;
}