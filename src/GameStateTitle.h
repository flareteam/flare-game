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


#pragma once
#ifndef GAMESTATETITLE_H
#define GAMESTATETITLE_H

#include "GameState.h"

class WidgetButton;
class WidgetLabel;

class GameStateTitle : public GameState {
private:
	SDL_Surface *logo;
	WidgetButton *button_play;
	WidgetButton *button_exit;
	WidgetButton *button_cfg;
	WidgetLabel *label_version;

public:
	GameStateTitle();
	~GameStateTitle();
	void logic();
	void render();

	// switch
	bool exit_game;
	bool load_game;

};

#endif

