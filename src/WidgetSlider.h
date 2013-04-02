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


#pragma once
#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

class Widget;

class WidgetSlider : public Widget {
public:
	WidgetSlider (const std::string & fname);
	~WidgetSlider ();

	bool checkClick ();
	bool checkClick (int x, int y);
	void set (int min, int max, int val);
	int getValue () const;
	void render (SDL_Surface *target = NULL);

	SDL_Rect pos_knob; // This is the position of the slider's knob within the screen

private:
	SDL_Surface * sl;
	bool pressed;
	int minimum;
	int maximum;
	int value;
};

#endif

