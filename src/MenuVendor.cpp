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
 * class MenuVendor
 */

#include "MenuVendor.h"
#include "SharedResources.h"
#include "WidgetLabel.h"

using namespace std;


MenuVendor::MenuVendor(ItemManager *_items, StatBlock *_stats) {
	items = _items;
	stats = _stats;

	int offset_y = (VIEW_H - 416)/2;

	slots_area.x = 32;
	slots_area.y = offset_y + 64;
	slots_area.w = 256;
	slots_area.h = 320;

	stock.init( VENDOR_SLOTS, items, slots_area, ICON_SIZE_32, 8);

	visible = false;
	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));
	closeButton->pos.x = 294;
	closeButton->pos.y = (VIEW_H - 480)/2 + 34;

	loadMerchant("");
}

void MenuVendor::loadGraphics() {
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

void MenuVendor::loadMerchant(const std::string& filename) {
}

void MenuVendor::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}
}

void MenuVendor::render() {
	if (!visible) return;
	SDL_Rect src;
	SDL_Rect dest;

	int offset_y = (VIEW_H - 416)/2;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = 0;
	dest.y = offset_y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);

	// close button
	closeButton->render();

	// text overlay
	WidgetLabel label;
	label.set(160, offset_y+8, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Vendor"), FONT_WHITE);
	label.render();
	label.set(160, offset_y+24, JUSTIFY_CENTER, VALIGN_TOP, npc->name, FONT_WHITE);
	label.render();

	// show stock
	stock.render();
}

/**
 * Start dragging a vendor item
 * Players can drag an item to their inventory to purchase.
 */
ItemStack MenuVendor::click(InputState * input) {
	ItemStack stack = stock.click(input);
	saveInventory();
	return stack;
}

/**
 * Cancel the dragging initiated by the clic()
 */
void MenuVendor::itemReturn(ItemStack stack) {
	stock.itemReturn(stack);
	saveInventory();
}

void MenuVendor::add(ItemStack stack) {
	stock.add(stack);
	saveInventory();
}

TooltipData MenuVendor::checkTooltip(Point mouse) {
	return stock.checkTooltip( mouse, stats, true);
}

bool MenuVendor::full() {
	return stock.full();
}

/**
 * Several NPCs vendors can share this menu.
 * When the player talks to a new NPC, apply that NPC's inventory
 */
void MenuVendor::setInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		stock[i] = npc->stock[i];
	}
}

/**
 * Save changes to the inventory back to the NPC
 * For persistent stock amounts and buyback (at least until
 * the player leaves this map)
 */
void MenuVendor::saveInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		npc->stock[i] = stock[i];
	}

}

MenuVendor::~MenuVendor() {
	SDL_FreeSurface(background);
	delete closeButton;
}

