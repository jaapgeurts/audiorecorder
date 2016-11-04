#ifndef VUMETER_H_INCLUDED
#define VUMETER_H_INCLUDED

#include "gamegui/gamegui.h"

typedef struct {
    GGWidget widget;
    GGScreen* screen;
    
    int min;
    int max;
    
    int threshold; // the red threshold above which distortion occurs
} GGVUMeter;


GGVUMeter* GGVUMeterCreate(GGScreen* screen, int left, int top,  int width, int height);
void GGVUMeterDestroy(GGVUMeter* vumeter);

void GGVUMeterSetRange(GGVUMeter* vumeter, int min, int max);
void GGVUMeterSetThreshold(GGVUMeter* vumeter, int threshold);

void GGVUMeterRender(GGWidget* widget, SDL_Renderer* renderer);


#endif // VUMETER_H_INCLUDED