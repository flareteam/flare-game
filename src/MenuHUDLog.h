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
 * class MenuHUDLog
 */

#ifndef MENU_HUD_LOG_H
#define MENU_HUD_LOG_H

#include "SDL.h"
#include "SDL_image.h"
#include "Settings.h"
#include "Utils.h"
#include "FontEngine.h"

const int MAX_HUD_MESSAGES = 32;

class MenuHUDLog{
private:

	int calcDuration(string s);

	SDL_Surface *screen;
	FontEngine *font;
	string log_msg[MAX_HUD_MESSAGES];
	int msg_age[MAX_HUD_MESSAGES];
	int log_count;
	int paragraph_spacing;
	
public:

	MenuHUDLog(SDL_Surface *screen, FontEngine *font);
	~MenuHUDLog();
	void logic();
	void render();
	void add(string s);
	void clear();
	
	Point list_area;

};

#endif
