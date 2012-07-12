/*
Copyright © 2011-2012 Clint Bellanger

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

#include <SDL.h>
#include <SDL_image.h>

#include <string>

class WidgetButton;
class WidgetTabControl;

const int MAX_LOG_MESSAGES = 32;

const int LOG_TYPE_COUNT = 3;
const int LOG_TYPE_QUESTS = 0;
const int LOG_TYPE_MESSAGES = 1;
const int LOG_TYPE_STATISTICS = 2;

class MenuLog : public Menu {
private:

	SDL_Surface *background;
	WidgetButton *closeButton;
  WidgetTabControl *tabControl;
	
	void loadGraphics();
	
	std::string log_msg[LOG_TYPE_COUNT][MAX_LOG_MESSAGES];
	SDL_Surface *msg_buffer[LOG_TYPE_COUNT][MAX_LOG_MESSAGES];
	int log_count[LOG_TYPE_COUNT];
	std::string tab_labels[LOG_TYPE_COUNT];
	SDL_Rect tab_rect[LOG_TYPE_COUNT];
	int paragraph_spacing;
	
public:
	MenuLog();
	~MenuLog();

	void logic();
  void tabsLogic();
	void render();
	void add(const std::string& s, int log_type);
	void remove(int msg_index, int log_type);
	void clear(int log_type);
	void clear();
	
	bool visible;
	SDL_Rect menu_area;

};

#endif
