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
    GGWidgetRepaint((GGWidget*)vumeter);
}

void GGVUMeterSetCurrent(GGVUMeter* vumeter, float current)
{
    vumeter->current = current;
    GGWidgetRepaint((GGWidget*)vumeter);
}

void GGVUMeterSetVolume(GGVUMeter* vumeter, float volume)
{
    vumeter->volume = volume;
    GGWidgetRepaint((GGWidget*)vumeter);
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

    SDL_Color red      = {0xff, 0x00, 0x00, 0xff};
    SDL_Color vu_color = {0xff, 0x00, 0x00, 0xff};

    int       ih         = widget->height - 2; // internalheight
    float     color_step = 0x80 / (float)ih;

    // draw vu meter values

    int h = ih - ih * vumeter->current;

    for (int y = h; y < ih; y += 2)
    {
        vu_color.r =  (ih - y) * color_step * 2;
        vu_color.g = 0xff - (ih - y) * color_step;
        SDL_SetRenderDrawColor(renderer, vu_color.r, vu_color.g, vu_color.b, vu_color.a);
        SDL_RenderDrawLine(renderer,
            widget->left + 1,
            widget->top + y + 1,
            widget->left + widget->width - 1,
            widget->top + y + 1);
    }

    // draw volume slider
    h = ih - ih * vumeter->volume;
    int top = widget->top + h + 1;

    SDL_SetRenderDrawColor(renderer, red.r, red.g, red.b, red.a);
    SDL_RenderDrawLine(renderer, widget->left - 1, top, widget->left + widget->width + 1, top);
}