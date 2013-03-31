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
#include "FileParser.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"

#include <string>
#include <sstream>

using namespace std;


MenuEnemy::MenuEnemy() {
	custom_text_pos = false;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/enemy.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "pos") {
				bar_pos.x = eatFirstInt(infile.val,',');
				bar_pos.y = eatFirstInt(infile.val,',');
				bar_pos.w = eatFirstInt(infile.val,',');
				bar_pos.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "text_pos") {
				custom_text_pos = true;
				text_pos = eatLabelInfo(infile.val);
			}
		}
		infile.close();
	}

	loadGraphics();
	enemy = NULL;
	timeout = 0;

	color_normal = font->getColor("menu_normal");
}

void MenuEnemy::loadGraphics()
{
	background = loadGraphicSurface("images/menus/enemy_bar.png");
	bar_hp = loadGraphicSurface("images/menus/enemy_bar_hp.png");
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
	if (enemy->stats.corpse && enemy->stats.corpse_ticks == 0) return;

	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;

	// draw trim/background
	dest.x = window_area.x+bar_pos.x;
	dest.y = window_area.y+bar_pos.y;
	dest.w = bar_pos.w;
	dest.h = bar_pos.h;

	SDL_BlitSurface(background, NULL, screen, &dest);

	if (enemy->stats.maxhp == 0)
		hp_bar_length = 0;
	else
		hp_bar_length = (enemy->stats.hp * 100) / enemy->stats.maxhp;

	// draw hp bar

	src.x = 0;
	src.y = 0;
	src.h = bar_pos.h;
	src.w = hp_bar_length;

	SDL_BlitSurface(bar_hp, &src, screen, &dest);

	stringstream ss;
	ss.str("");
	if (enemy->stats.hp > 0)
		ss << enemy->stats.hp << "/" << enemy->stats.maxhp;
	else
		ss << msg->get("Dead");

	if (!text_pos.hidden) {
		WidgetLabel label;

		if (custom_text_pos) {
			label.set(window_area.x+text_pos.x, window_area.y+text_pos.y, text_pos.justify, text_pos.valign, msg->get("%s level %d", enemy->stats.level, enemy->stats.name), color_normal, text_pos.font_style);
		} else {
			label.set(window_area.x+bar_pos.x+bar_pos.w/2, window_area.y+bar_pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, msg->get("%s level %d", enemy->stats.level, enemy->stats.name), color_normal);
		}
		label.render();

		label.set(window_area.x+bar_pos.x+bar_pos.w/2, window_area.y+bar_pos.y+bar_pos.h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), color_normal);
		label.render();
	}


	//SDL_UpdateRects(screen, 1, &dest);
}

MenuEnemy::~MenuEnemy() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar_hp);
}
