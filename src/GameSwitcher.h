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
 * class GameSwitcher
 *
 * State machine handler between main game modes that take up the entire view/control
 *
 * Examples:
 * - the main gameplay (GameEngine class)
 * - title screen
 * - new game screen (character create)
 * - load game screen
 * - maybe full-video cutscenes
 */
 
#ifndef GAME_SWITCHER_H
#define GAME_SWITCHER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Settings.h"
#include "InputState.h"
#include "FontEngine.h"

const int GAME_STATE_TITLE = 0;
const int GAME_STATE_PLAY = 1;
const int GAME_STATE_LOAD = 2;
const int GAME_STATE_NEW = 3;

class GameState;

class GameSwitcher {
private:
	Mix_Music *music;
	
	GameState *currentState;
	
public:
	GameSwitcher();
	void loadMusic();
	void logic();
	void render();
	~GameSwitcher();
	
	bool done;
};

#endif

