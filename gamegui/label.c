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
 *  This is a standard label
 */

#include "label.h"
#include "keymapping.h"

struct GGLabel
{
    GGWidget  widget;
    GGScreen* screen;
    
    char*        label;
    SDL_Texture* label_texture;
    int          label_w;
    int          label_h;
    
};

GGLabel* GGLabelCreate(GGScreen* screen, const char* label, int left, int top,  int width, int height)
{
    GGLabel* lbl = (GGLabel*)calloc(1,sizeof(GGLabel));
    
    GGWidgetInit(&lbl->widget, left, top, width, height);
    lbl->label                    = strdup(label);
    lbl->widget.render_func       = GGLabelRender;
    lbl->screen                   = screen;
    lbl->widget.accepts_focus = false;
    
    // add it to the screen
    
    GGScreenAddWidget(screen, &lbl->widget);
    
    return lbl;
}

void GGLabelRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGLabel* label = (GGLabel*)widget;
    

    SDL_Rect rect = {
        label->widget.left,
        label->widget.top,
        label->widget.width,
        label->widget.height
    };

    SDL_Color color = widget->color_white;
    
    if (label->label_texture == NULL)
    {
        // render only once
        // cache these variables
        TTF_Font* font = widget->font;
        if (font == NULL)
            font = GGScreenSystemFont(label->screen);
        
        SDL_Surface* surface = TTF_RenderText_Blended(font, label->label, color);
        label->label_w = surface->w;
        label->label_h = surface->h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        label->label_texture = texture;
    }
    SDL_Rect textrect = {
        label->widget.left,
        label->widget.top + (rect.h - label->label_h) / 2,
        label->label_w,
        label->label_h
    };
    SDL_RenderCopy(renderer, label->label_texture, NULL, &textrect);
}

void GGLabelDestroy(GGLabel* label)
{
    if (label->label_texture)
        SDL_DestroyTexture(label->label_texture);
    free(label->label);
    free(label);
}
