#ifndef KEYMAPPING_INCLUDED
#define KEYMAPPING_INCLUDED

#include <SDL/SDL.h>

#ifdef GCW0
#define GCW0K_LEFT           SDLK_LEFT
#define GCW0K_RIGHT          SDLK_RIGHT
#define GCW0K_UP             SDLK_UP
#define GCW0K_DOWN           SDLK_DOWN
#define GCW0K_A              SDLK_LCTRL
#define GCW0K_B              SDLK_LALT
#define GCW0K_X              SDLK_LSHIFT
#define GCW0K_Y              SDLK_SPACE
#define GCW0K_SHOULDER_LEFT  SDLK_TAB
#define GCW0K_SHOULDER_RIGHT SDLK_BACKSPACE
#define GCW0K_SELECT         SDLK_ESCAPE
#define GCW0K_START          SDLK_RETURN
#define GCW0K_PAUSE          SDLK_PAUSE
#define GCW0K_HOME           SDLK_HOME

#define NAVIGATE_LEFT    GCW0K_LEFT
#define NAVIGATE_RIGHT   GCW0K_RIGHT
#define NAVIGATE_UP      GCW0K_UP
#define NAVIGATE_DOWN    GCW0K_DOWN
#define ACTION_PRIMARY   GCW0K_A
#define ACTION_SECONDARY GCW0K_B
#define ACTION_SELECT    GCW0K_SELECT
#define ACTION_BACK      GCW0K_X

#else

#define NAVIGATE_LEFT    SDLK_LEFT
#define NAVIGATE_RIGHT   SDLK_RIGHT
#define NAVIGATE_UP      SDLK_UP
#define NAVIGATE_DOWN    SDLK_DOWN
#define ACTION_PRIMARY   SDLK_SPACE
#define ACTION_SECONDARY SDLK_RETURN
#define ACTION_SELECT    SDLK_e
#define ACTION_BACK      SDLK_ESCAPE

#endif

#endif // KEYMAPPING_INCLUDED