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

	limits.slow = 0;
	limits.bleed = 0;
	limits.stun = 0;
	limits.immobilize = 0;
	limits.immunity = 0;
	limits.transform = 0;
	limits.haste = 0;
	limits.hot = 0;
	limits.shield = 0;

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
	} else fprintf(stderr, "Unable to open activeeffects.txt!\n");

	loadGraphics();
}

void MenuActiveEffects::loadGraphics() {

	timer = IMG_Load(mods->locate("images/menus/disabled.png").c_str());
	if(!timer) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = timer;
	timer = SDL_DisplayFormatAlpha(timer);
	SDL_FreeSurface(cleanup);

}

void MenuActiveEffects::renderIcon(int icon_id, int index, int current, int max){
	SDL_Rect pos,src,overlay;
	if (orientation == 0) {
		pos.x = window_area.x + (index * ICON_SIZE_SMALL);
		pos.y = window_area.y;
	} else if (orientation == 1) {
		pos.x = window_area.x;
		pos.y = window_area.y + (index * ICON_SIZE_SMALL);;
	}
	
	src.x = (icon_id % 16) * ICON_SIZE_SMALL;
	src.y = (icon_id / 16) * ICON_SIZE_SMALL;
	src.w = src.h = ICON_SIZE_SMALL;

	overlay.x = 0;
	overlay.y = ICON_SIZE_SMALL * ((float)current/max);
	overlay.w = ICON_SIZE_SMALL;
	overlay.h = ICON_SIZE_SMALL - overlay.y;

	SDL_BlitSurface(icons,&src,screen,&pos);
	SDL_BlitSurface(timer,&overlay,screen,&pos);
}

void MenuActiveEffects::update(StatBlock *_stats) {
	stats = _stats;
}

void MenuActiveEffects::render() {
	// Make sure the list of effects is empty
	if (effects.size() > 0) effects.clear();

	// Append active effects to the effects list
	if (stats->slow_duration > 0) effects.push_back("slow");
	if (stats->bleed_duration > 0) effects.push_back("bleed");
	if (stats->stun_duration > 0) effects.push_back("stun");
	if (stats->immobilize_duration > 0) effects.push_back("immobilize");
	if (stats->immunity_duration > 0) effects.push_back("immunity");
	if (stats->transform_duration > 0) effects.push_back("transform");
	if (stats->haste_duration > 0) effects.push_back("haste");
	if (stats->hot_duration > 0) effects.push_back("hot");
	if (stats->shield_hp > 0) effects.push_back("shield");
	if (stats->blocking) effects.push_back("block");

	// Step through the list of effects and render those that are active
	for (unsigned int i=0; i<effects.size(); i++) {
		if (effects[i] == "slow")
			renderIcon(14,i,stats->slow_duration,stats->slow_duration_total);
		if (effects[i] == "bleed")
			renderIcon(5,i,stats->bleed_duration,stats->bleed_duration_total);
		if (effects[i] == "stun")
			renderIcon(10,i,stats->stun_duration,stats->stun_duration_total);
		if (effects[i] == "immobilize")
			renderIcon(4,i,stats->immobilize_duration,stats->immobilize_duration_total);
		if (effects[i] == "immunity")
			renderIcon(9,i,stats->immunity_duration,stats->immunity_duration_total);
		if (effects[i] == "transform")
			renderIcon(21,i,stats->transform_duration,stats->transform_duration_total);
		if (effects[i] == "haste")
			renderIcon(12,i,stats->haste_duration,stats->haste_duration_total);
		if (effects[i] == "hot")
			renderIcon(7,i,stats->hot_duration,stats->hot_duration_total);
		if (effects[i] == "shield")
			renderIcon(11,i,stats->shield_hp,stats->shield_hp_total);
		if (effects[i] == "block")
			renderIcon(3,i,0,0);
	}
}

MenuActiveEffects::~MenuActiveEffects() {
	SDL_FreeSurface(timer);
}
