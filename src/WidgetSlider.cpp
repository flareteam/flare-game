#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "Widget.h"
#include "WidgetSlider.h"
#include "SharedResources.h"

using namespace std;

WidgetSlider::WidgetSlider (const string  & fname)
        : imgFileName(fname),
          sl(NULL),
          pressed(false)
{
    SDL_Surface * tmp = IMG_Load(imgFileName.c_str());
    if (NULL == tmp) {
        fprintf(stderr, "Could not load image \"%s\" error \"%s\"\n",
                imgFileName.c_str(), IMG_GetError());
        SDL_Quit();
        exit(1);
    }
    sl = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);

    pos.w = sl->w;
    pos.h = sl->h / 2;

    pos_knob.w = sl->w / 8;
    pos_knob.h = sl->h / 2;
}

WidgetSlider::~WidgetSlider ()
{
    SDL_FreeSurface(sl);
}


    bool
WidgetSlider::checkClick ()
{
    // main button already in use, new click not allowed
    if (inp->lock[MAIN1]) return false;

    if (pressed && !inp->lock[MAIN1]) { // this is a button release
        pressed = false;

        // set the value of the slider
        float tmp;
        if (inp->mouse.x < pos.x)
            tmp = 0;
        else if (inp->mouse.x > pos.x+pos.w)
            tmp = pos.w;
        else
            tmp = inp->mouse.x - pos.x;

        pos_knob.x = pos.x + tmp - (pos_knob.w/2);
        value = tmp*((float)maximum/pos.w);

	    // activate upon release
	    return true;
    }

    if (inp->pressing[MAIN1]) {
        if (isWithin(pos, inp->mouse)) {
            pressed = true;
            inp->lock[MAIN1] = true;
        }
    }
    return false;
}


    void
WidgetSlider::set (int min, int max, int val)
{
    minimum = min;
    maximum = max;
    value = val;

    pos_knob.x = pos.x + (val/((float)max/pos.w)) - pos_knob.w/2;
    pos_knob.y = pos.y;
}


    int
WidgetSlider::getValue () const
{
    return value;
}


    void
WidgetSlider::render ()
{
    SDL_Rect    base;
    base.x = 0;
    base.y = 0;
    base.h = pos.h;
    base.w = pos.w;

    SDL_Rect    knob;
    knob.x = 0;
    knob.y = pos.h;
    knob.h = pos_knob.h;
    knob.w = pos_knob.w;

    SDL_BlitSurface(sl, &base, screen, &pos);
    SDL_BlitSurface(sl, &knob, screen, &pos_knob);
}

