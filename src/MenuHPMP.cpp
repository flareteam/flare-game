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
#include "FileParser.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"

#include <string>
#include <sstream>


using namespace std;

MenuHPMP::MenuHPMP() {

	hphover = new WidgetLabel();
	mphover = new WidgetLabel();

	loadGraphics();

	orientation = 0; // horizontal

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/hpmp.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "hp_pos") {
				hp_pos.x = eatFirstInt(infile.val,',');
				hp_pos.y = eatFirstInt(infile.val,',');
				hp_pos.w = eatFirstInt(infile.val,',');
				hp_pos.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "mp_pos") {
				mp_pos.x = eatFirstInt(infile.val,',');
				mp_pos.y = eatFirstInt(infile.val,',');
				mp_pos.w = eatFirstInt(infile.val,',');
				mp_pos.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "orientation") {
				orientation = eatFirstInt(infile.val,',');
			}
		}
	}
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

void MenuHPMP::update(StatBlock *_stats, Point _mouse) {
	mouse = _mouse;
	stats = _stats;
}

void MenuHPMP::render() {
	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;
	int mp_bar_length;

	// position elements based on the window position
	SDL_Rect hp_dest,mp_dest;
	hp_dest = hp_pos;
	mp_dest = mp_pos;
	hp_dest.x = hp_pos.x+window_area.x;
	hp_dest.y = hp_pos.y+window_area.y;
	mp_dest.x = mp_pos.x+window_area.x;
	mp_dest.y = mp_pos.y+window_area.y;

	// draw hp background
	dest.x = hp_dest.x;
	dest.y = hp_dest.y;
	src.x = 0;
	src.y = 0;
	src.w = hp_pos.w;
	src.h = hp_pos.h;
	SDL_BlitSurface(background, &src, screen, &dest);

	// draw mp background
	dest.x = mp_dest.x;
	dest.y = mp_dest.y;
	src.x = 0;
	src.y = hp_pos.h;
	src.w = mp_pos.w;
	src.h = mp_pos.h;
	SDL_BlitSurface(background, &src, screen, &dest);

	if (orientation == 0) {
		// draw hp bar
		if (stats->maxhp == 0) hp_bar_length = 0;
		else hp_bar_length = (stats->hp * hp_pos.w) / stats->maxhp;
		dest.x = hp_dest.x;
		dest.y = hp_dest.y;
		src.x = src.y = 0;
		src.w = hp_bar_length;
		src.h = hp_pos.h;
		SDL_BlitSurface(bar_hp, &src, screen, &dest);

		// draw mp bar
		if (stats->maxmp == 0) mp_bar_length = 0;
		else mp_bar_length = (stats->mp * hp_pos.w) / stats->maxmp;
		dest.x = mp_dest.x;
		dest.y = mp_dest.y;
		src.x = src.y = 0;
		src.w = mp_bar_length;
		src.h = mp_pos.h;
		SDL_BlitSurface(bar_mp, &src, screen, &dest);
	} else if (orientation == 1) {
		// draw hp bar
		if (stats->maxhp == 0) hp_bar_length = 0;
		else hp_bar_length = (stats->hp * hp_pos.h) / stats->maxhp;
		src.x = 0;
		src.y = hp_pos.h-hp_bar_length;
		src.w = hp_pos.w;
		src.h = hp_bar_length;
		dest.x = hp_dest.x;
		dest.y = hp_dest.y+src.y;
		SDL_BlitSurface(bar_hp, &src, screen, &dest);

		// draw mp bar
		if (stats->maxmp == 0) mp_bar_length = 0;
		else mp_bar_length = (stats->mp * mp_pos.h) / stats->maxmp;
		src.x = 0;
		src.y = mp_pos.h-mp_bar_length;
		src.w = mp_pos.w;
		src.h = mp_bar_length;
		dest.x = mp_dest.x;
		dest.y = mp_dest.y+src.y;
		SDL_BlitSurface(bar_mp, &src, screen, &dest);
	}

	// if mouseover, draw text
	hphover->set(hp_dest.x+hp_pos.w/2, hp_dest.y+hp_pos.h/2, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);
	mphover->set(mp_dest.x+mp_pos.w/2, mp_dest.y+mp_pos.h/2, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);
	if (isWithin(hp_dest,mouse) || isWithin(mp_dest,mouse)) {

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
