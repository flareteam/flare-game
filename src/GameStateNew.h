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
 * GameStateNew
 *
 * Handle player choices when starting a new game
 * (e.g. character appearance)
 */

#ifndef GAMESTATENEW_H
#define GAMESTATENEW_H

#include "GameState.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <sstream>
#include <vector>

class WidgetButton;
class WidgetCheckBox;
class WidgetInput;
class WidgetLabel;
class WidgetListBox;

class GameStateNew : public GameState {
private:

	void loadGraphics();
	void loadPortrait(const std::string& portrait_filename);
	void loadOptions(const std::string& option_filename);
	std::string getClassTooltip(int index);

	std::vector<std::string> base;
	std::vector<std::string> head;
	std::vector<std::string> portrait;
	int option_count;
	int current_option;

	SDL_Surface *portrait_image;
	SDL_Surface *portrait_border;
	WidgetButton *button_exit;
	WidgetButton *button_create;
	WidgetButton *button_next;
	WidgetButton *button_prev;
	WidgetLabel *label_portrait;
	WidgetLabel *label_name;
	WidgetInput *input_name;
	WidgetCheckBox *button_permadeath;
	WidgetLabel *label_permadeath;
	WidgetLabel *label_classlist;
	WidgetListBox *class_list;

	Point name;
	Point portrait_label;
	Point name_label;
	Point permadeath_label;
	Point classlist_label;
	SDL_Rect portrait_pos;

	SDL_Color color_normal;

public:
	GameStateNew();
	~GameStateNew();
	void logic();
	void render();
	int game_slot;

};

#endif
