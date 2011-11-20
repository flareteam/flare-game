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
 * class MenuLog
 */

#ifndef MENU_LOG_H
#define MENU_LOG_H

#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "WidgetButton.h"
#include "MessageEngine.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>


const int MAX_LOG_MESSAGES = 100;

const int LOG_TYPE_COUNT = 3;
const int LOG_TYPE_QUESTS = 0;
const int LOG_TYPE_MESSAGES = 1;
const int LOG_TYPE_STATISTICS = 2;

class MenuLog {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;

	SDL_Surface *background;
	SDL_Surface *tab_active;
	SDL_Surface *tab_inactive;
	WidgetButton *closeButton;
	
	void loadGraphics();
	void renderTab();
	
	string log_msg[LOG_TYPE_COUNT][MAX_LOG_MESSAGES];
	int log_count[LOG_TYPE_COUNT];
	string tab_labels[LOG_TYPE_COUNT];
	SDL_Rect tab_rect[LOG_TYPE_COUNT];
	Point tab_padding;
	int active_log;
	int paragraph_spacing;
	
public:
	MenuLog(SDL_Surface *screen, InputState *inp, FontEngine *font);
	~MenuLog();

	void logic();
	void render();
	void renderTab(int log_type);
	void add(string s, int log_type);
	void clear(int log_type);
	void clear();
	void clickTab(Point mouse);
	
	bool visible;
	SDL_Rect menu_area;
	SDL_Rect list_area;
	SDL_Rect tabs_area;

};

#endif
