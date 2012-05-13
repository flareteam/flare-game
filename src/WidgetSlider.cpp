/*
Copyright © 2012 Justin Jacobs

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class WidgetSlider
 */

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


bool WidgetSlider::checkClick ()
{
	// main button already in use, new click not allowed
	if (inpt->lock[MAIN1]) return false;

	if (pressed && !inpt->lock[MAIN1]) { // this is a button release
		pressed = false;

		// set the value of the slider
		float tmp;
		if (inpt->mouse.x < pos.x)
			tmp = 0;
		else if (inpt->mouse.x > pos.x+pos.w)
			tmp = pos.w;
		else
			tmp = inpt->mouse.x - pos.x;

		pos_knob.x = pos.x + tmp - (pos_knob.w/2);
		value = tmp*((float)maximum/pos.w);

		// activate upon release
		return true;
	}

	if (inpt->pressing[MAIN1]) {
		if (isWithin(pos, inpt->mouse)) {
			pressed = true;
			inpt->lock[MAIN1] = true;
		}
	}
	return false;
}


void WidgetSlider::set (int min, int max, int val)
{
	minimum = min;
	maximum = max;
	value = val;

	pos_knob.x = pos.x + (val/((float)max/pos.w)) - pos_knob.w/2;
	pos_knob.y = pos.y;
}


int WidgetSlider::getValue () const
{
	return value;
}


void WidgetSlider::render ()
{
	SDL_Rect	base;
	base.x = 0;
	base.y = 0;
	base.h = pos.h;
	base.w = pos.w;

	SDL_Rect	knob;
	knob.x = 0;
	knob.y = pos.h;
	knob.h = pos_knob.h;
	knob.w = pos_knob.w;

	SDL_BlitSurface(sl, &base, screen, &pos);
	SDL_BlitSurface(sl, &knob, screen, &pos_knob);
}

