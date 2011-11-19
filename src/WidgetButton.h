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
 * class WidgetButton
 */

#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "FontEngine.h"
#include "InputState.h"
#include <string>

const int BUTTON_GFX_NORMAL = 0;
const int BUTTON_GFX_PRESSED = 1;
const int BUTTON_GFX_HOVER = 2;
const int BUTTON_GFX_DISABLED = 3;

class WidgetButton {
private:
	SDL_Surface *screen;
	FontEngine *font;
	InputState *inp;

	std::string fileName; // the path to the buttons background image

	SDL_Surface *buttons;
	Mix_Chunk *click;
	
public:
	WidgetButton(SDL_Surface *_screen, FontEngine *_font, InputState *_inp, const std::string& _fileName);
	~WidgetButton();

	void loadArt();
	bool checkClick();
	void render();

	string label;
	SDL_Rect pos;
	bool enabled;
	bool pressed;
};

#endif
