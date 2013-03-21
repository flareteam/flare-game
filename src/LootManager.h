/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller
Copyright © 2013 Henrik Andersson

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


#pragma once
#ifndef LOOT_MANAGER_H
#define LOOT_MANAGER_H

#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"

#include "Loot.h"
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

class CurrencyRange {
public:
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
	MapRenderer *map;
	StatBlock *hero;

	// functions
	void loadGraphics();

	SoundManager::SoundID sfx_currency;
	SoundManager::SoundID sfx_loot;

	// loot refers to ItemManager indices
	std::vector<Loot> loot;

	SDL_Rect animation_pos;
	Point animation_offset;
	std::vector<CurrencyRange> currency_range;

	// enemies which should drop loot, but didnt yet.
	std::vector<const class Enemy*> enemiesDroppingLoot;

public:
	static LootManager *getInstance();
	LootManager(ItemManager *_items, MapRenderer *_map, StatBlock *_hero);
	LootManager(const LootManager &copy); // not implemented
	~LootManager();

	void handleNewMap();
	void logic();
	void renderTooltips(Point cam);
	void checkEnemiesForLoot();

	void playCurrencySound(Point loot_pos = Point(0,0));

	// called by enemy, who definitly wants to drop loot.
	void addEnemyLoot(const Enemy *e);
	void checkMapForLoot();
	void determineLootByEnemy(const Enemy *e, Point pos); // pick from enemy-specific loot table
	void addLoot(ItemStack stack, Point pos);
	void addCurrency(int count, Point pos);
	ItemStack checkPickup(Point mouse, Point cam, Point hero_pos, int &currency, MenuInventory *inv);
	ItemStack checkAutoPickup(Point hero_pos, int &currency);
	ItemStack checkNearestPickup(Point hero_pos, int &currency, MenuInventory *inv);

	void addRenders(std::vector<Renderable> &ren, std::vector<Renderable> &ren_dead);

	int tooltip_margin; // pixels between loot drop center and label
	bool full_msg;
};

extern LootManager *lootManager;

#endif
