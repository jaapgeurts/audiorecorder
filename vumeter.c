#include "vumeter.h"

static bool GGVUMeterHandleEvent(GGWidget* widget, SDL_Event* event);

GGVUMeter* GGVUMeterCreate(GGScreen* screen, int left, int top,  int width, int height)
{
    GGVUMeter* vumeter = (GGVUMeter*)calloc(1, sizeof(GGVUMeter));

    GGWidgetInit(&vumeter->widget, left, top, width, height);
    vumeter->widget.render_func = GGVUMeterRender;
    vumeter->screen             = screen;

    vumeter->widget.handle_event_func = GGVUMeterHandleEvent;

    // add it to the screen
    GGScreenAddWidget(screen, &vumeter->widget);

    return vumeter;
}

void GGVUMeterDestroy(GGVUMeter* vumeter)
{
    free(vumeter);
}

void GGVUMeterSetThreshold(GGVUMeter* vumeter, float threshold)
{
    vumeter->threshold = threshold;
}

void GGVUMeterSetCurrent(GGVUMeter* vumeter, float current)
{
    vumeter->current = current;
}

static bool GGVUMeterHandleEvent(GGWidget* widget, SDL_Event* event)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;
    bool       handled = false;

    if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
            case ACTION_SELECT:
                GGScreenGrabDPad(vumeter->screen, widget);
                break;

            case ACTION_BACK:
                GGScreenReleaseDPad(vumeter->screen, widget);
                break;

            case NAVIGATE_UP:

                if (vumeter->volume_up_func != NULL)
                    vumeter->volume_up_func(widget);
                break;

            case NAVIGATE_DOWN:

                if (vumeter->volume_down_func != NULL)
                    vumeter->volume_down_func(widget);
                break;
        }
    }

    return handled;
}

void GGVUMeterRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;

    SDL_Color  color = widget->has_focus ? widget->color_red : widget->color_gray;

    // TODO: grab dpad keys

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {
        vumeter->widget.left,
        vumeter->widget.top,
        vumeter->widget.width,
        vumeter->widget.height
    };
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Color red = {0xff, 0x00, 0x00, 0xff};

    int ih = widget->height -2; // internalheight
    
    int       h   = ih - ih * vumeter->current;
    int       top = widget->top + h + 1;

    SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
    SDL_RenderDrawLine(renderer, widget->left - 1, top, widget->left + widget->width + 1, top);
}