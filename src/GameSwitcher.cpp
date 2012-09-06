/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk

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
#include "SharedResources.h"
#include "Settings.h"
#include "FileParser.h"
#include "UtilsParsing.h"

#include <sstream>

GameSwitcher::GameSwitcher() {

	// The initial state is the title screen
	currentState = new GameStateTitle();

	label_fps = new WidgetLabel();
	done = false;
	music = NULL;
	loadMusic();

	// Load FPS rendering settings
	FileParser infile;
	if(infile.open(mods->locate("menus/fps.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "show_fps") {
				show_fps = eatFirstInt(infile.val,',');
			} else if(infile.key == "position") {
				fps_position.x = eatFirstInt(infile.val,',');
				fps_position.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "color") {
				fps_color.r = eatFirstInt(infile.val,',');
				fps_color.g = eatFirstInt(infile.val,',');
				fps_color.b = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/fps.txt!\n");


}

void GameSwitcher::loadMusic() {
	if (audio && MUSIC_VOLUME) {
		Mix_FreeMusic(music);
		music = Mix_LoadMUS((mods->locate("music/title_theme.ogg")).c_str());
		if (!music)
		  printf("Mix_LoadMUS: %s\n", Mix_GetError());
	}

	if (music) {
		Mix_VolumeMusic(MUSIC_VOLUME);
		Mix_PlayMusic(music, -1);
	}
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
                if (music)
                    Mix_PlayMusic(music, -1);
            }
        }
	}

	currentState->logic();

	// Check if the GameState wants to quit the application
	done = currentState->isExitRequested();

	if (currentState->reload_music) {
		loadMusic();
		currentState->reload_music = false;
	}
}

void GameSwitcher::showFPS(int fps) {
	if (!show_fps) return;
	std::stringstream ss;
	ss << fps << "fps";
	label_fps->set(fps_position.x, fps_position.y, JUSTIFY_CENTER, VALIGN_TOP, ss.str(), fps_color);
	label_fps->render();
}

void GameSwitcher::render() {
	currentState->render();
}

GameSwitcher::~GameSwitcher() {
	delete currentState;
	delete label_fps;
	Mix_FreeMusic(music);
}

