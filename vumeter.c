#include "vumeter.h"


GGVUMeter* GGVUMeterCreate(GGScreen* screen, int left, int top,  int width, int height)
{
    GGVUMeter* vumeter = (GGVUMeter*)calloc(1,sizeof(GGVUMeter));
    
    GGWidgetInit(&vumeter->widget, left, top, width, height);
    vumeter->widget.render_func       = GGVUMeterRender;
    vumeter->screen                   = screen;
    
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


void GGVUMeterRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;
    
    SDL_Color color = widget->has_focus ? widget->color_red : widget->color_gray;
    
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