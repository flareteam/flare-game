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
		SDL_Quit();
		exit(1);
	}

	// optimize
	SDL_Surface *cleanup = timer;
	timer = SDL_DisplayFormatAlpha(timer);
	SDL_FreeSurface(cleanup);

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
	for (unsigned int i=0; i<stats->effects.size(); i++) {
		if (stats->effects[i].type == "slow")
			renderIcon(stats->effects[i].icon,i,stats->slow_duration,stats->slow_duration_total);
		if (stats->effects[i].type == "bleed")
			renderIcon(stats->effects[i].icon,i,stats->bleed_duration,stats->bleed_duration_total);
		if (stats->effects[i].type == "stun")
			renderIcon(stats->effects[i].icon,i,stats->stun_duration,stats->stun_duration_total);
		if (stats->effects[i].type == "immobilize")
			renderIcon(stats->effects[i].icon,i,stats->immobilize_duration,stats->immobilize_duration_total);
		if (stats->effects[i].type == "immunity")
			renderIcon(stats->effects[i].icon,i,stats->immunity_duration,stats->immunity_duration_total);
		if (stats->effects[i].type == "transform")
			renderIcon(stats->effects[i].icon,i,stats->transform_duration,stats->transform_duration_total);
		if (stats->effects[i].type == "haste")
			renderIcon(stats->effects[i].icon,i,stats->haste_duration,stats->haste_duration_total);
		if (stats->effects[i].type == "hot")
			renderIcon(stats->effects[i].icon,i,stats->hot_duration,stats->hot_duration_total);
		if (stats->effects[i].type == "shield")
			renderIcon(stats->effects[i].icon,i,stats->shield_hp,stats->shield_hp_total);
		if (stats->effects[i].type == "block")
			renderIcon(stats->effects[i].icon,i,0,0);
	}
}

MenuActiveEffects::~MenuActiveEffects() {
	SDL_FreeSurface(timer);
}
