#ifndef WAVEFORMWIDGET_H_INCLUDED
#define WAVEFORMWIDGET_H_INCLUDED

#include <stdint.h>

#include "gamegui/gamegui.h"

typedef struct {
    GGWidget widget;
    GGScreen* screen;
    
    int16_t* samples;
    
} GGWaveform;

GGWaveform* GGWaveformCreate(GGScreen* screen, int left, int top,  int width, int height);
void GGWaveformDestroy(GGWaveform* waveform);

void GGWaveformRender(GGWidget* widget, SDL_Renderer* renderer);

void GGWaveformSetData(GGWaveform* waveform, int16_t* data, uint32_t count);


#endif // WAVEFORMWIDGET_H_INCLUDED