#ifndef HELPBARWIDGET_H_INCLUDED
#define HELPBARWIDGET_H_INCLUDED

#include <stdint.h>

#include "gamegui/gamegui.h"

#define TOTAL_BUTTONS 9

typedef enum
{
    GCW_BTN_A,
    GCW_BTN_B,
    GCW_BTN_X,
    GCW_BTN_Y,
    GCW_BTN_SELECT,
    GCW_BTN_START,
    GCW_BTN_DPAD,
    GCW_BTN_SHOULDER_LEFT,
    GCW_BTN_SHOULDER_RIGHT
} GGHelpBarButton;


typedef struct
{
    GGWidget  widget;
    GGScreen* screen;

    char* help_text[TOTAL_BUTTONS];
    SDL_Surface* icons[TOTAL_BUTTONS];
    SDL_Texture* textures[TOTAL_BUTTONS];
    
    SDL_Rect help_rect[TOTAL_BUTTONS];
    
} GGHelpBar;



/* Attached itself to the bottom of the screen */
GGHelpBar* GGHelpBarCreate(GGScreen* screen);
void GGHelpBarDestroy(GGHelpBar* helpbar);

/* setting text to NULL will disable help for this key */
void GGHelpBarSetHelp(GGHelpBar* helpbar, GGHelpBarButton key, const char* text);

void GGHelpBarRender(GGWidget* widget, SDL_Renderer* renderer);

#endif // HELPBARWIDGET_H_INCLUDED