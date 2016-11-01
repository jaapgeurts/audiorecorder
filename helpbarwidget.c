#include "helpbarwidget.h"

GGHelpBar* GGHelpBarCreate(GGScreen* screen)
{
    GGHelpBar* helpbar = (GGHelpBar*)calloc(1, sizeof(GGHelpBar));
    
    int w = GGScreenGetWidth(screen);
    int h = GGScreenGetHeight(screen);
    int bar_height = 30;

    GGWidgetInit(&helpbar->widget, 0, h - bar_height, w, bar_height);
    helpbar->widget.render_func   = GGHelpBarRender;
    helpbar->screen               = screen;
    helpbar->widget.accepts_focus = false;

    GGScreenAddWidget(screen, &helpbar->widget);

    return helpbar;
}

void GGHelpBarDestroy(GGHelpBar* helpbar)
{
    free(helpbar);
}

void GGHelpBarSetHelp(GGHelpBar* helpbar, GGHelpBarKey key, const char* text)
{
    helpbar->help_text[key] = strdup(text);
}

void GGHelpBarRender(GGWidget* widget, SDL_Renderer* renderer)
{
}