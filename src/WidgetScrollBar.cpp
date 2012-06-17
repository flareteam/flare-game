/*
Copyright © 2011-2012 Clint Bellanger
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
 * class WidgetScrollBar
 */

#include "WidgetScrollBar.h"
#include "SharedResources.h"

using namespace std;

WidgetScrollBar::WidgetScrollBar(const std::string& _fileName)
	: fileName(_fileName) {

	scrollbars = NULL;
	click = NULL;
	
	loadArt();

	pos_up.w = pos_down.w  = pos_knob.w = scrollbars->w;
	pos_up.h = pos_down.h = pos_knob.h = (scrollbars->h / 5); //height of one button
}

void WidgetScrollBar::loadArt() {

	// load ScrollBar images
	scrollbars = IMG_Load(fileName.c_str());

	if(!scrollbars) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
		exit(1); // or abort ??
	}
	
	// optimize
	SDL_Surface *cleanup = scrollbars;
	scrollbars = SDL_DisplayFormatAlpha(scrollbars);
	SDL_FreeSurface(cleanup);
}

/**
 * Sets and releases the "pressed" visual state of the ScrollBar
 * If press and release, activate (return 1 for up, 2 for down)
 */
int WidgetScrollBar::checkClick() {

	// main ScrollBar already in use, new click not allowed
	//if (inpt->lock[MAIN1]) return 0;

	// main click released, so the ScrollBar state goes back to unpressed
	if (pressed_up && !inpt->lock[MAIN1]) {
		pressed_up = false;
		if (isWithin(pos_up, inpt->mouse)) {
			// activate upon release
			return 1;
		}
	}
	if (pressed_down && !inpt->lock[MAIN1]) {
		pressed_down = false;
		if (isWithin(pos_down, inpt->mouse)) {
			// activate upon release
			return 2;
		}
	}
	if (pressed_knob) {
		if (!inpt->lock[MAIN1]) {
			pressed_knob = false;
		}
		// set the value of the slider
		float tmp;
		if (inpt->mouse.y < pos_up.y+pos_up.h) {
			tmp = 0;
		} else if (inpt->mouse.y > pos_down.y-pos_knob.h) {
			tmp = bar_height;
		} else {
			tmp = inpt->mouse.y - pos_up.y;
		}

		pos_knob.y = pos_up.y + tmp;
		value = tmp*((float)maximum/(bar_height-pos_down.h));
		set();

		return 3;
	}

	pressed_up = false;
	pressed_down = false;
	pressed_knob = false;

	// detect new click
	if (inpt->pressing[MAIN1]) {
		if (isWithin(pos_up, inpt->mouse)) {
			inpt->lock[MAIN1] = true;
			pressed_up = true;
		} else if (isWithin(pos_down, inpt->mouse)) {
			inpt->lock[MAIN1] = true;
			pressed_down = true;
		} else if (isWithin(pos_knob, inpt->mouse)) {
			inpt->lock[MAIN1] = true;
			pressed_knob = true;
		}

	}
	return 0;

}

void WidgetScrollBar::set() {
	pos_knob.y = pos_up.y + pos_up.h + ((float)value/((float)maximum/(bar_height-pos_down.h)));
	if (pos_knob.y+pos_knob.h > pos_down.y) {
		pos_knob.y = pos_down.y-pos_knob.h;
	}
}

int WidgetScrollBar::getValue() {
	return value;
}

void WidgetScrollBar::render() {
	SDL_Rect src_up, src_down, src_knob;

	src_up.x = 0;
	src_up.w = pos_up.w;
	src_up.h = pos_up.h;

	src_down.x = 0;
	src_down.w = pos_down.w;
	src_down.h = pos_down.h;

	src_knob.x = 0;
	src_knob.y = pos_knob.h * 4;
	src_knob.w = pos_knob.w;
	src_knob.h = pos_knob.h;

	if (pressed_up)
		src_up.y = pos_up.h;
	else
		src_up.y = 0;

	if (pressed_down)
		src_down.y = pos_down.h*3;
	else
		src_down.y = pos_down.h*2;

	SDL_BlitSurface(scrollbars, &src_up, screen, &pos_up);
	SDL_BlitSurface(scrollbars, &src_down, screen, &pos_down);
	SDL_BlitSurface(scrollbars, &src_knob, screen, &pos_knob);
}

/**
 * Updates the scrollbar's location
 */
void WidgetScrollBar::refresh(int x, int y, int h, int val, int max) {
	maximum = max;
	value = val;
	pos_up.x = pos_down.x = pos_knob.x = x;
	pos_up.y = y;
	pos_down.y = y+h;
	bar_height = pos_down.y-pos_down.h-pos_down.h-pos_up.y+pos_up.h;
	set();
}

WidgetScrollBar::~WidgetScrollBar() {
	SDL_FreeSurface(scrollbars);
}

