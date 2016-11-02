#include "helpbarwidget.h"

char* icons_names[TOTAL_BUTTONS] = {
    "assets/btn_a.png",  // A
    "assets/btn_b.png",  // B
    "assets/btn_x.png",  // X
    "assets/btn_y.png",  // Y
    "assets/btn_select.png",  // SELET
    "assets/btn_start.png",  // START
    "assets/btn_dpad.png",  // DPAD
    "assets/btn_left.png",  // SHOULDER_LEFT
    "assets/btn_right.png",  // SHOULDER_RIGHT
};

GGHelpBar* GGHelpBarCreate(GGScreen* screen)
{
    GGHelpBar* helpbar = (GGHelpBar*)calloc(1, sizeof(GGHelpBar));

    int        w          = GGScreenGetWidth(screen);
    int        h          = GGScreenGetHeight(screen);
    int        bar_height = 23;

    GGWidgetInit(&helpbar->widget, 0, h - bar_height, w, bar_height);
    helpbar->widget.render_func   = GGHelpBarRender;
    helpbar->screen               = screen;
    helpbar->widget.accepts_focus = false;

    for (int i = 0; i < TOTAL_BUTTONS; i++)
    {
        helpbar->icons[i] = IMG_Load(icons_names[i]);

        if (helpbar->icons[i] == NULL)
            fprintf(stderr, "Error loading icons %s (%s)\n", icons_names[i], SDL_GetError());
    }

    GGScreenAddWidget(screen, &helpbar->widget);

    return helpbar;
}

void GGHelpBarDestroy(GGHelpBar* helpbar)
{
    for (int i = 0; i < TOTAL_BUTTONS; i++)
    {
        SDL_DestroyTexture(helpbar->textures[i]);
        SDL_FreeSurface(helpbar->icons[i]);
        free(helpbar->help_text[i]);
    }
    free(helpbar);
}

void GGHelpBarSetHelp(GGHelpBar* helpbar, GGHelpBarButton key, const char* text)
{
    helpbar->help_text[key] = strdup(text);
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

void GGHelpBarRender(GGWidget* widget, SDL_Renderer* renderer)
{
    GGHelpBar* helpbar = (GGHelpBar*)widget;
    SDL_Color  white   = {0xa0, 0xa0, 0xa0, 0xa0};

    SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
    SDL_RenderDrawLine(renderer, widget->left, widget->top, widget->left + widget->width, widget->top);

    int x            = 1;
    int y            = 1;
    int help_spacing = 4;

    for (int i = 0; i < TOTAL_BUTTONS; i++)
    {
        if (helpbar->help_text[i] == NULL)
            continue;

        if (helpbar->textures[i] == NULL && helpbar->icons[i] != NULL)
        {
            int          icon_spacing = 2;
            int          padding      = 0;
            
        
            // render the textures
            SDL_Surface* text_surface =
                TTF_RenderText_Blended(GGScreenSystemFont(helpbar->screen), helpbar->help_text[i], white);
            SDL_Surface* icon_surface  = helpbar->icons[i];
            int          w             = icon_surface->w + icon_spacing + text_surface->w + padding * 2;
            int          h             = max(icon_surface->h, text_surface->h) + padding;
            SDL_Surface* final_surface = SDL_CreateRGBSurface(0, w, h, GGScreenGetDepth(helpbar->screen), 0, 0, 0, 0);
            helpbar->help_rect[i].w = final_surface->w;
            helpbar->help_rect[i].h = final_surface->h;
            SDL_FillRect(final_surface,NULL,0x2f2f2f);

            SDL_Rect dstrect = {padding, padding, icon_surface->w, icon_surface->h};
            SDL_BlitSurface(icon_surface, NULL, final_surface, &dstrect);

            dstrect.x = dstrect.x + dstrect.w + icon_spacing;
            dstrect.y = dstrect.y + (final_surface->h - text_surface->h) / 2 + padding;
            dstrect.w = dstrect.x + text_surface->w;
            dstrect.h = dstrect.y + text_surface->h;

            SDL_BlitSurface(text_surface, NULL, final_surface, &dstrect);

            helpbar->textures[i] = SDL_CreateTextureFromSurface(renderer, final_surface);
            SDL_FreeSurface(text_surface);
            SDL_FreeSurface(final_surface);
            SDL_FreeSurface(helpbar->icons[i]);
            helpbar->icons[i] = NULL;
        }
        SDL_Rect dstrect = {
            helpbar->widget.left + x + 1,  helpbar->widget.top + y, helpbar->help_rect[i].w, helpbar->help_rect[i].h
        };
        printf("Rendering help: %s at, %d,%d,%d,%d\n", helpbar->help_text[i], dstrect.x, dstrect.y, dstrect.w,
            dstrect.h);
        SDL_RenderCopy(renderer, helpbar->textures[i], NULL, &dstrect);
        x +=  dstrect.w + help_spacing;
    }
}