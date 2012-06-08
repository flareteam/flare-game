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

#ifndef WIDGET_ScrollBar_H
#define WIDGET_ScrollBar_H

#include "Widget.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>


class WidgetScrollBar : public Widget {
private:

	std::string fileName; // the path to the ScrollBars background image

	SDL_Surface *scrollbars;
	Mix_Chunk *click;
	
	int barHeight;
	
public:
	WidgetScrollBar(int x, int y, int height, const std::string& _fileName);
	~WidgetScrollBar();

	void loadArt();
	int checkClick();
	void render();
	void refresh(int x, int y);

	SDL_Rect pos_up;
	SDL_Rect pos_down;
	bool pressed_up;
	bool pressed_down;
};

#endif
