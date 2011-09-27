/*
Copyright 2011 Pavel Kirpichyov (Cheshire)

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
 * MenuEnemy
 *
 * Handles the display of the Enemy info of the screen
 */

#ifndef MENU_ENEMY_H
#define MENU_ENEMY_H

#include "SDL.h"
#include "SDL_image.h"
#include "StatBlock.h"
#include "Utils.h"
#include "FontEngine.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>
#include "Enemy.h"

const int MENU_ENEMY_TIMEOUT = FRAMES_PER_SEC * 10;

class MenuEnemy {
private:
	SDL_Surface *screen;
	FontEngine *font;
	MessageEngine *msg;
	SDL_Surface *background;
	SDL_Surface *bar_hp;
public:
	MenuEnemy(SDL_Surface *_screen, FontEngine *_font, MessageEngine *_msg);
	~MenuEnemy();
	Enemy *enemy;
	void loadGraphics();
	void handleNewMap();
	void logic();
	void render();
	int timeout;
};

#endif
