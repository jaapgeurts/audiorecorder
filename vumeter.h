#ifndef VUMETER_H_INCLUDED
#define VUMETER_H_INCLUDED

#include "gamegui/gamegui.h"

typedef struct {
    GGWidget widget;
    GGScreen* screen;
    
    float current;
    float volume;
    
    float threshold; // the red threshold above which distortion occurs
    
    GGWidgetChangeFunc volume_up_func;
    GGWidgetChangeFunc volume_down_func;
    
} GGVUMeter;


GGVUMeter* GGVUMeterCreate(GGScreen* screen, int left, int top,  int width, int height);
void GGVUMeterDestroy(GGVUMeter* vumeter);

void GGVUMeterSetThreshold(GGVUMeter* vumeter, float threshold);
void GGVUMeterSetCurrent(GGVUMeter* vumeter, float current);
void GGVUMeterSetVolume(GGVUMeter* vumeter, float current);

void GGVUMeterRender(GGWidget* widget, SDL_Renderer* renderer);


#endif // VUMETER_H_INCLUDED