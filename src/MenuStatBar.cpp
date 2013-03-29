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
 * MenuStatBar
 *
 * Handles the display of a status bar
 */

#include "Menu.h"
#include "MenuStatBar.h"
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

MenuStatBar::MenuStatBar(std::string type) {

	label = new WidgetLabel();

	orientation = 0; // horizontal
	custom_text_pos = false; // label will be placed in the middle of the bar
	custom_string = "";

	stat_cur = 0;
	stat_max = 0;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/"+type+".txt"))) {
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
			} else if(infile.key == "orientation") {
				int orient = eatFirstInt(infile.val,',');
				if (orient == 1)
					orientation = true;
				else
					orientation = false;
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/%s.txt!\n", type.c_str());

	loadGraphics(type);

	color_normal = font->getColor("menu_normal");
}

void MenuStatBar::loadGraphics(std::string type)
{
	background = loadGraphicSurface("images/menus/bar_" + type + "_background.png");
	bar = loadGraphicSurface("images/menus/bar_" + type + ".png");
}

void MenuStatBar::update(int _stat_cur, int _stat_max, Point _mouse, std::string _custom_string) {
	if (_custom_string != "") custom_string = _custom_string;
	mouse = _mouse;
	stat_cur = _stat_cur;
	stat_max = _stat_max;
}

void MenuStatBar::render() {
	SDL_Rect src;
	SDL_Rect dest;
	int bar_length;

	// position elements based on the window position
	SDL_Rect bar_dest = bar_pos;
	bar_dest.x = bar_pos.x+window_area.x;
	bar_dest.y = bar_pos.y+window_area.y;

	// draw bar background
	dest.x = bar_dest.x;
	dest.y = bar_dest.y;
	src.x = 0;
	src.y = 0;
	src.w = bar_pos.w;
	src.h = bar_pos.h;
	SDL_BlitSurface(background, &src, screen, &dest);

	// draw bar progress based on orientation
	if (orientation == 0) {
		if (stat_max == 0) bar_length = 0;
		else bar_length = (stat_cur * bar_pos.w) / stat_max;
		dest.x = bar_dest.x;
		dest.y = bar_dest.y;
		src.x = src.y = 0;
		src.w = bar_length;
		src.h = bar_pos.h;
		SDL_BlitSurface(bar, &src, screen, &dest);
	} else if (orientation == 1) {
		if (stat_max == 0) bar_length = 0;
		else bar_length = (stat_cur * bar_pos.h) / stat_max;
		src.x = 0;
		src.y = bar_pos.h-bar_length;
		src.w = bar_pos.w;
		src.h = bar_length;
		dest.x = bar_dest.x;
		dest.y = bar_dest.y+src.y;
		SDL_BlitSurface(bar, &src, screen, &dest);
	}

	// if mouseover, draw text
	if (!text_pos.hidden) {
		if (custom_text_pos)
			label->set(bar_dest.x+text_pos.x, bar_dest.y+text_pos.y, text_pos.justify, text_pos.valign, "", color_normal, text_pos.font_style);
		else
			label->set(bar_dest.x+bar_pos.w/2, bar_dest.y+bar_pos.h/2, JUSTIFY_CENTER, VALIGN_CENTER, "", color_normal);

		if (isWithin(bar_dest,mouse)) {
			stringstream ss;
			if (custom_string != "")
				ss << custom_string;
			else
				ss << stat_cur << "/" << stat_max;
			label->set(ss.str());
			label->render();
		}
	}
}

MenuStatBar::~MenuStatBar() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar);
	delete label;
}
