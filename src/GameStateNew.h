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
 * GameStateNew
 * 
 * Handle player choices when starting a new game
 * (e.g. character appearance)
 */

#ifndef GAMESTATENEW_H
#define GAMESTATENEW_H

#include "InputState.h"
#include "FontEngine.h"
#include "WidgetButton.h"
#include "WidgetInput.h"
#include "FileParser.h"
#include "Settings.h"
#include "GameState.h"
#include "UtilsParsing.h"
#include "SharedResources.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <sstream>


class WidgetLabel;


const int PLAYER_OPTION_MAX = 32;

class GameStateNew : public GameState {
private:

	void loadGraphics();
	void loadPortrait(const std::string& portrait_filename);
	void loadOptions(const std::string& option_filename);

	std::string base[PLAYER_OPTION_MAX];
	std::string head[PLAYER_OPTION_MAX];
	std::string portrait[PLAYER_OPTION_MAX];
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

public:
	GameStateNew();
	~GameStateNew();
	void logic();
	void render();
	int game_slot;
	
};

#endif
