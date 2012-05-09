#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "Widget.h"
#include "WidgetCheckBox.h"
#include "SharedResources.h"

using namespace std;

WidgetCheckBox::WidgetCheckBox (const string  & fname)
        : imgFileName(fname),
          cb(NULL),
          checked(false),
          pressed(false)
{
    SDL_Surface * tmp = IMG_Load(imgFileName.c_str());
    if (NULL == tmp) {
        fprintf(stderr, "Could not load image \"%s\" error \"%s\"\n",
                imgFileName.c_str(), IMG_GetError());
        SDL_Quit();
        exit(1);
    }
    cb = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);

    pos.w = cb->w;
    pos.h = cb->h / 2;
}

WidgetCheckBox::~WidgetCheckBox ()
{
    SDL_FreeSurface(cb);
}


    bool
WidgetCheckBox::checkClick ()
{
    // main button already in use, new click not allowed
    if (inp->lock[MAIN1]) return false;

    if (pressed && !inp->lock[MAIN1]) { // this is a button release
        pressed = false;
	if (isWithin(pos, inp->mouse)) { // the button release is done over the widget
            // toggle the state of the check button
            checked = !checked;
	    // activate upon release
	    return true;
	}
    }

    if (inp->pressing[MAIN1]) {
        if (isWithin(pos, inp->mouse)) {
            pressed = true;
            inp->lock[MAIN1] = true;
        }
    }
    return false;
}


    bool
WidgetCheckBox::isChecked () const
{
    return checked;
}


    void
WidgetCheckBox::render ()
{
    SDL_Rect    src;
    src.x = 0;
    src.y = checked ? pos.h : 0;
    src.h = pos.h;
    src.w = pos.w;

    SDL_BlitSurface(cb, &src, screen, &pos);
}

