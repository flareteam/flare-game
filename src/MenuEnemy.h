/*
Copyright © 2011-2012 Pavel Kirpichyov (Cheshire)

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

#include "Enemy.h"
#include "Settings.h"

#include <SDL.h>
#include <SDL_image.h>

const int MENU_ENEMY_TIMEOUT = FRAMES_PER_SEC * 10;

class MenuEnemy : public Menu {
private:
	SDL_Surface *background;
	SDL_Surface *bar_hp;
	SDL_Rect bar_pos;
	SDL_Rect text_pos;
	bool custom_text_pos;
public:
	MenuEnemy();
	~MenuEnemy();
	Enemy *enemy;
	void loadGraphics();
	void handleNewMap();
	void logic();
	void render();
	int timeout;
};

#endif
