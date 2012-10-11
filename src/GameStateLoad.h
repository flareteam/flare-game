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
 * GameStateLoad
 * 
 * Display the current save-game slots
 * Allow the player to continue a previous game
 * Allow the player to start a new game
 * Allow the player to abandon a previous game
 */

#ifndef GAMESTATELOAD_H
#define GAMESTATELOAD_H

#include "StatBlock.h"
#include "GameState.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <sstream>

class ItemManager;
class MenuConfirm;
class WidgetButton;
class WidgetLabel;


const int GAME_SLOT_MAX = 4;

class GameStateLoad : public GameState {
private:

	void loadGraphics();
	void loadPortrait(int slot);
	std::string getMapName(const std::string& map_filename);
	void updateButtons();

	ItemManager *items;
	WidgetButton *button_exit;
	WidgetButton *button_action;
	WidgetButton *button_alternate;
	WidgetLabel *label_loading;
	WidgetLabel *label_name[GAME_SLOT_MAX];
	WidgetLabel *label_level[GAME_SLOT_MAX];
	WidgetLabel *label_map[GAME_SLOT_MAX];

	MenuConfirm *confirm;

	SDL_Surface *background;
	SDL_Surface *selection;
	SDL_Surface *portrait_border;
	SDL_Surface *portrait;
	std::vector<SDL_Surface *> sprites[GAME_SLOT_MAX];
	StatBlock stats[GAME_SLOT_MAX];
	std::vector<int> equipped[GAME_SLOT_MAX];
	std::vector<std::string> preview_layer;
	SDL_Rect slot_pos[GAME_SLOT_MAX];
	std::string current_map[GAME_SLOT_MAX];

	bool loading_requested;
	bool loading;
	bool loaded;
	
	Point name_pos;
	Point level_pos;
	Point map_pos;
	Point sprites_pos;
	Point loading_pos;
	
	// animation info
	int current_frame;
	int frame_ticker;

	SDL_Rect portrait_pos;
	SDL_Rect gameslot_pos;
	SDL_Rect preview_pos;

	SDL_Color color_normal;
	
public:
	GameStateLoad();
	~GameStateLoad();

	void logic();
	void logicLoading();
	void render();	
	void readGameSlot(int slot);
	void readGameSlots();
	void loadPreview(int slot);
	
	bool load_game;
	int selected_slot;
};

#endif
