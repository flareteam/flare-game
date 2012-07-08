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

#include "MenuActiveEffects.h"
#include "ModManager.h"
#include "SharedResources.h"
#include "StatBlock.h"

#include <string>
#include <sstream>


using namespace std;


MenuActiveEffects::MenuActiveEffects(SDL_Surface *_icons) {
	icons = _icons;

	loadGraphics();

	limits.slow = 0;
	limits.bleed = 0;
	limits.stun = 0;
	limits.immobilize = 0;
	limits.immunity = 0;
	limits.transform = 0;
	limits.haste = 0;
	limits.hot = 0;
	limits.shield = 0;
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
	pos.x = 106 + (index * 32);
	pos.y = 0;
	
	src.x = (icon_id % 16) * 32;
	src.y = (icon_id / 16) * 32;
	src.w = src.h = 32;

	overlay.x = 0;
	overlay.y = 32 * ((float)current/max);
	overlay.w = 32;
	overlay.h = 32 - overlay.y;

	SDL_BlitSurface(icons,&src,screen,&pos);
	SDL_BlitSurface(timer,&overlay,screen,&pos);
}

void MenuActiveEffects::render(StatBlock *stats) {
	// Make sure the list of effects is empty
	if (effects.size() > 0) effects.clear();

	// Append active effects to the effects list
	if (stats->slow_duration > 0) {
		effects.push_back("slow");
		if (limits.slow == 0) limits.slow = stats->slow_duration;
	}
	if (stats->bleed_duration > 0) {
		effects.push_back("bleed");
		if (limits.bleed == 0) limits.bleed = stats->bleed_duration;
	}
	if (stats->stun_duration > 0) {
		effects.push_back("stun");
		if (limits.stun == 0) limits.stun = stats->stun_duration;
	}
	if (stats->immobilize_duration > 0) {
		effects.push_back("immobilize");
		if (limits.immobilize == 0) limits.immobilize = stats->immobilize_duration;
	}
	if (stats->immunity_duration > 0) {
		effects.push_back("immunity");
		if (limits.immunity == 0) limits.immunity = stats->immunity_duration;
	}
	if (stats->transform_duration > 0) {
		effects.push_back("transform");
		if (limits.transform == 0) limits.transform = stats->transform_duration;
	}
	if (stats->haste_duration > 0) {
		effects.push_back("haste");
		if (limits.haste == 0) limits.haste = stats->haste_duration;
	}
	if (stats->hot_duration > 0) {
		effects.push_back("hot");
		if (limits.hot == 0) limits.hot = stats->hot_duration;
	}
	if (stats->shield_hp > 0) {
		effects.push_back("shield");
		if (limits.shield == 0) limits.shield = stats->shield_hp;
	}
	if (stats->blocking) {
		effects.push_back("block");
	}

	// Step through the list of effects and render those that are active
	for (unsigned int i=0; i<effects.size(); i++) {
		if (effects[i] == "slow")
			renderIcon(14,i,stats->slow_duration,limits.slow);
		if (effects[i] == "bleed")
			renderIcon(5,i,stats->bleed_duration,limits.bleed);
		if (effects[i] == "stun")
			renderIcon(10,i,stats->stun_duration,limits.stun);
		if (effects[i] == "immobilize")
			renderIcon(4,i,stats->immobilize_duration,limits.immobilize);
		if (effects[i] == "immunity")
			renderIcon(9,i,stats->immunity_duration,limits.immunity);
		if (effects[i] == "transform")
			renderIcon(21,i,stats->transform_duration,limits.transform);
		if (effects[i] == "haste")
			renderIcon(12,i,stats->haste_duration,limits.haste);
		if (effects[i] == "hot")
			renderIcon(7,i,stats->hot_duration,limits.hot);
		if (effects[i] == "shield")
			renderIcon(11,i,stats->shield_hp,limits.shield);
		if (effects[i] == "block")
			renderIcon(3,i,0,0);
	}
}

MenuActiveEffects::~MenuActiveEffects() {
	SDL_FreeSurface(timer);
}
