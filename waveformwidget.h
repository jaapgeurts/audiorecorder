#ifndef WAVEFORMWIDGET_H_INCLUDED
#define WAVEFORMWIDGET_H_INCLUDED

#include <stdint.h>

#include "gamegui/gamegui.h"

typedef struct {
    GGWidget widget;
    GGScreen* screen;
    
    int16_t* samples;
    
} GGWaveformWidget;

GGWaveformWidget* GGWaveformWidgetCreate(GGScreen* screen, int left, int top,  int width, int height);
void GGWaveformWidgetDestroy(GGWaveformWidget* waveformwidget);

void GGWaveformWidgetRender(GGWidget* widget, SDL_Renderer* renderer);

void GGWaveformWidgetSetData(GGWidget* widget, int16_t* data, uint32_t count);


#endif // WAVEFORMWIDGET_H_INCLUDED