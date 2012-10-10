/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller

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
 * class LootManager
 *
 * Handles floor loot
 */

#ifndef LOOT_MANAGER_H
#define LOOT_MANAGER_H

#include "Animation.h"

#include "ItemManager.h"
#include "Settings.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>

class EnemyManager;
class MapRenderer;
class MenuInventory;
class WidgetTooltip;

struct LootDef {
	ItemStack stack;
	Point pos;
	Animation *animation;
	int currency;
	TooltipData tip;

	LootDef() {
		stack.item = 0;
		stack.quantity = 0;
		pos.x = 0;
		pos.y = 0;
		animation = NULL;
		currency = 0;
		tip.clear();
	}

	LootDef(const LootDef &other) {
		stack.item = other.stack.item;
		stack.quantity = other.stack.quantity;
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		animation = new Animation(*other.animation);
		animation->syncTo(other.animation);
		currency = other.currency;
		tip = other.tip;
	}

	// The assignment operator mainly used in internal vector managing,
	// e.g. in vector::erase()
	LootDef& operator= (const LootDef &other) {
	
		delete animation;	
		animation = new Animation(*other.animation);
		animation->syncTo(other.animation);
		
		stack.item = other.stack.item;
		stack.quantity = other.stack.quantity;
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		currency = other.currency;
		tip = other.tip;
		
		return *this;		
	}

	~LootDef() {
		delete animation;
	}
};

struct CurrencyRange {
	std::string filename;
	int low;
	int high;
};


// this means that normal items are 10x more common than epic items
// these numbers have to be balanced by various factors
const int RARITY_LOW = 7;
const int RARITY_NORMAL = 10;
const int RARITY_HIGH = 3;
const int RARITY_EPIC = 1;

// how close (map units) does the hero have to be to pick up loot?
const int LOOT_RANGE = 3 * UNITS_PER_TILE;

class LootManager {
private:

	ItemManager *items;
	WidgetTooltip *tip;
	EnemyManager *enemies;
	MapRenderer *map;
	StatBlock *hero;

	// functions
	void loadGraphics();
	void calcTables();
	int lootLevel(int base_level);

	Mix_Chunk *loot_flip;

	// loot refers to ItemManager indices
	std::vector<LootDef> loot;

	// loot tables multiplied out
	// currently loot can range from levels 0-20
	int loot_table[21][1024]; // level, number.  the int is an item id
	int loot_table_count[21]; // total number per level

	SDL_Rect animation_pos;
	Point animation_offset;
	std::vector<CurrencyRange> currency_range;

	// enemies which should drop loot, but didnt yet.
	std::vector<const Enemy*> enemiesDroppingLoot;

public:
	static LootManager *getInstance();
	LootManager(ItemManager *_items, MapRenderer *_map, StatBlock *_hero);
	~LootManager();

	void handleNewMap();
	void logic();
	void renderTooltips(Point cam);
	void checkEnemiesForLoot();

	// called by enemy, who definitly wants to drop loot.
	void addEnemyLoot(const Enemy *e);
	void checkMapForLoot();
	bool isFlying(const LootDef &ld);
	void determineLoot(int base_level, Point pos); // uniformly distributed within the base_level set, randomly chosen
	void determineLootByClass(const Enemy *e, Point pos); // distributed according to enemies loot type probabilities, only from specific item class
	int randomItem(int base_level);
	void addLoot(ItemStack stack, Point pos);
	void addCurrency(int count, Point pos);
	void removeLoot(int index);
	ItemStack checkPickup(Point mouse, Point cam, Point hero_pos, int &currency, MenuInventory *inv);
	ItemStack checkAutoPickup(Point hero_pos, int &currency);

	void addRenders(std::vector<Renderable> &ren, std::vector<Renderable> &ren_dead);

	int tooltip_margin; // pixels between loot drop center and label
	bool full_msg;
};

extern LootManager *lootManager;

#endif
