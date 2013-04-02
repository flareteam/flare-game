/*
Copyright © 2012-2013 Henrik Andersson

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

#include <queue>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "Settings.h"
#include "SharedResources.h"
#include "Utils.h"
#include "UtilsParsing.h"
#include "WidgetLabel.h"

#ifndef GAMESTATECUTSCENE_H
#define GAMESTATECUTSCENE_H

#include "GameState.h"

class SceneComponent {
public:
	std::string type;
	SDL_Surface *i;
	std::string s;
	int x,y,z;
};

class Scene {
private:
	int frame_counter;
	int pause_frames;
	std::string caption;
	Point caption_size;
	SDL_Surface *art;
	SDL_Rect art_dest;
	SoundManager::SoundID sid;

public:
	Scene();
	~Scene();
	bool logic();
	void render();

	std::queue<SceneComponent> components;

};

class GameStateCutscene : public GameState {
private:
	GameState *previous_gamestate;
	std::string dest_map;
	Point dest_pos;
	bool scale_graphics;

	std::queue<Scene> scenes;

	SDL_Surface *loadImage(std::string filename);

public:
	GameStateCutscene(GameState *game_state);
	~GameStateCutscene();

	bool load(std::string filename);
	void logic();
	void render();

	int game_slot;
};

#endif

