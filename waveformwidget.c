#include <time.h>
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
    srand(time(NULL));

    wfw->samples = (int16_t*)calloc(1, count * sizeof(int16_t));

    /*    for (int i = 0; i < count; i++)
        {
            int16_t rndnum = (float)rand() / (float)RAND_MAX * 65535.0 - 32768.0;

            wfw->samples[i] = rndnum;
        }
     */
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

    SDL_Color   c    = { 0x00, 0xff, 0x00, 0xff };
    SDL_Color   gray = { 0xa0, 0xa0, 0xa0, 0xa0 };

    SDL_SetRenderDrawColor(renderer, gray.r, gray.g, gray.b, gray.a);
    SDL_Rect rect = {
        wfw->widget.left,
        wfw->widget.top,
        wfw->widget.width,
        wfw->widget.height
    };
    SDL_RenderDrawRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

    float vs    = (widget->height - 2) / 65535.0;
    int   lasty = widget->top + (widget->height - 2) / 2;

    for (int x = 0; x < widget->width - 2; x++)
    {
        int y = widget->top + widget->height / 2 + (int)(wfw->samples[x] * vs);
        SDL_RenderDrawLine(renderer, widget->left + x + 1, lasty + 1, widget->left + x + 1, y + 1);
        lasty = y;
    }
}

void GGWaveformSetData(GGWidget* widget, int16_t* data, uint32_t count)
{
    GGWaveform* wfw        = (GGWaveform*)widget;
    int         numsamples = widget->width - 2;       // -2 for a 1 pixel line at the borders
    int         step;

    if (count < numsamples)
        step = 1;
    else
        step = count / widget->width;

    int i = 0;

    for (; i < widget->width - 2; i++)
    {
        wfw->samples[i] = data[i * step];
    }

    if (step == 1)
    {
        // pad with zeros
        for (int j = i; j < numsamples; j++)
            wfw->samples[j] = 0;
    }
}