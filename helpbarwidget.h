#ifndef HELPBARWIDGET_H_INCLUDED
#define HELPBARWIDGET_H_INCLUDED

#include <stdint.h>

#include "gamegui/gamegui.h"

#define TOTAL_KEYS 9

typedef enum
{
    GCW_KEY_A,
    GCW_KEY_B,
    GCW_KEY_X,
    GCW_KEY_Y,
    GCW_KEY_SELECT,
    GCW_KEY_START,
    GCW_KEY_DPAD,
    GCW_KEY_SHOULDER_LEFT,
    GCW_KEY_SHOULDER_RIGHT
} GGHelpBarKey;


typedef struct
{
    GGWidget  widget;
    GGScreen* screen;

    char* help_text[TOTAL_KEYS];
    SDL_Surface* icons[TOTAL_KEYS];
    SDL_Texture* key_texture[TOTAL_KEYS];
    
} GGHelpBar;



/* Attached itself to the bottom of the screen */
GGHelpBar* GGHelpBarCreate(GGScreen* screen);
void GGHelpBarDestroy(GGHelpBar* helpbar);

/* setting text to NULL will disable help for this key */
void GGHelpBarSetHelp(GGHelpBar* helpbar, GGHelpBarKey key, const char* text);

void GGHelpBarRender(GGWidget* widget, SDL_Renderer* renderer);

#endif // HELPBARWIDGET_H_INCLUDED