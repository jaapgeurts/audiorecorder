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

   This file is the "root" window of the display. The main window is created here.

   A screen has only one window. There can only be one screen during the application's run.

   Multiple screens/windows monitors are not supported.

   A screen keeps a list of widgets. When it's time to paint the screen will call render_func
   on the widget.
   When an event happens it will be forwarded to the active widget with focus by calling
   handle_event
   DPad events are caught by the screen and are used to move input focus between widgets

 */

#include "../containerlib/arraylist.h"

#include "gamegui.h"
#include "screen.h"
#include "keymapping.h"

struct GGScreen
{
    SDL_Window*   window;
    SDL_Renderer* renderer;

    JGArrayList* widgets; // all widgets on this screen
    GGWidget*    focus_widget; // the widget with active focus

    TTF_Font* system_font;

    void (* render_background_func)(SDL_Renderer* renderer);
};

static void GGCleanUp(GGScreen* screen)
{
    if (screen)
    {
        if (screen->widgets)
            JGArrayListDestroy(screen->widgets, free);

        if (screen->system_font)
            TTF_CloseFont(screen->system_font);

        if (screen->renderer)
            SDL_DestroyRenderer(screen->renderer);

        if (screen->window)
            SDL_DestroyWindow(screen->window);
        free(screen);
    }
}

static void GGInternalScreenClear(SDL_Renderer* renderer)
{
    // just draw black
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
}

enum Direction { DIR_UP = 1, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

static int midpoint_x(GGWidget* widget)
{
    return widget->left + widget->width / 2;
}

static int midpoint_y(GGWidget* widget)
{
    return widget->top + widget->height / 2;
}

/* finds the next widget dependent on the direction */
static GGWidget* GGScreenFindFocusWidget(GGScreen* screen, GGWidget* current, enum Direction dir)
{
    int       i;
    int       count = JGArrayListCount(screen->widgets);
    int       x, y;
    GGWidget* candidate = NULL;

    // get the midpoint of the widget
    x = midpoint_x(current);
    y = midpoint_y(current);

    for (i = 0; i < count; i++)
    {
        GGWidget* cursor = JGArrayListGet(screen->widgets, i);

        // exclude ourselves
        if (cursor == current)
            continue;
        
        if (!cursor->accepts_focus)
            continue;

        // only look at controls in the correct direction
        if ((dir == DIR_UP && midpoint_y(cursor) > y) ||
            (dir == DIR_DOWN && midpoint_y(cursor) < y) ||
            (dir == DIR_LEFT && midpoint_x(cursor) > x) ||
            (dir == DIR_RIGHT && midpoint_x(cursor) < x))
            continue;

        if (dir == DIR_UP || dir == DIR_DOWN)
        {
            // is the cursor widget in line with the midpoint of the current widget
            if (cursor->left < x && cursor->left + cursor->width > x)
            {
                // we found a candidate. Make sure it closest to the current
                if (candidate == NULL)
                {
                    candidate = cursor;
                }
                else
                {
                    if (abs(midpoint_y(cursor) - y) <  abs(midpoint_y(candidate) - y))
                        candidate = cursor;
                }
            }
        }
        else
        {
            // is the cursor widget in line with the midpoint of the current widget
            if (cursor->top < y && cursor->top + cursor->height > y)
            {
                // we found a candidate. Make sure it closest to the current
                if (candidate == NULL)
                {
                    candidate = cursor;
                }
                else
                {
                    if (abs(midpoint_x(cursor) - x) < abs(midpoint_x(candidate) - x))
                        candidate = cursor;
                }
            }
        }
    }

    return candidate;
}

/*
   Creates the screen for the app. Setups up the SDL window and renderer
 */
GGScreen* GGScreenCreate(const char* title, int width, int height, bool fullscreen)
{
    GGScreen* screen = (GGScreen*)calloc(1, sizeof(GGScreen));

    // create a new window
    screen->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width, height,
            fullscreen ? SDL_WINDOW_FULLSCREEN |
            SDL_WINDOW_OPENGL : SDL_WINDOW_OPENGL);

    if (!screen->window)
    {
        GGCleanUp(screen);
        GGSetLastError("%s", SDL_GetError());
        return NULL;
    }

    /* create the renderer */
    screen->renderer = SDL_CreateRenderer(screen->window, -1, SDL_RENDERER_ACCELERATED);

    if (!screen->renderer)
    {
        GGCleanUp(screen);
        GGSetLastError("%s", SDL_GetError());
        return NULL;
    }

    screen->widgets = JGArrayListCreate(64);

    if (screen->widgets == NULL)
    {
        GGCleanUp(screen);
        return NULL;
    }

    screen->system_font = TTF_OpenFont("assets/DroidSans.ttf", 11);

    if (!screen->system_font)
    {
        GGCleanUp(screen);
        GGSetLastError("%s", SDL_GetError());
        return NULL;
    }

    screen->render_background_func = GGInternalScreenClear;

    return screen;
}

void GGScreenDestroy(GGScreen* screen)
{
    GGCleanUp(screen);
}

int GGScreenGetHeight(GGScreen* screen)
{
    int h,t;
    SDL_GetWindowSize(screen->window,&t,&h);
    return h;
}

int GGScreenGetWidth(GGScreen* screen)
{
    int w,t;
    SDL_GetWindowSize(screen->window,&w,&t);
    return w;
    
}


void GGScreenRender(GGScreen* screen)
{
    // render all widgets
    int i;
    int count = JGArrayListCount(screen->widgets);

    // printf("Draw background\n");
    // clear the screen first
    screen->render_background_func(screen->renderer);

    for (i = 0; i < count; i++)
    {
        GGWidget* widget = JGArrayListGet(screen->widgets, i);

        SDL_Rect rect = {widget->left, widget->top, widget->width, widget->height};

        widget->render_func(widget, screen->renderer);
    }

    SDL_RenderPresent(screen->renderer);
}

void GGScreenClear(GGScreen* screen)
{
    GGInternalScreenClear(screen->renderer);
}

void GGScreenSetBackgroundRenderFunc(GGScreen* screen, void (* render_func)(SDL_Renderer*))
{
    screen->render_background_func =  render_func;
}

void GGScreenAddWidget(GGScreen* screen, GGWidget* widget)
{
    if (screen->focus_widget == NULL)
    {
        screen->focus_widget = widget;
        GGWidgetSetFocus(widget, true);
    }
    JGArrayListPush(screen->widgets, widget);
}

TTF_Font* GGScreenSystemFont(GGScreen* screen)
{
    return screen->system_font;
}

bool GGScreenHandleEvent(GGScreen* screen, SDL_Event* event)
{
    // render all widgets
    GGWidget* focus_widget = screen->focus_widget;

    /* if the DPad was pushed, find the closed widget and change focus */
    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.sym)
        {
            case NAVIGATE_UP:
                focus_widget = GGScreenFindFocusWidget(screen, focus_widget, DIR_UP);
                break;

            case NAVIGATE_DOWN:
                focus_widget = GGScreenFindFocusWidget(screen, focus_widget, DIR_DOWN);
                break;

            case NAVIGATE_LEFT:
                focus_widget = GGScreenFindFocusWidget(screen, focus_widget, DIR_LEFT);
                break;

            case NAVIGATE_RIGHT:
                focus_widget = GGScreenFindFocusWidget(screen, focus_widget, DIR_RIGHT);
                break;

            default:
                break;
        }
    }

    if (focus_widget == NULL)
        return false;

    if (focus_widget != screen->focus_widget)
    {
        GGWidgetSetFocus(screen->focus_widget, false);
        screen->focus_widget = focus_widget;
        GGWidgetSetFocus(focus_widget, true);
    }

    if (focus_widget->handle_event_func == NULL)
        return false;
    
    return focus_widget->handle_event_func(focus_widget, event);
    
}