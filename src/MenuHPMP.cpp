/*
Copyright 2011 Clint Bellanger

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

#include "MenuHPMP.h"
#include "ModManager.h"

#include <string>
#include <sstream>


using namespace std;


MenuHPMP::MenuHPMP(SDL_Surface *_screen, FontEngine *_font) {
	screen = _screen;
	font = _font;
	
	hphover = new WidgetLabel(screen, font);
	mphover = new WidgetLabel(screen, font);
	hphover->set(53, 9, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);
	mphover->set(53, 24, JUSTIFY_CENTER, VALIGN_CENTER, "", FONT_WHITE);

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

void MenuHPMP::render(StatBlock *stats, Point mouse) {
	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;
	int mp_bar_length;
	
	// draw trim/background
	src.x = dest.x = 0;
	src.y = dest.y = 0;
	src.w = dest.w = 106;
	src.h = dest.h = 33;
	
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
	dest.x = 3;
	dest.y = 3;
	src.w = hp_bar_length;	
	SDL_BlitSurface(bar_hp, &src, screen, &dest);
	
	// draw mp bar
	dest.y = 18;
	src.w = mp_bar_length;
	SDL_BlitSurface(bar_mp, &src, screen, &dest);		
	
	// if mouseover, draw text
	if (mouse.x <= 106 && mouse.y <= 33) {

		stringstream ss;
		ss << stats->hp << "/" << stats->maxhp;
		hphover->text = ss.str();
		hphover->render();

		ss.str("");
		ss << stats->mp << "/" << stats->maxmp;
		mphover->text = ss.str();
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
