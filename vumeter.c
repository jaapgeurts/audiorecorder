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

void GGVUMeterSetRange(GGVUMeter* vumeter, int min, int max)
{
    vumeter->min = min;
    vumeter->max = max;
}

void GGVUMeterSetThreshold(GGVUMeter* vumeter, int threshold)
{
    vumeter->threshold = threshold;
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
                printf("up\n");
                break;
            case NAVIGATE_DOWN:
                printf("down\n");
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
}