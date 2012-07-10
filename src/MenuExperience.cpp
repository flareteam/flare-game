/*
Copyright © 2011-2012 Clint Bellanger

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
 * MenuExperience
 *
 * Handles the display of the Experience bar on the HUD
 */

#include "Menu.h"
#include "MenuExperience.h"
#include "ModManager.h"
#include "SharedResources.h"
#include "StatBlock.h"
#include "WidgetLabel.h"

#include <SDL_mixer.h>

#include <string>
#include <sstream>

using namespace std;


MenuExperience::MenuExperience() {
	loadGraphics();
}

void MenuExperience::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/menu_xp.png").c_str());
	bar = IMG_Load(mods->locate("images/menus/bar_xp.png").c_str());

	if(!background || !bar) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		Mix_CloseAudio();
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = bar;
	bar = SDL_DisplayFormatAlpha(bar);
	SDL_FreeSurface(cleanup);
}

void MenuExperience::update(StatBlock *_stats, Point _mouse) {
	mouse = _mouse;
	stats = _stats;
}

/**
 * Display the XP bar background and current progress.
 * On mouseover, display progress in text form.
 */
void MenuExperience::render() {

	SDL_Rect src;
	SDL_Rect dest;
	int xp_bar_length;

	// don't display anything if max level
	if (stats->level < 1 || stats->level == MAX_CHARACTER_LEVEL) return;

	// lay down the background image first
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h-16;
	dest.x = window_area.x;
	dest.y = window_area.y;
	SDL_BlitSurface(background, &src, screen, &dest);

	// calculate the length of the xp bar
	// when at a new level, 0% progress
	src.x = 0;
	src.y = 0;
	int required = stats->xp_table[stats->level] - stats->xp_table[stats->level-1];
	int current = stats->xp - stats->xp_table[stats->level-1];
	xp_bar_length = (current * 100) / required;
	src.w = xp_bar_length;
	src.h = 4;
	dest.x = window_area.x+3;
	dest.y = window_area.y+3;

	// draw xp bar
	SDL_BlitSurface(bar, &src, screen, &dest);


	string text_label;

	// if mouseover, draw text
	if (isWithin(window_area, mouse)) {

		if (stats->level < MAX_CHARACTER_LEVEL) {
			text_label = msg->get("XP: %d/%d", stats->xp, stats->xp_table[stats->level]);
		}
		else {
			text_label = msg->get("XP: %d", stats->xp);
		}

		WidgetLabel label;
		label.set(window_area.x+2, window_area.y+window_area.h-14, JUSTIFY_LEFT, VALIGN_TOP, text_label, FONT_WHITE);
		label.render();
	}
}

MenuExperience::~MenuExperience() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar);
}

