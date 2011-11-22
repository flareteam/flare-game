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
 * class MenuItemStorage
 */

#include "MenuItemStorage.h"

void MenuItemStorage::init(int _slot_number, ItemManager *_items, SDL_Rect _area, int _icon_size, int _nb_cols) {
	ItemStorage::init( _slot_number, _items);
	area = _area;
	icon_size = _icon_size;
	nb_cols = _nb_cols;
}

void MenuItemStorage::render() {
	for (int i=0; i<slot_number; i++) {
		if (storage[i].item > 0) {
			items->renderIcon(storage[i], area.x + (i % nb_cols * icon_size), area.y + (i / nb_cols * icon_size), icon_size);
		}	
	}
}

int MenuItemStorage::slotOver(Point mouse) {
	if( isWithin( area, mouse)) {
		return (mouse.x - area.x) / icon_size + (mouse.y - area.y) / icon_size * nb_cols;
	}
	else {
		return -1;
	}
}

TooltipData MenuItemStorage::checkTooltip(Point mouse, StatBlock *stats, bool vendor_view) {
	TooltipData tip;
	int slot = slotOver( mouse);

	if (slot > -1 && storage[slot].item > 0) {
		return items->getTooltip( storage[slot].item, stats, vendor_view);
	}
	return tip;
}

ItemStack MenuItemStorage::click(InputState * input) {
	ItemStack item;
	drag_prev_slot = slotOver(input->mouse);
	if( drag_prev_slot > -1) { 
		item = storage[drag_prev_slot];
		if( input->pressing[SHIFT]) {
			item.quantity = 1;
		}
		substract( drag_prev_slot, item.quantity);
		return item;
	}
	else {
		item.item = 0;
		item.quantity = 0;
		return item;
	}
}

void MenuItemStorage::itemReturn(ItemStack stack) {
	add( stack, drag_prev_slot);
	drag_prev_slot = -1;
}

