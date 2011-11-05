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

/*
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

#include "GameSwitcher.h"
#include "GameStateTitle.h"
#include "GameStateLoad.h"

GameSwitcher::GameSwitcher(SDL_Surface *_screen, InputState *_inp) {
	inp = _inp;
	screen = _screen;
		
	font = new FontEngine();

	// The initial state is the title screen
	currentState = new GameStateTitle(screen, inp, font);
	
	done = false;
	music = NULL;
	loadMusic();
	
}

void GameSwitcher::loadMusic() {

	music = Mix_LoadMUS((PATH_DATA + "music/title_theme.ogg").c_str());
	if (!music) {
	  printf("Mix_LoadMUS: %s\n", Mix_GetError());
	  SDL_Quit();
	}

	Mix_VolumeMusic(MUSIC_VOLUME);
	Mix_PlayMusic(music, -1);
	
}

void GameSwitcher::logic() {

	/*
	*  Check if a the game state is to be changed and change it if necessary, deleting the old state
	*/
	if (currentState->getRequestedGameState() != NULL) {
		GameState* newState = currentState->getRequestedGameState();

		delete currentState;
	
		currentState = newState;
		
		// if this game state does not provide music, use the title theme
		if (!currentState->hasMusic) {
			if (!Mix_PlayingMusic()) {
				Mix_PlayMusic(music, -1);
			}
		}
		
	}

	currentState->logic();

	// Check if the GameState wants to quit the application
	done = currentState->isExitRequested();
}

void GameSwitcher::render() {
	currentState->render();
}

GameSwitcher::~GameSwitcher() {
	delete font;
	delete currentState;
	Mix_FreeMusic(music);
}

