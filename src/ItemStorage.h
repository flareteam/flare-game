/**
 * class ItemStorage
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef ITEM_STORAGE_H
#define ITEM_STORAGE_H

#include "SDL.h"
#include "ItemDatabase.h"

using namespace std;

class ItemStorage {
protected:
	ItemDatabase *items;
	int slot_number;

public:
	void init(int _slot_number, ItemDatabase *_items);
	~ItemStorage();

	ItemStack & operator [] (int slot);

	void add(ItemStack stack, int slot = -1);
	void substract(int slot, int quantity = 1);
	bool remove(int item);
	void sort();
	void clear();

	bool full();
	int count(int item);
	bool contain(int item);

	ItemStack * storage;
};

#endif

