#include <stdlib.h>

#include "waveformwidget.h"

GGWaveform* GGWaveformCreate(GGScreen* screen, int left, int top,  int width, int height)
{
    GGWaveform* wfw = (GGWaveform*)calloc(1, sizeof(GGWaveform));

    GGWidgetInit(&wfw->widget, left, top, width, height);
    wfw->widget.render_func   = GGWaveformRender;
    wfw->screen               = screen;
    wfw->widget.accepts_focus = false;

    // 10 seconds @44.1Khz, 1 channel

    int count = wfw->widget.width - 2;

    wfw->samples = (int16_t*)calloc(1, count * sizeof(int16_t));

    // add it to the screen

    GGScreenAddWidget(screen, &wfw->widget);

    return wfw;
}

void GGWaveformDestroy(GGWaveform* waveform)
{
    free(waveform->samples);
    free(waveform);
}

void GGWaveformRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGWaveform* wfw = (GGWaveform*)widget;

    
    SDL_Color   green    = { 0x00, 0xff, 0x00, 0xff };


    SDL_SetRenderDrawColor(renderer, widget->color_dark_gray.r, widget->color_dark_gray.g, widget->color_dark_gray.b, widget->color_dark_gray.a);
    SDL_Rect rect = {
        wfw->widget.left,
        wfw->widget.top,
        wfw->widget.width,
        wfw->widget.height
    };
    SDL_RenderDrawRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a);

    float vs    = (widget->height - 2) / 65535.0;
    int   lasty = widget->top + (widget->height - 2) / 2;
    int   lastx = 0;

    for (int x = 0; x < widget->width - 2; x++)
    {
        int y = widget->top + (widget->height-2) / 2 + (int)(wfw->samples[x] * vs);
        SDL_RenderDrawLine(renderer, widget->left+lastx+1, lasty + 1, widget->left + x + 1, y + 1);
        lasty = y;
        lastx = x;
    }
}

void GGWaveformSetData(GGWaveform* wfw, int16_t* data, uint32_t count)
{
    int         maxdstsamples = wfw->widget.width - 2;       // -2 for a 1 pixel line at the borders
    // only ever display 10s worth of samples.
    int         maxsrcsamples = 10 * 44100; // 10s * 44100KHz
    int         step;

    int         takesrcsamples = maxsrcsamples;
    int         putdstsamples  = maxdstsamples;

    if (count < takesrcsamples)
        takesrcsamples = count;

    if (takesrcsamples < putdstsamples)
    {
        putdstsamples = takesrcsamples;
    }

    step = takesrcsamples / putdstsamples;

    int i = 0;

    while (i < putdstsamples)
    {
        wfw->samples[i] = data[i * step];
        i++;
    }

    int j = i;

    while (j < maxdstsamples)
    {
        // pad with zeros
        wfw->samples[j] = 0;
        j++;
    }
    
    GGWidgetRepaint((GGWidget*)wfw);
    
}