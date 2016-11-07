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

 */

#include "widget.h"

void GGWidgetInit(GGWidget* widget, int left, int top,  int width, int height)
{
    widget->left          = left;
    widget->top           = top;
    widget->width         = width;
    widget->height        = height;
    widget->has_focus     = false;
    widget->accepts_focus = true;
    widget->render_func   = NULL;
    widget->is_dirty      = true;
    widget->is_disabled   = false;

    widget->color_white = (SDL_Color){
        0xff, 0xff, 0xff, 0xff
    };
    widget->color_gray = (SDL_Color){
        0xa0, 0xa0, 0xa0, 0xff
    };
    widget->color_dark_gray = (SDL_Color){
        0x40, 0x40, 0x40, 0xff
    };
    widget->color_red = (SDL_Color){
        0xff, 0x00, 0x00, 0xff
    };
}

/* Setting font to NULL reverts back to the system font */
void GGWidgetSetFont(GGWidget* widget, TTF_Font* font)
{
    widget->font = font;
}

void GGWidgetSetDisabled(GGWidget* widget,bool state)
{
    widget->is_disabled = state;
}

void GGWidgetRepaint(GGWidget* widget)
{
    widget->is_dirty = true;
}