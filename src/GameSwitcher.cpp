/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
Copyright © 2012 Stefan Beller
Copyright © 2013 Henrik Andersson

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
#include "GameStateCutscene.h"
#include "SharedResources.h"
#include "Settings.h"
#include "FileParser.h"
#include "UtilsParsing.h"

using namespace std;

GameSwitcher::GameSwitcher() {

	// The initial state is the intro cutscene and then title screen
	GameStateTitle *title=new GameStateTitle();
	GameStateCutscene *intro = new GameStateCutscene(title);

	currentState = intro;

	if (!intro->load("intro.txt")) {
		delete intro;
		currentState = title;
	}

	label_fps = new WidgetLabel();
	done = false;
	music = NULL;
	loadMusic();
	loadFPS();
}

void GameSwitcher::loadMusic() {
	if (AUDIO && MUSIC_VOLUME) {
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
	// Check if a the game state is to be changed and change it if necessary, deleting the old state
	GameState* newState = currentState->getRequestedGameState();
	if (newState != NULL) {
		delete currentState;
		currentState = newState;

		// reload the fps meter position
		loadFPS();

		// if this game state does not provide music, use the title theme
		if (!currentState->hasMusic)
			if (!Mix_PlayingMusic())
				if (music)
					Mix_PlayMusic(music, -1);
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
	if (!SHOW_FPS) return;
	string sfps = toString(typeid(fps), &fps) + string(" fps");
	label_fps->set(fps_position.x, fps_position.y, JUSTIFY_LEFT, VALIGN_TOP, sfps, fps_color);
	label_fps->render();
}

void GameSwitcher::loadFPS() {
	// Load FPS rendering settings
	FileParser infile;
	if(infile.open(mods->locate("menus/fps.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "position") {
				fps_position.x = eatFirstInt(infile.val,',');
				fps_position.y = eatFirstInt(infile.val,',');
				fps_corner = eatFirstString(infile.val,',');
			} else if(infile.key == "color") {
				fps_color.r = eatFirstInt(infile.val,',');
				fps_color.g = eatFirstInt(infile.val,',');
				fps_color.b = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	}

	// this is a dummy string used to approximate the fps position when aligned to the right
	font->setFont("font_regular");
	int w = font->calc_width("00 fps");
	int h = font->getLineHeight();

	if (fps_corner == "top_left") {
		// relative to {0,0}, so no changes
	} else if (fps_corner == "top_right") {
		fps_position.x += VIEW_W-w;
	} else if (fps_corner == "bottom_left") {
		fps_position.y += VIEW_H-h;
	} else if (fps_corner == "bottom_right") {
		fps_position.x += VIEW_W-w;
		fps_position.y += VIEW_H-h;
	}

}

void GameSwitcher::render() {
	currentState->render();
}

GameSwitcher::~GameSwitcher() {
	delete currentState;
	delete label_fps;
	Mix_FreeMusic(music);
}

