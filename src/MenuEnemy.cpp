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
 * Handles the display of the Enemy bar on the HUD
 */

#include "Menu.h"
#include "MenuEnemy.h"
#include "SharedResources.h"
#include "WidgetLabel.h"

#include <string>
#include <sstream>

using namespace std;


MenuEnemy::MenuEnemy() {
	loadGraphics();
	enemy = NULL;
	timeout = 0;
}

void MenuEnemy::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/bar_enemy.png").c_str());
	bar_hp = IMG_Load(mods->locate("images/menus/bar_hp.png").c_str());

	if(!background || !bar_hp) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = bar_hp;
	bar_hp = SDL_DisplayFormatAlpha(bar_hp);
	SDL_FreeSurface(cleanup);
}

void MenuEnemy::handleNewMap() {
	enemy = NULL;
}

void MenuEnemy::logic() {

	// after a fixed amount of time, hide the enemy display
	if (timeout > 0) timeout--;
	if (timeout == 0) enemy = NULL;
}

void MenuEnemy::render() {
	if (enemy == NULL) return;

	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;

	// draw trim/background
	dest.x = window_area.x;
	dest.y = window_area.y;
	dest.w = window_area.w;
	dest.h = window_area.h;

	SDL_BlitSurface(background, NULL, screen, &dest);

	if (enemy->stats.maxhp == 0)
		hp_bar_length = 0;
	else
		hp_bar_length = (enemy->stats.hp * 100) / enemy->stats.maxhp;

	// draw hp bar

	dest.x = window_area.x+3;
	dest.y = window_area.y+18;

	src.x = 0;
	src.y = 0;
	src.h = 12;
	src.w = hp_bar_length;

	SDL_BlitSurface(bar_hp, &src, screen, &dest);

	stringstream ss;
	ss.str("");
	if (enemy->stats.hp > 0)
		ss << enemy->stats.hp << "/" << enemy->stats.maxhp;
	else
		ss << msg->get("Dead");

	WidgetLabel label;

	label.set(window_area.x+window_area.w/2, window_area.y+9, JUSTIFY_CENTER, VALIGN_CENTER, msg->get("%s level %d", enemy->stats.level, enemy->stats.name), FONT_WHITE);
	label.render();

	label.set(window_area.x+window_area.w/2, window_area.y+24, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);
	label.render();


	//SDL_UpdateRects(screen, 1, &dest);
}

MenuEnemy::~MenuEnemy() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar_hp);
}
