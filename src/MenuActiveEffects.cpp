/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Justin Jacobs

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
 * MenuActiveEffects
 *
 * Handles the display of active effects (buffs/debuffs)
 */

#include "Menu.h"
#include "MenuActiveEffects.h"
#include "ModManager.h"
#include "Settings.h"
#include "SharedResources.h"
#include "StatBlock.h"
#include "FileParser.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"

#include <string>
#include <sstream>


using namespace std;


MenuActiveEffects::MenuActiveEffects(SDL_Surface *_icons) {
	stats = NULL;
	icons = _icons;
	orientation = 0; // horizontal

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/activeeffects.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "orientation") {
				orientation = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/activeeffects.txt!\n");

	loadGraphics();
}

void MenuActiveEffects::loadGraphics() {

	timer = IMG_Load(mods->locate("images/menus/disabled.png").c_str());
	if(!timer) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
	} else {
		// optimize
		SDL_Surface *cleanup = timer;
		timer = SDL_DisplayFormatAlpha(timer);
		SDL_FreeSurface(cleanup);
	}
}

void MenuActiveEffects::renderIcon(int icon_id, int index, int current, int max){
	if (icon_id > -1) {
		SDL_Rect pos,src,overlay;
		if (orientation == 0) {
			pos.x = window_area.x + (index * ICON_SIZE);
			pos.y = window_area.y;
		} else if (orientation == 1) {
			pos.x = window_area.x;
			pos.y = window_area.y + (index * ICON_SIZE);;
		}

		src.x = (icon_id % 16) * ICON_SIZE;
		src.y = (icon_id / 16) * ICON_SIZE;
		src.w = src.h = ICON_SIZE;

		SDL_BlitSurface(icons,&src,screen,&pos);

		if (max > 0) {
			overlay.x = 0;
			overlay.y = (ICON_SIZE * current) / max;
			overlay.w = ICON_SIZE;
			overlay.h = ICON_SIZE - overlay.y;

			SDL_BlitSurface(timer,&overlay,screen,&pos);
		}
	}
}

void MenuActiveEffects::update(StatBlock *_stats) {
	stats = _stats;
}

void MenuActiveEffects::render() {
	// Step through the list of effects and render those that are active
	for (unsigned int i=0; i<stats->effects.effect_list.size(); i++) {
		std::string type = stats->effects.effect_list[i].type;
		int icon = stats->effects.effect_list[i].icon;
		int ticks = stats->effects.effect_list[i].ticks;
		int duration = stats->effects.effect_list[i].duration;
		int shield_hp = stats->effects.effect_list[i].shield_hp;
		int shield_maxhp = stats->effects.effect_list[i].shield_maxhp;

		if (type == "shield")
			renderIcon(icon,i,shield_hp,shield_maxhp);
		else if (type == "block")
			renderIcon(icon,i,0,0);
		else if (ticks > 0 && duration > 0)
			renderIcon(icon,i,ticks,duration);
	}
}

MenuActiveEffects::~MenuActiveEffects() {
	SDL_FreeSurface(timer);
}
