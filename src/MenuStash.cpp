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
 * class MenuStash
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuStash.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"

using namespace std;


MenuStash::MenuStash(ItemManager *_items, StatBlock *_stats) {
	items = _items;
	stats = _stats;

	visible = false;
	updated = false;
	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/stash.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "slots_area") {
				slots_area.x = eatFirstInt(infile.val,',');
				slots_area.y = eatFirstInt(infile.val,',');
			} else if (infile.key == "stash_cols"){
				slots_cols = eatFirstInt(infile.val,',');
			} else if (infile.key == "stash_rows"){
				slots_rows = eatFirstInt(infile.val,',');
			} else if (infile.key == "title"){
				title_pos.x =  eatFirstInt(infile.val,',');
				title_pos.y =  eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open stash.txt!\n");

	STASH_SLOTS = slots_cols * slots_rows;
}

void MenuStash::loadGraphics() {
	background = IMG_Load(mods->locate("images/menus/vendor.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);
}

void MenuStash::update() {
	slots_area.x += window_area.x;
	slots_area.y += window_area.y;
	slots_area.w = slots_cols*32;
	slots_area.h = slots_rows*32;

	stock.init( STASH_SLOTS, items, slots_area, ICON_SIZE_32, slots_cols);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;
}

void MenuStash::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}
}

void MenuStash::render() {
	if (!visible) return;
	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = window_area.w;
	src.h = dest.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &dest);

	// close button
	closeButton->render();

	// text overlay
	WidgetLabel label;
	label.set(window_area.x+title_pos.x, window_area.y+title_pos.y, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Shared Stash"), FONT_WHITE);
	label.render();

	// show stock
	stock.render();
}

/**
 * Start dragging a vendor item
 * Players can drag an item to their inventory.
 */
ItemStack MenuStash::click(InputState * input) {
	ItemStack stack = stock.click(input);
	return stack;
}

/**
 * Cancel the dragging initiated by the click()
 */
void MenuStash::itemReturn(ItemStack stack) {
	stock.itemReturn(stack);
}

void MenuStash::add(ItemStack stack) {
	stock.add(stack);
}

TooltipData MenuStash::checkTooltip(Point mouse) {
	return stock.checkTooltip( mouse, stats, true);
}

bool MenuStash::full(int item) {
	return stock.full(item);
}

MenuStash::~MenuStash() {
	SDL_FreeSurface(background);
	delete closeButton;
}

