#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

typedef struct GGButton GGButton;

GGButton* GGButtonCreate(GGScreen *screen, const char* label, int top, int left, int width, int height);
void GGButtonDestroy(GGButton* button);

#endif // BUTTON_H_INCLUDED
