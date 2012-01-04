/*
Copyright 2011 kitano

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
 * class WidgetInput
 *
 * A simple text box with a label above it.
 * It has two images - one for focused and one for unfocused.
 */

#ifndef WIDGETINPUT_H
#define WIDGETINPUT_H

#include "FontEngine.h"
#include "InputState.h"
#include <SDL.h>
#include <string>

class WidgetInput {

protected:

	void loadGraphics(const std::string& filename);

	SDL_Surface *background;

	bool enabled;
	bool inFocus;
	bool pressed;

	std::string text; // the text that has been typed into the box
	unsigned int max_characters;
	int cursor_frame;
	
	Point font_pos;

public:
	WidgetInput();
	
	void logic();
	void render();
	bool checkClick();
	std::string getText() { return text; }
	void setPosition(int x, int y);

	SDL_Rect pos;
};

#endif
