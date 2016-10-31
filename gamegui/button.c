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

struct GGButton
{
    GGWidget widget;

    char* label;
};

GGButton* GGButtonCreate(GGScreen* screen, const char* label, int left, int top,  int width, int height)
{
    GGButton* btn = (GGButton*)malloc(sizeof(GGButton));
    GGWidgetInit(&btn->widget,left,top,width,height);
    btn->label = strdup(label);
    return btn;
}

void GGButtonDestroy(GGButton* button)
{
  free(button->label);
  free(button);
}