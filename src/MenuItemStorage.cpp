/**
 * class MenuItemStorage
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuItemStorage.h"

void MenuItemStorage::init(int _slot_number, ItemDatabase *_items, SDL_Surface *_screen, FontEngine *_font, SDL_Rect _area, int _icon_size, int _nb_cols) {
	ItemStorage::init( _slot_number, _items);
	screen = _screen;
	font = _font;
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

