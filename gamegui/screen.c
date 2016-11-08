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

    bool forward_dpad;

    void (* render_background_func)(SDL_Renderer* renderer);

    GGWidgetChangeFunc grab_dpad_func;
    GGWidgetChangeFunc release_dpad_func;

    GGScreenHookFunc pre_event_func;
    GGScreenHookFunc post_event_func;
    GGScreenHookFunc pre_render_func;
    GGScreenHookFunc post_render_func;
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

static int perpline_x(GGWidget* widget, int offset)
{
    return widget->left + offset;
}

static int perpline_y(GGWidget* widget, int offset)
{
    return widget->top + offset;
}

/* finds the next widget dependent on the direction */
static GGWidget* GGScreenFindFocusWidget(GGScreen* screen, GGWidget* current, enum Direction dir)
{
    int       i;
    int       count = JGArrayListCount(screen->widgets);
    int       x, y;
    GGWidget* candidate = NULL;

    int       posx[3] = { 1, current->width / 2, current->width -1};
    int       posy[3] = { 1, current->height / 2, current->height-1 };

    for (int pos = 0; pos < 3; pos++)
    {
        // get the midpoint of the widget
        x = perpline_x(current, posx[pos]);
        y = perpline_y(current, posy[pos]);

        for (i = 0; i < count; i++)
        {
            GGWidget* cursor = JGArrayListGet(screen->widgets, i);

            // exclude ourselves
            if (cursor == current)
                continue;

            if (!cursor->accepts_focus || !cursor->is_enabled || !cursor->is_visible)
                continue;

            // only look at controls in the correct direction
            if ((dir == DIR_UP && perpline_y(cursor, posy[pos]) > y) ||
                (dir == DIR_DOWN && perpline_y(cursor, posy[pos]) < y) ||
                (dir == DIR_LEFT && perpline_x(cursor, posx[pos]) > x) ||
                (dir == DIR_RIGHT && perpline_x(cursor, posx[pos]) < x))
                continue;

            if (dir == DIR_UP || dir == DIR_DOWN)
            {
                // is the cursor widget in line with the perpendicular line of the current widget at pos
                if (cursor->left < x && cursor->left + cursor->width > x)
                {
                    // we found a candidate. Make sure it closest to the current
                    if (candidate == NULL)
                    {
                        candidate = cursor;
                    }
                    else
                    {
                        if (abs(perpline_y(cursor, posy[pos]) - y) <  abs(perpline_y(candidate, posy[pos]) - y))
                            candidate = cursor;
                    }
                }
            }
            else
            {
                // is the cursor widget in line with the perpendicular line of the current widget at pos
                if (cursor->top < y && cursor->top + cursor->height > y)
                {
                    // we found a candidate. Make sure it closest to the current
                    if (candidate == NULL)
                    {
                        candidate = cursor;
                    }
                    else
                    {
                        if (abs(perpline_x(cursor, posx[pos]) - x) < abs(perpline_x(candidate, posx[pos]) - x))
                            candidate = cursor;
                    }
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

    screen->forward_dpad = false;

    return screen;
}

void GGScreenDestroy(GGScreen* screen)
{
    GGCleanUp(screen);
}

int GGScreenGetHeight(GGScreen* screen)
{
    int h, t;

    SDL_GetWindowSize(screen->window, &t, &h);
    return h;
}

int GGScreenGetWidth(GGScreen* screen)
{
    int w, t;

    SDL_GetWindowSize(screen->window, &w, &t);
    return w;
}

int GGScreenGetDepth(GGScreen* screen)
{
    SDL_DisplayMode mode;

    SDL_GetWindowDisplayMode(screen->window, &mode);
    return SDL_BITSPERPIXEL(mode.format);
}

void GGScreenRender(GGScreen* screen)
{
    // render all widgets
    int i;
    int count = JGArrayListCount(screen->widgets);

    // printf("Draw background\n");
    // clear the screen first
    bool screen_dirty = false;

    // if at least one control is dirty then render all
    for (i = 0; i < count; i++)
    {
        GGWidget* widget = JGArrayListGet(screen->widgets, i);

        if (widget->is_dirty)
        {
            screen_dirty = true;
            break;
        }
    }

    if (screen_dirty)
    {
        printf("Render cycle\n");
        screen->render_background_func(screen->renderer);

        for (i = 0; i < count; i++)
        {
            GGWidget* widget = JGArrayListGet(screen->widgets, i);

            widget->render_func(widget, screen->renderer);
            widget->is_dirty = false;
        }

        SDL_RenderPresent(screen->renderer);
    }
}

void GGScreenClear(GGScreen* screen)
{
    GGInternalScreenClear(screen->renderer);
}

void GGScreenSetBackgroundRenderFunc(GGScreen* screen, void (* render_func)(SDL_Renderer*))
{
    screen->render_background_func =  render_func;
}

void GGScreenSetPreEventFunc(GGScreen* screen, GGScreenHookFunc pre_event_func)
{
    screen->pre_event_func = pre_event_func;
}

void GGScreenSetPostEventFunc(GGScreen* screen, GGScreenHookFunc post_event_func)
{
    screen->post_event_func = post_event_func;
}

void GGScreenSetPreRenderFunc(GGScreen* screen, GGScreenHookFunc pre_render_func)
{
    screen->pre_render_func = pre_render_func;
}

void GGScreenSetPostRenderFunc(GGScreen* screen, GGScreenHookFunc post_render_func)
{
    screen->post_render_func = post_render_func;
}

void GGScreenHandlePreEventFunc(GGScreen* screen)
{
    if (screen->pre_event_func != NULL)
        screen->pre_event_func(screen);
}

void GGScreenHandlePostEventFunc(GGScreen* screen)
{
    if (screen->post_event_func != NULL)
        screen->post_event_func(screen);
}

void GGScreenHandlePreRenderFunc(GGScreen* screen)
{
    if (screen->pre_render_func != NULL)
        screen->pre_render_func(screen);
}

void GGScreenHandlePostRenderFunc(GGScreen* screen)
{
    if (screen->post_render_func != NULL)
        screen->post_render_func(screen);
}

void GGScreenAddWidget(GGScreen* screen, GGWidget* widget)
{
    if (screen->focus_widget == NULL && widget->accepts_focus)
    {
        screen->focus_widget            = widget;
        screen->focus_widget->has_focus = true;
        GGWidgetRepaint(screen->focus_widget);

        if (widget->focus_gained_func != NULL)
            widget->focus_gained_func(widget);
    }
    JGArrayListPush(screen->widgets, widget);
}

void GGScreenSetFocusWidget(GGScreen* screen, GGWidget* widget)
{
    if (screen->focus_widget == widget)
        return;

    if (widget->accepts_focus)
    {
        screen->focus_widget->has_focus = false;
        GGWidgetRepaint(screen->focus_widget);

        if (screen->focus_widget->focus_lost_func != NULL)
            screen->focus_widget->focus_lost_func(widget);

        screen->focus_widget            = widget;
        screen->focus_widget->has_focus = true;
        GGWidgetRepaint(screen->focus_widget);

        if (screen->focus_widget->focus_gained_func != NULL)
            screen->focus_widget->focus_gained_func(widget);
    }
    else
    {
        fprintf(stderr, "Attempt to set focus to widget that doesn't accept it\n");
    }
}

void GGScreenGrabDPad(GGScreen* screen, GGWidget* widget)
{
    screen->forward_dpad = true;

    if (screen->grab_dpad_func != NULL)
        screen->grab_dpad_func(widget);
}

void GGScreenReleaseDPad(GGScreen* screen, GGWidget* widget)
{
    screen->forward_dpad = false;

    if (screen->release_dpad_func != NULL)
        screen->release_dpad_func(widget);
}

void GGScreenSetGrabDPadCallBack(GGScreen* screen, GGWidgetChangeFunc callback)
{
    screen->grab_dpad_func = callback;
}

void GGScreenSetReleaseDPadCallBack(GGScreen* screen, GGWidgetChangeFunc callback)
{
    screen->release_dpad_func = callback;
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
    if (!screen->forward_dpad)
    {
        if (event->type == SDL_KEYUP && (
                event->key.keysym.sym == NAVIGATE_UP ||
                event->key.keysym.sym == NAVIGATE_DOWN ||
                event->key.keysym.sym == NAVIGATE_LEFT ||
                event->key.keysym.sym == NAVIGATE_RIGHT))
        {
            //swallow these keys
            return true;
        }
        else if (event->type == SDL_KEYDOWN)
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
    }

    if (focus_widget == NULL)
        return false;

    if (focus_widget != screen->focus_widget)
    {
        // another widget was selected. change focus and return
        // ie. don't send dpad to newly focussed controller
        GGScreenSetFocusWidget(screen, focus_widget);
        return true;
    }

    if (focus_widget->handle_event_func == NULL)
        return false;

    return focus_widget->handle_event_func(focus_widget, event);
}