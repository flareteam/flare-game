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
 * class ItemStorage
 */

#include "ItemStorage.h"

void ItemStorage::init(int _slot_number, ItemManager *_items) {
	slot_number = _slot_number;
	items = _items;

	storage = new ItemStack[slot_number];

	for( int i=0; i<slot_number; i++) {
		storage[i].item = 0;
		storage[i].quantity = 0;
	}
}

ItemStack & ItemStorage::operator [] (int slot) {
	return storage[slot];
}

/**
 * Take the savefile CSV list of items id and convert to storage array
 */
void ItemStorage::setItems(string s) {
	s = s + ',';
	for (int i=0; i<slot_number; i++) {
		storage[i].item = eatFirstInt(s, ',');
		if( storage[i].item != 0) storage[i].quantity = 1;
		else storage[i].quantity = 0;
	}
}

/**
 * Take the savefile CSV list of items quantities and convert to storage array
 */
void ItemStorage::setQuantities(string s) {
	s = s + ',';
	for (int i=0; i<slot_number; i++) {
		storage[i].quantity = eatFirstInt(s, ',');
	}
}

/**
 * Convert storage array to a CSV list of items id for savefile
 */
string ItemStorage::getItems() {
	stringstream ss;
	ss.str("");
	for (int i=0; i<slot_number; i++) {
		ss << storage[i].item;
		if (i < slot_number-1) ss << ',';
	}
	return ss.str();
}

/**
 * Convert storage array to a CSV list of items quantities for savefile
 */
string ItemStorage::getQuantities() {
	stringstream ss;
	ss.str("");
	for (int i=0; i<slot_number; i++) {
		ss << storage[i].quantity;
		if (i < slot_number-1) ss << ',';
	}
	return ss.str();
}

void ItemStorage::clear() {
	for( int i=0; i<slot_number; i++) {
		storage[i].item = 0;
		storage[i].quantity = 0;
	}
}

/**
 * Insert item into first available carried slot, preferably in the optionnal specified slot
 *
 * @param ItemStack Stack of items
 * @param slot Slot number where it will try to store the item
 */
void ItemStorage::add( ItemStack stack, int slot) {
	int max_quantity;
	int quantity_added;
	int i;

	if( stack.item != 0) {
		max_quantity = items->items[stack.item].max_quantity;
		if( slot > -1) {
			// a slot is specified
			if( storage[slot].item != 0 && storage[slot].item != stack.item) {
				// the proposed slot isn't available
				slot = -1;
			}
		} else {
			// first search of stack to complete if the item is stackable
			i = 0;
			while( max_quantity > 1 && slot == -1 && i < slot_number) {
				if (storage[i].item == stack.item && storage[i].quantity < max_quantity) {
					slot = i;
				}
				i++;
			}
			// then an empty slot
			i = 0;
			while( slot == -1 && i < slot_number) {
				if (storage[i].item == 0) {
					slot = i;
				}
				i++;
			}
		}
		if( slot != -1) {
			// Add
			quantity_added = min( stack.quantity, max_quantity - storage[slot].quantity);
			storage[slot].item = stack.item;
			storage[slot].quantity += quantity_added;
			stack.quantity -= quantity_added;
			// Add back the remaining
			if( stack.quantity > 0) {
				add( stack);
			}
		}
		else {
			// No available slot, drop
			// TODO: We should drop on the floor an item we can't store
		}
	}
}

/**
 * Substract an item from the specified slot, or remove it if it's the last
 *
 * @param slot Slot number
 */
void ItemStorage::substract(int slot, int quantity) {
	storage[slot].quantity -= quantity;
	if (storage[slot].quantity <= 0) {
		storage[slot].item = 0;
	}
}

/**
 * Remove one given item
 */
bool ItemStorage::remove(int item) {
	for (int i=0; i<slot_number; i++) {
		if (storage[i].item == item) {
			substract(i, 1);
			return true;
		}
	}
	return false;
}

void ItemStorage::sort() {
	bubbleSort(storage, slot_number);
}

//TODO: handle stackable items
bool ItemStorage::full() {
	for (int i=0; i<slot_number; i++) {
		if (storage[i].item == 0) {
			return false;
		}
	}
	return true;
}
 
/**
 * Get the number of the specified item carried (not equipped)
 */
int ItemStorage::count(int item) {
	int count=0;
	for (int i=0; i<slot_number; i++) {
		if (storage[i].item == item) {
			count += storage[i].quantity;
		}
	}
	return count;
}

/**
 * Check to see if the given item is equipped
 */
bool ItemStorage::contain(int item) {
	for (int i=0; i<slot_number; i++) {
		if (storage[i].item == item)
			return true;
	}
	return false;
}

ItemStorage::~ItemStorage() {
	delete[] storage;
}

