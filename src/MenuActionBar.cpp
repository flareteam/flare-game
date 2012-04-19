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
 * class MenuActionBar
 *
 * Handles the config, display, and usage of the 0-9 hotkeys, mouse buttons, and menu calls
 */

#include "MenuActionBar.h"
#include "SharedResources.h"
#include "WidgetLabel.h"

#include <string>
#include <sstream>

using namespace std;


MenuActionBar::MenuActionBar(PowerManager *_powers, StatBlock *_hero, SDL_Surface *_icons) {
	powers = _powers;
	hero = _hero;
	icons = _icons;

	src.x = 0;
	src.y = 0;
	src.w = 32;
	src.h = 32;
	label_src.x = 0;
	label_src.y = 0;
	label_src.w = 640;
	label_src.h = 10;
	drag_prev_slot = -1;

	clear();

	// TEMP: set action bar positions
	// TODO: define in a config file so that the menu is customizable
	int offset_x = (VIEW_W - 640)/2;
	int offset_y = VIEW_H-32;

	for (int i=0; i<12; i++) {
		slots[i].w = slots[i].h = 32;
		slots[i].y = VIEW_H-32;
		slots[i].x = offset_x + i*32 + 32;
	}
	slots[10].x += 32;
	slots[11].x += 32;

	// menu button positions
	for (int i=0; i<4; i++) {
		menus[i].w = menus[i].h = 32;
		menus[i].y = VIEW_H-32;
		menus[i].x = offset_x + 480 + i*32;
	}

	// screen areas occupied by the three main sections
	numberArea.h = mouseArea.h = menuArea.h = 32;
	numberArea.y = mouseArea.y = menuArea.y = offset_y;
	numberArea.x = offset_x+32;
	numberArea.w = 320;
	mouseArea.x = offset_x+384;
	mouseArea.w = 64;
	menuArea.x = offset_x+480;
	menuArea.w = 128;

	loadGraphics();
}

void MenuActionBar::clear() {
	// clear action bar
	for (int i=0; i<12; i++) {
		hotkeys[i] = -1;
		slot_item_count[i] = -1;
		slot_enabled[i] = true;
	}

    // clear menu notifications
    for (int i=0; i<4; i++)
        requires_attention[i] = false;

	// default: LMB set to basic melee attack
	hotkeys[10] = 1;
}

void MenuActionBar::loadGraphics() {

	emptyslot = IMG_Load(mods->locate("images/menus/slot_empty.png").c_str());
	background = IMG_Load(mods->locate("images/menus/actionbar_trim.png").c_str());
	labels = IMG_Load(mods->locate("images/menus/actionbar_labels.png").c_str());
	disabled = IMG_Load(mods->locate("images/menus/disabled.png").c_str());
	attention = IMG_Load(mods->locate("images/menus/attention_glow.png").c_str());
	if(!emptyslot || !background || !labels || !disabled) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = emptyslot;
	emptyslot = SDL_DisplayFormatAlpha(emptyslot);
	SDL_FreeSurface(cleanup);

	cleanup = labels;
	labels = SDL_DisplayFormatAlpha(labels);
	SDL_FreeSurface(cleanup);

	cleanup = disabled;
	disabled = SDL_DisplayFormatAlpha(disabled);
	SDL_FreeSurface(cleanup);

	cleanup = attention;
	attention = SDL_DisplayFormatAlpha(attention);
	SDL_FreeSurface(cleanup);
}

/**
 * generic render 32-pixel icon
 */
void MenuActionBar::renderIcon(int icon_id, int x, int y) {
	SDL_Rect icon_src;
	SDL_Rect icon_dest;

	icon_dest.x = x;
	icon_dest.y = y;
	icon_src.w = icon_src.h = icon_dest.w = icon_dest.h = 32;
	icon_src.x = (icon_id % 16) * 32;
	icon_src.y = (icon_id / 16) * 32;
	SDL_BlitSurface(icons, &icon_src, screen, &icon_dest);
}

// Renders the "needs attention" icon over the appropriate log menu
void MenuActionBar::renderAttention(int menu_id) {
	SDL_Rect dest;

    // x-value is 12 hotkeys and 4 empty slots over
	dest.x = (VIEW_W - 640)/2 + (menu_id * 32) + 32*15;
	dest.y = VIEW_H-32;
    dest.w = dest.h = 32;
	SDL_BlitSurface(attention, NULL, screen, &dest);
}

void MenuActionBar::logic() {
}



void MenuActionBar::render() {

	SDL_Rect dest;
	SDL_Rect trimsrc;

	int offset_x = (VIEW_W - 640)/2;

	dest.x = offset_x;
	dest.y = VIEW_H-35;
	dest.w = 640;
	dest.h = 35;
	trimsrc.x = 0;
	trimsrc.y = 0;
	trimsrc.w = 640;
	trimsrc.h = 35;

	SDL_BlitSurface(background, &trimsrc, screen, &dest);

	// draw hotkeyed icons
	src.x = src.y = 0;
	src.w = src.h = dest.w = dest.h = 32;
	dest.y = VIEW_H-32;
	for (int i=0; i<12; i++) {

		if (i<=9)
			dest.x = offset_x + (i * 32) + 32;
		else
			dest.x = offset_x + (i * 32) + 64;

		if (hotkeys[i] != -1) {
			slot_enabled[i] = (hero->hero_cooldown[hotkeys[i]] == 0) && (slot_item_count[i] != 0); //see if the slot should be greyed out
			renderIcon(powers->powers[hotkeys[i]].icon, dest.x, dest.y);
		}
		else {
			SDL_BlitSurface(emptyslot, &src, screen, &dest);
		}
	}
	
	renderCooldowns();
	renderItemCounts();

    // render log attention notifications
    for (int i=0; i<4; i++)
        if (requires_attention[i])
            renderAttention(i);

	// draw hotkey labels
	// TODO: keybindings
	dest.x = offset_x;
	dest.y = VIEW_H-10;
	dest.w = 640;
	dest.h = 10;
	SDL_BlitSurface(labels, &label_src, screen, &dest);

}

/**
 * Display a notification for any power on cooldown
 * Also displays disabled powers
 */
void MenuActionBar::renderCooldowns() {

	SDL_Rect item_src;
	SDL_Rect item_dest;

	for (int i=0; i<12; i++) {
		if (!slot_enabled[i]) {

			item_src.x = 0;
			item_src.y = 0;
			item_src.h = 32;
			item_src.w = 32;

			// Wipe from bottom to top
			if (hero->hero_cooldown[hotkeys[i]]) {
				item_src.h = 32 * (hero->hero_cooldown[hotkeys[i]] / (float)powers->powers[hotkeys[i]].cooldown);
			}

			// SDL_BlitSurface will write to these Rects, so make a copy
			item_dest.x = slots[i].x;
			item_dest.y = slots[i].y;
			item_dest.w = slots[i].w;
			item_dest.h = slots[i].h;

			SDL_BlitSurface(disabled, &item_src, screen, &item_dest);
		}
	}
}

/**
 * For powers that have consumables, display the number of consumables remaining
 */
void MenuActionBar::renderItemCounts() {

	stringstream ss;

	for (int i=0; i<12; i++) {
		if (slot_item_count[i] > -1) {
			ss.str("");
			ss << slot_item_count[i];

			WidgetLabel label;
			label.set(slots[i].x, slots[i].y, JUSTIFY_LEFT, VALIGN_TOP, ss.str(), FONT_WHITE);
			label.render();
		}
	}
}

/**
 * On mouseover, show tooltip for buttons
 */
TooltipData MenuActionBar::checkTooltip(Point mouse) {
	TooltipData tip;

	//int offset_x = (VIEW_W - 640)/2;
	if (isWithin(menus[0], mouse)) {
		tip.lines[tip.num_lines++] = msg->get("Character Menu (C)");
		return tip;
	}
	if (isWithin(menus[1], mouse)) {
		tip.lines[tip.num_lines++] = msg->get("Inventory Menu (I)");
		return tip;
	}
	if (isWithin(menus[2], mouse)) {
		tip.lines[tip.num_lines++] = msg->get("Power Menu (P)");
		return tip;
	}
	if (isWithin(menus[3], mouse)) {
		tip.lines[tip.num_lines++] = msg->get("Log Menu (L)");
		return tip;
	}
	for (int i=0; i<12; i++) {
		if (hotkeys[i] != -1) {
			if (isWithin(slots[i], mouse)) {
				tip.lines[tip.num_lines++] = powers->powers[hotkeys[i]].name;
			}
		}
	}

	return tip;
}

/**
 * After dragging a power or item onto the action bar, set as new hotkey
 */
void MenuActionBar::drop(Point mouse, int power_index, bool rearranging) {
	for (int i=0; i<12; i++) {
		if (isWithin(slots[i], mouse)) {
			if (rearranging) {
				hotkeys[drag_prev_slot] = hotkeys[i];
			}
			hotkeys[i] = power_index;
			return;
		}
	}
}

/**
 * CTRL-click a hotkey to clear it
 */
void MenuActionBar::remove(Point mouse) {
	for (int i=0; i<12; i++) {
		if (isWithin(slots[i], mouse)) {
			hotkeys[i] = -1;
			return;
		}
	}
}

/**
 * If pressing an action key (keyboard or mouseclick) and the power is enabled,
 * return that power's ID.
 */
int MenuActionBar::checkAction(Point mouse) {

	// check click action
	if ((inp->pressing[MAIN1] && !inp->lock[MAIN1]) || (inp->pressing[MAIN2] && !inp->lock[MAIN2])) {
		for (int i=0; i<12; i++) {
			if (isWithin(slots[i], mouse) && slot_enabled[i]) {

				return hotkeys[i];
			}
		}
	}

	// check hotkey action
	if (inp->pressing[BAR_1] && slot_enabled[0]) return hotkeys[0];
	if (inp->pressing[BAR_2] && slot_enabled[1]) return hotkeys[1];
	if (inp->pressing[BAR_3] && slot_enabled[2]) return hotkeys[2];
	if (inp->pressing[BAR_4] && slot_enabled[3]) return hotkeys[3];
	if (inp->pressing[BAR_5] && slot_enabled[4]) return hotkeys[4];
	if (inp->pressing[BAR_6] && slot_enabled[5]) return hotkeys[5];
	if (inp->pressing[BAR_7] && slot_enabled[6]) return hotkeys[6];
	if (inp->pressing[BAR_8] && slot_enabled[7]) return hotkeys[7];
	if (inp->pressing[BAR_9] && slot_enabled[8]) return hotkeys[8];
	if (inp->pressing[BAR_0] && slot_enabled[9]) return hotkeys[9];
	if (inp->pressing[MAIN1] && slot_enabled[10] && !inp->lock[MAIN1]) {
		return hotkeys[10];
	}
	if (inp->pressing[MAIN2] && slot_enabled[11] && !inp->lock[MAIN2]) {
		return hotkeys[11];
	}
	return -1;
}

/**
 * If clicking while a menu is open, assume the player wants to rearrange the action bar
 */
int MenuActionBar::checkDrag(Point mouse) {
	int power_index;

	for (int i=0; i<12; i++) {
		if (isWithin(slots[i], mouse)) {
			drag_prev_slot = i;
			power_index = hotkeys[i];
			hotkeys[i] = -1;
			return power_index;
		}
	}

	return -1;
 }

/**
 * if clicking a menu, act as if the player pressed that menu's hotkey
 */
void MenuActionBar::checkMenu(Point mouse, bool &menu_c, bool &menu_i, bool &menu_p, bool &menu_l) {
	if ((inp->pressing[MAIN1] && !inp->lock[MAIN1]) || (inp->pressing[MAIN2] && !inp->lock[MAIN2])) {
		if (isWithin(menus[MENU_CHARACTER], mouse)) {
			if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) inp->lock[MAIN1] = true;
			else inp->lock[MAIN2] = true;
			menu_c = true;
		}
		else if (isWithin(menus[MENU_INVENTORY], mouse)) {
			if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) inp->lock[MAIN1] = true;
			else inp->lock[MAIN2] = true;
			menu_i = true;
		}
		else if (isWithin(menus[MENU_POWERS], mouse)) {
			if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) inp->lock[MAIN1] = true;
			else inp->lock[MAIN2] = true;
			inp->lock[MAIN1] = true;
			menu_p = true;
		}
		else if (isWithin(menus[MENU_LOG], mouse)) {
			if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) inp->lock[MAIN1] = true;
			else inp->lock[MAIN2] = true;
			inp->lock[MAIN1] = true;
			menu_l = true;
		}
	}
}

/**
 * Set all hotkeys at once e.g. when loading a game
 */
void MenuActionBar::set(int power_id[12]) {
	for (int i=0; i<12; i++)
		hotkeys[i] = power_id[i];
}

MenuActionBar::~MenuActionBar() {
	SDL_FreeSurface(emptyslot);
	SDL_FreeSurface(background);
	SDL_FreeSurface(labels);
	SDL_FreeSurface(disabled);
}
