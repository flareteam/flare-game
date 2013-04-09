/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk

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

#include "FileParser.h"
#include "Menu.h"
#include "MenuActionBar.h"
#include "PowerManager.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetLabel.h"
#include "WidgetTooltip.h"

#include <string>
#include <sstream>
#include <climits>

using namespace std;


MenuActionBar::MenuActionBar(PowerManager *_powers, StatBlock *_hero, SDL_Surface *_icons) {
	powers = _powers;
	hero = _hero;
	icons = _icons;

	src.x = 0;
	src.y = 0;
	src.w = ICON_SIZE;
	src.h = ICON_SIZE;
	drag_prev_slot = -1;
	last_mouse.x = 0;
	last_mouse.y = 0;

	clear();

	for (unsigned int i=0;i<16;i++) {
		labels[i] = new WidgetLabel();
	}

	loadGraphics();

}

void MenuActionBar::update() {

	// Read data from config file
	FileParser infile;

	if (infile.open(mods->locate("menus/actionbar.txt"))) {
	  while (infile.next()) {
		infile.val = infile.val + ',';

		if (infile.key == "slot1") {
			slots[0].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[0].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[0].w = eatFirstInt(infile.val, ',');
			slots[0].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot2") {
			slots[1].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[1].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[1].w = eatFirstInt(infile.val, ',');
			slots[1].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot3") {
			slots[2].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[2].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[2].w = eatFirstInt(infile.val, ',');
			slots[2].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot4") {
			slots[3].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[3].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[3].w = eatFirstInt(infile.val, ',');
			slots[3].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot5") {
			slots[4].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[4].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[4].w = eatFirstInt(infile.val, ',');
			slots[4].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot6") {
			slots[5].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[5].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[5].w = eatFirstInt(infile.val, ',');
			slots[5].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot7") {
			slots[6].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[6].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[6].w = eatFirstInt(infile.val, ',');
			slots[6].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot8") {
			slots[7].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[7].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[7].w = eatFirstInt(infile.val, ',');
			slots[7].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot9") {
			slots[8].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[8].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[8].w = eatFirstInt(infile.val, ',');
			slots[8].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot10") {
			slots[9].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[9].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[9].w = eatFirstInt(infile.val, ',');
			slots[9].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot_M1") {
			slots[10].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[10].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[10].w = eatFirstInt(infile.val, ',');
			slots[10].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "slot_M2") {
			slots[11].x = window_area.x+eatFirstInt(infile.val, ',');
			slots[11].y = window_area.y+eatFirstInt(infile.val, ',');
			slots[11].w = eatFirstInt(infile.val, ',');
			slots[11].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "char_menu") {
			menus[0].x = window_area.x+eatFirstInt(infile.val, ',');
			menus[0].y = window_area.y+eatFirstInt(infile.val, ',');
			menus[0].w = eatFirstInt(infile.val, ',');
			menus[0].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "inv_menu") {
			menus[1].x = window_area.x+eatFirstInt(infile.val, ',');
			menus[1].y = window_area.y+eatFirstInt(infile.val, ',');
			menus[1].w = eatFirstInt(infile.val, ',');
			menus[1].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "powers_menu") {
			menus[2].x = window_area.x+eatFirstInt(infile.val, ',');
			menus[2].y = window_area.y+eatFirstInt(infile.val, ',');
			menus[2].w = eatFirstInt(infile.val, ',');
			menus[2].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "log_menu") {
			menus[3].x = window_area.x+eatFirstInt(infile.val, ',');
			menus[3].y = window_area.y+eatFirstInt(infile.val, ',');
			menus[3].w = eatFirstInt(infile.val, ',');
			menus[3].h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "numberArea") {
			numberArea.x = window_area.x+eatFirstInt(infile.val, ',');
			numberArea.w = eatFirstInt(infile.val, ',');
			numberArea.h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "mouseArea") {
			mouseArea.x = window_area.x+eatFirstInt(infile.val, ',');
			mouseArea.w = eatFirstInt(infile.val, ',');
			mouseArea.h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "menuArea") {
			menuArea.x = window_area.x+eatFirstInt(infile.val, ',');
			menuArea.w = eatFirstInt(infile.val, ',');
			menuArea.h = eatFirstInt(infile.val, ',');
		}

	  }
	  infile.close();
	} else fprintf(stderr, "Unable to open menus/actionbar.txt!\n");

	// screen areas occupied by the three main sections
	numberArea.y = mouseArea.y = menuArea.y = window_area.y;

	// set keybinding labels
	for (unsigned int i=0; i<10; i++) {
		if (inpt->binding[i+6] < 8)
			labels[i]->set(slots[i].x+slots[i].w, slots[i].y+slots[i].h-12, JUSTIFY_RIGHT, VALIGN_TOP, inpt->mouse_button[inpt->binding[i+6]-1], font->getColor("menu_normal"));
		else
			labels[i]->set(slots[i].x+slots[i].w, slots[i].y+slots[i].h-12, JUSTIFY_RIGHT, VALIGN_TOP, SDL_GetKeyName((SDLKey)inpt->binding[i+6]), font->getColor("menu_normal"));
	}
	for (unsigned int i=0; i<2; i++) {
		if (inpt->binding[i+20] < 8)
			labels[i+10]->set(slots[i+10].x+slots[i+10].w, slots[i+10].y+slots[i+10].h-12, JUSTIFY_RIGHT, VALIGN_TOP, inpt->mouse_button[inpt->binding[i+20]-1], font->getColor("menu_normal"));
		else
			labels[i+10]->set(slots[i+10].x+slots[i+10].w, slots[i+10].y+slots[i+10].h-12, JUSTIFY_RIGHT, VALIGN_TOP, SDL_GetKeyName((SDLKey)inpt->binding[i+20]), font->getColor("menu_normal"));
	}
	for (unsigned int i=0; i<4; i++) {
		if (inpt->binding[i+16] < 8)
			labels[i+12]->set(menus[i].x+menus[i].w, menus[i].y+menus[i].h-12, JUSTIFY_RIGHT, VALIGN_TOP, inpt->mouse_button[inpt->binding[i+16]-1], font->getColor("menu_normal"));
		else
			labels[i+12]->set(menus[i].x+menus[i].w, menus[i].y+menus[i].h-12, JUSTIFY_RIGHT, VALIGN_TOP, SDL_GetKeyName((SDLKey)inpt->binding[i+16]), font->getColor("menu_normal"));
	}
}

void MenuActionBar::clear() {
	// clear action bar
	for (int i=0; i<12; i++) {
		hotkeys[i] = 0;
		actionbar[i] = 0;
		slot_item_count[i] = -1;
		slot_enabled[i] = true;
		locked[i] = false;
	}

    // clear menu notifications
    for (int i=0; i<4; i++)
        requires_attention[i] = false;

}

void MenuActionBar::loadGraphics() {

	emptyslot = IMG_Load(mods->locate("images/menus/slot_empty.png").c_str());
	background = IMG_Load(mods->locate("images/menus/actionbar_trim.png").c_str());
	disabled = IMG_Load(mods->locate("images/menus/disabled.png").c_str());
	attention = IMG_Load(mods->locate("images/menus/attention_glow.png").c_str());
	if(!emptyslot || !background || !disabled || !attention) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
	}

	// optimize
	SDL_Surface *cleanup;

	if (background) {
		cleanup = background;
		background = SDL_DisplayFormatAlpha(background);
		SDL_FreeSurface(cleanup);
	}

	if (emptyslot) {
		cleanup = emptyslot;
		emptyslot = SDL_DisplayFormatAlpha(emptyslot);
		SDL_FreeSurface(cleanup);
	}

	if (disabled) {
		cleanup = disabled;
		disabled = SDL_DisplayFormatAlpha(disabled);
		SDL_FreeSurface(cleanup);
	}

	if (attention) {
		cleanup = attention;
		attention = SDL_DisplayFormatAlpha(attention);
		SDL_FreeSurface(cleanup);
	}
}

/**
 * generic render small icon
 */
void MenuActionBar::renderIcon(int icon_id, int x, int y) {
	SDL_Rect icon_src;
	SDL_Rect icon_dest;

	icon_dest.x = x;
	icon_dest.y = y;
	icon_src.w = icon_src.h = icon_dest.w = icon_dest.h = ICON_SIZE;

	int columns = icons->w / ICON_SIZE;
	icon_src.x = (icon_id % columns) * ICON_SIZE;
	icon_src.y = (icon_id / columns) * ICON_SIZE;
	SDL_BlitSurface(icons, &icon_src, screen, &icon_dest);
}

// Renders the "needs attention" icon over the appropriate log menu
void MenuActionBar::renderAttention(int menu_id) {
	SDL_Rect dest;

    // x-value is 12 hotkeys and 4 empty slots over
	dest.x = window_area.x + (menu_id * ICON_SIZE) + ICON_SIZE*15;
	dest.y = window_area.y+3;
    dest.w = dest.h = ICON_SIZE;
	SDL_BlitSurface(attention, NULL, screen, &dest);
}

void MenuActionBar::logic() {
}



void MenuActionBar::render() {

	SDL_Rect dest;
	SDL_Rect trimsrc;

	dest = window_area;
	trimsrc.x = 0;
	trimsrc.y = 0;
	trimsrc.w = window_area.w;
	trimsrc.h = window_area.h;

	SDL_BlitSurface(background, &trimsrc, screen, &dest);

	// draw hotkeyed icons
	src.x = src.y = 0;
	src.w = src.h = dest.w = dest.h = ICON_SIZE;
	dest.y = window_area.y+3;
	for (int i=0; i<12; i++) {

		if (i<=9)
			dest.x = window_area.x + (i * ICON_SIZE) + ICON_SIZE;
		else
			dest.x = window_area.x + (i * ICON_SIZE) + ICON_SIZE* 2;

		if (hotkeys[i] != 0) {
			const Power &power = powers->getPower(hotkeys[i]);
			slot_enabled[i] = (hero->hero_cooldown[hotkeys[i]] == 0)
						   && (slot_item_count[i] != 0)
						   && !hero->effects.stun
						   && hero->alive
						   && hero->canUsePower(power, hotkeys[i]); //see if the slot should be greyed out
			unsigned icon_offset = 0;/* !slot_enabled[i] ? ICON_DISABLED_OFFSET :
								   (hero->activated_powerslot == i ? ICON_HIGHLIGHT_OFFSET : 0); */
			renderIcon(power.icon + icon_offset, dest.x, dest.y);
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
	for (int i=0; i<16;i++) {
		labels[i]->render();
	}

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
			item_src.h = ICON_SIZE;
			item_src.w = ICON_SIZE;

			// Wipe from bottom to top
			if (hero->hero_cooldown[hotkeys[i]]) {
				item_src.h = (ICON_SIZE * hero->hero_cooldown[hotkeys[i]]) / powers->powers[hotkeys[i]].cooldown;
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
			label.set(slots[i].x, slots[i].y, JUSTIFY_LEFT, VALIGN_TOP, ss.str(), font->getColor("menu_normal"));
			label.render();
		}
	}
}

/**
 * On mouseover, show tooltip for buttons
 */
TooltipData MenuActionBar::checkTooltip(Point mouse) {
	TooltipData tip;

	if (isWithin(menus[0], mouse)) {
		tip.addText(msg->get("Character"));
		return tip;
	}
	if (isWithin(menus[1], mouse)) {
		tip.addText(msg->get("Inventory"));
		return tip;
	}
	if (isWithin(menus[2], mouse)) {
		tip.addText(msg->get("Powers"));
		return tip;
	}
	if (isWithin(menus[3], mouse)) {
		tip.addText(msg->get("Log"));
		return tip;
	}
	for (int i=0; i<12; i++) {
		if (hotkeys[i] != 0) {
			if (isWithin(slots[i], mouse)) {
				tip.addText(powers->powers[hotkeys[i]].name);
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
				if ((locked[i] && !locked[drag_prev_slot]) || (!locked[i] && locked[drag_prev_slot])) {
					locked[i] = !locked[i];
					locked[drag_prev_slot] = !locked[drag_prev_slot];
				}
				hotkeys[drag_prev_slot] = hotkeys[i];
			} else if (locked[i]) return;
			hotkeys[i] = power_index;
			return;
		}
	}
}

/**
 * Return the power to the last clicked on slot
 */
void MenuActionBar::actionReturn(int power_index) {
	drop(last_mouse, power_index, 0);
}

/**
 * CTRL-click a hotkey to clear it
 */
void MenuActionBar::remove(Point mouse) {
	for (int i=0; i<12; i++) {
		if (isWithin(slots[i], mouse)) {
			if (locked[i]) return;
			hotkeys[i] = 0;
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
	if ((inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) || (inpt->pressing[MAIN2] && !inpt->lock[MAIN2])) {
		for (int i=0; i<12; i++) {
			if (isWithin(slots[i], mouse) && slot_enabled[i]) {

				return hotkeys[i];
			}
		}
	}

	// check hotkey action
	if (inpt->pressing[BAR_1] && slot_enabled[0]) return hotkeys[0];
	if (inpt->pressing[BAR_2] && slot_enabled[1]) return hotkeys[1];
	if (inpt->pressing[BAR_3] && slot_enabled[2]) return hotkeys[2];
	if (inpt->pressing[BAR_4] && slot_enabled[3]) return hotkeys[3];
	if (inpt->pressing[BAR_5] && slot_enabled[4]) return hotkeys[4];
	if (inpt->pressing[BAR_6] && slot_enabled[5]) return hotkeys[5];
	if (inpt->pressing[BAR_7] && slot_enabled[6]) return hotkeys[6];
	if (inpt->pressing[BAR_8] && slot_enabled[7]) return hotkeys[7];
	if (inpt->pressing[BAR_9] && slot_enabled[8]) return hotkeys[8];
	if (inpt->pressing[BAR_0] && slot_enabled[9]) return hotkeys[9];
	if (inpt->pressing[MAIN1] && slot_enabled[10] && !inpt->lock[MAIN1]) return hotkeys[10];
	if (inpt->pressing[MAIN2] && slot_enabled[11] && !inpt->lock[MAIN2]) return hotkeys[11];
	return 0;
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
			hotkeys[i] = 0;
			last_mouse = mouse;
			return power_index;
		}
	}

	return 0;
 }

/**
 * if clicking a menu, act as if the player pressed that menu's hotkey
 */
void MenuActionBar::checkMenu(Point mouse, bool &menu_c, bool &menu_i, bool &menu_p, bool &menu_l) {
	if ((inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) || (inpt->pressing[MAIN2] && !inpt->lock[MAIN2])) {
		if (isWithin(menus[MENU_CHARACTER], mouse)) {
			if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) inpt->lock[MAIN1] = true;
			else inpt->lock[MAIN2] = true;
			menu_c = true;
		}
		else if (isWithin(menus[MENU_INVENTORY], mouse)) {
			if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) inpt->lock[MAIN1] = true;
			else inpt->lock[MAIN2] = true;
			menu_i = true;
		}
		else if (isWithin(menus[MENU_POWERS], mouse)) {
			if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) inpt->lock[MAIN1] = true;
			else inpt->lock[MAIN2] = true;
			inpt->lock[MAIN1] = true;
			menu_p = true;
		}
		else if (isWithin(menus[MENU_LOG], mouse)) {
			if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) inpt->lock[MAIN1] = true;
			else inpt->lock[MAIN2] = true;
			inpt->lock[MAIN1] = true;
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
	SDL_FreeSurface(disabled);
	SDL_FreeSurface(attention);

	for (unsigned int i=0; i<16; i++) {
		delete labels[i];
	}
}
