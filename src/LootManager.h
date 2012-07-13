/*
Copyright � 2011-2012 Clint Bellanger

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

#include "ItemManager.h"
#include "Settings.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>

class EnemyManager;
class MapRenderer;
class WidgetTooltip;

struct LootDef {
	ItemStack stack;
	int frame;
	Point pos;
	int gold;
	TooltipData tip;

	void clear() {
		stack.item = 0;
		stack.quantity = 0;
		frame = 0;
		pos.x = 0;
		pos.y = 0;
		gold = 0;
		tip.clear();
	}
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
	void clearLoot(LootDef &ld);


	SDL_Surface *flying_loot[64];
	SDL_Surface *flying_gold[3];

	std::string animation_id[64];
	int animation_count;

	Mix_Chunk *loot_flip;

	Point frame_size;
	int frame_count; // the last frame is the "at-rest" floor loot graphic

	// loot refers to ItemManager indices
	std::vector<LootDef> loot;

	// loot tables multiplied out
	// currently loot can range from levels 0-20
	int loot_table[21][1024]; // level, number.  the int is an item id
	int loot_table_count[21]; // total number per level

	// animation vars
	int anim_loot_frames;
	int anim_loot_duration;

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
	void determineLoot(int base_level, Point pos); // uniformly distributed within the base_level set
	void determineLootWithProbability(const Enemy *e, Point pos); // distributed according to enemies loot type probabilities.
	int randomItem(int base_level);
	void addLoot(ItemStack stack, Point pos);
	void addGold(int count, Point pos);
	void removeLoot(int index);
	ItemStack checkPickup(Point mouse, Point cam, Point hero_pos, int &gold, bool inv_full);
	ItemStack checkAutoPickup(Point cam, Point hero_pos, int &gold, bool inv_full);

	void addRenders(std::vector<Renderable> &renderables);

	int tooltip_margin;
	bool full_msg;
};

extern LootManager *lootManager;

#endif
