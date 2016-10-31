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
    widget->left   = left;
    widget->top    = top;
    widget->width  = width;
    widget->height = height;
    widget->has_focus = false;
    widget->is_dirty = false;
}