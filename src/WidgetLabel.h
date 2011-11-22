/*
Copyright 2011 Clint Bellanger

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
 * class WidgetLabel
 *
 * A simple text display for menus.
 * This is preferred to directly displaying text because it helps handle positioning
 */

#ifndef WIDGET_LABEL_H
#define WIDGET_LABEL_H

#include <SDL.h>
#include "FontEngine.h"
#include <string>

const int VALIGN_CENTER = 0;
const int VALIGN_TOP = 1;
const int VALIGN_BOTTOM = 2;

class WidgetLabel {
private:

public:
	WidgetLabel();
	~WidgetLabel();
	void render();
	void set(int _x, int _y, int _justify, int _valign, string _text, int _color);
	
	string text;
	int color;
	int x;
	int y;
	int justify;
};

#endif
