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
 * MenuHPMP
 *
 * Handles the display of the HP and MP bars at the top/left of the screen
 */

#include "Menu.h"
#include "MenuHPMP.h"
#include "ModManager.h"
#include "SharedResources.h"
#include "StatBlock.h"
#include "WidgetLabel.h"

#include <string>
#include <sstream>


using namespace std;

MenuHPMP::MenuHPMP() {

	hphover = new WidgetLabel();
	mphover = new WidgetLabel();

	loadGraphics();
}

void MenuHPMP::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/bar_hp_mp.png").c_str());
	bar_hp = IMG_Load(mods->locate("images/menus/bar_hp.png").c_str());
	bar_mp = IMG_Load(mods->locate("images/menus/bar_mp.png").c_str());

	if(!background || !bar_hp || !bar_mp) {
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

	cleanup = bar_mp;
	bar_mp = SDL_DisplayFormatAlpha(bar_mp);
	SDL_FreeSurface(cleanup);
}

void MenuHPMP::render() {
}

void MenuHPMP::render(StatBlock *stats, Point mouse) {
	hphover->set(window_area.x+window_area.w/2, window_area.y+9, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);
	mphover->set(window_area.x+window_area.w/2, window_area.y+24, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);

	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;
	int mp_bar_length;

	// draw trim/background
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.x = src.y = 0;
	src.w = dest.w = window_area.w;
	src.h = dest.h = window_area.h;

	SDL_BlitSurface(background, &src, screen, &dest);

	if (stats->maxhp == 0)
		hp_bar_length = 0;
	else
		hp_bar_length = (stats->hp * 100) / stats->maxhp;

	if (stats->maxmp == 0)
		mp_bar_length = 0;
	else
		mp_bar_length = (stats->mp * 100) / stats->maxmp;

	// draw hp bar
	src.x = 0;
	src.y = 0;
	src.h = 12;
	dest.x = window_area.x+3;
	dest.y = window_area.y+3;
	src.w = hp_bar_length;
	SDL_BlitSurface(bar_hp, &src, screen, &dest);

	// draw mp bar
	dest.y = window_area.y+18;
	src.w = mp_bar_length;
	SDL_BlitSurface(bar_mp, &src, screen, &dest);

	// if mouseover, draw text
	if (isWithin(window_area,mouse)) {

		stringstream ss;
		ss << stats->hp << "/" << stats->maxhp;
		hphover->set(ss.str());
		hphover->render();

		ss.str("");
		ss << stats->mp << "/" << stats->maxmp;
		mphover->set(ss.str());
		mphover->render();
	}
}

MenuHPMP::~MenuHPMP() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar_hp);
	SDL_FreeSurface(bar_mp);
	delete hphover;
	delete mphover;
}
