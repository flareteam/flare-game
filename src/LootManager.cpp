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

#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"

#include "EnemyManager.h"
#include "FileParser.h"
#include "LootManager.h"
#include "Menu.h"
#include "MenuInventory.h"
#include "SharedResources.h"
#include "UtilsParsing.h"

#include <sstream>
#include <iostream>

using namespace std;

LootManager *lootManager = 0;
LootManager *LootManager::getInstance() {
	return lootManager;
}

LootManager::LootManager(ItemManager *_items, MapRenderer *_map, StatBlock *_hero) {
	items = _items;
	map = _map; // we need to be able to read loot that drops from map containers
	hero = _hero; // we need the player's position for dropping loot in a valid spot

	tip = new WidgetTooltip();

	FileParser infile;
	// load loot animation settings from engine config file
	if (infile.open(mods->locate("engine/loot.txt").c_str())) {
		while (infile.next()) {
			infile.val = infile.val + ',';

			if (infile.key == "loot_animation") {
				animation_pos.x = eatFirstInt(infile.val, ',');
				animation_pos.y = eatFirstInt(infile.val, ',');
				animation_pos.w = eatFirstInt(infile.val, ',');
				animation_pos.h = eatFirstInt(infile.val, ',');
			} else if (infile.key == "loot_animation_offset") {
				animation_offset.x = eatFirstInt(infile.val, ',');
				animation_offset.y = eatFirstInt(infile.val, ',');
			} else if (infile.key == "tooltip_margin") {
				tooltip_margin = eatFirstInt(infile.val, ',');
			} else if (infile.key == "autopickup_range") {
				AUTOPICKUP_RANGE = eatFirstInt(infile.val, ',');
			} else if (infile.key == "autopickup_currency") {
				int currency = eatFirstInt(infile.val, ',');
				if (currency == 1)
					AUTOPICKUP_CURRENCY = true;
				else
					AUTOPICKUP_CURRENCY = false;
			} else if (infile.key == "currency_name") {
				CURRENCY = msg->get(eatFirstString(infile.val, ','));
			} else if (infile.key == "vendor_ratio") {
				VENDOR_RATIO = eatFirstInt(infile.val, ',') / 100.0f;
			} else if (infile.key == "currency_range") {
				CurrencyRange cr;
				cr.filename = eatFirstString(infile.val, ',');
				cr.low = eatFirstInt(infile.val, ',');
				cr.high = eatFirstInt(infile.val, ',');
				currency_range.push_back(cr);
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/loot.txt!\n");

	loot_flip = NULL;

	// reset current map loot
	loot.clear();

	loadGraphics();
	if (audio && SOUND_VOLUME)
		loot_flip = Mix_LoadWAV(mods->locate("soundfx/flying_loot.ogg").c_str());
	full_msg = false;

	if (!lootManager)
		lootManager = this;
	else
		exit(25);
	// TODO: make sure only one instance of the lootmanager is created.
}

/**
 * The "loot" variable on each item refers to the "flying loot" animation for that item.
 * Here we load all the animations used by the item database.
 */
void LootManager::loadGraphics() {

	// check all items in the item database
	for (unsigned int i=0; i < items->items.size(); i++) {
		string anim_id = items->items[i].loot_animation;
		if (anim_id == "") continue;

		string animationname = "animations/loot/" + anim_id + ".txt";
		anim->increaseCount(animationname);
	}

	// currency
	for (unsigned int i=0; i<currency_range.size(); i++) {
		string animationname = "animations/loot/" + currency_range[i].filename + ".txt";
		anim->increaseCount(animationname);
	}
}

void LootManager::handleNewMap() {
	loot.clear();
}

void LootManager::logic() {
	vector<Loot>::iterator it;
	for (it = loot.begin(); it != loot.end(); ++it) {

		// animate flying loot
		it->animation->advanceFrame();

		if (it->animation->isSecondLastFrame()) {
			if (it->stack.item > 0)
				items->playSound(it->stack.item);
			else
				items->playCoinsSound();
		}
	}

	checkEnemiesForLoot();
	checkMapForLoot();
}

/**
 * Show all tooltips for loot on the floor
 */
void LootManager::renderTooltips(Point cam) {
	Point dest;
	stringstream ss;

	vector<Loot>::iterator it;
	for (it = loot.begin(); it != loot.end(); ++it) {
		if (it->animation->isLastFrame()) {
			Point p = map_to_screen(it->pos.x, it->pos.y, cam.x, cam.y);
			dest.x = p.x;
			dest.y = p.y + TILE_H_HALF;

			// adjust dest.y so that the tooltip floats above the item
			dest.y -= tooltip_margin;

			// create tooltip data if needed
			if (it->tip.isEmpty()) {

				if (it->stack.item > 0) {
					it->tip = items->getShortTooltip(it->stack);
				}
				else {
					ss.str("");
					ss << msg->get("%d %s", it->currency, CURRENCY);
					it->tip.addText(ss.str());
				}
			}

			tip->render(it->tip, dest, STYLE_TOPLABEL);
		}
	}
}

/**
 * Enemies that drop loot raise a "loot_drop" flag to notify this loot
 * manager to create loot based on that creature's level and position.
 */
void LootManager::checkEnemiesForLoot() {
	ItemStack istack;
	istack.quantity = 1;

	for (unsigned i=0; i < enemiesDroppingLoot.size(); ++i) {
		const Enemy *e = enemiesDroppingLoot[i];
		if (e->stats.quest_loot_id != 0) {
			// quest loot
			istack.item = e->stats.quest_loot_id;
			addLoot(istack, e->stats.pos);
		}
		else { // random loot
			//determine position
			Point pos = hero->pos;
			if (map->collider.is_valid_position(e->stats.pos.x, e->stats.pos.y, MOVEMENT_NORMAL))
				pos = e->stats.pos;

			determineLootByEnemy(e, pos);
		}
	}
	enemiesDroppingLoot.clear();
}

void LootManager::addEnemyLoot(const Enemy *e) {
	enemiesDroppingLoot.push_back(e);
}

/**
 * As map events occur, some might have a component named "loot"
 * Loot is created at component x,y
 */
void LootManager::checkMapForLoot() {
	Point p;
	Event_Component *ec;
	ItemStack new_loot;

	while (!map->loot.empty()) {
		ec = &map->loot.front();
		p.x = ec->x;
		p.y = ec->y;

		if (ec->s == "id") {
			new_loot.item = ec->z;
			new_loot.quantity = 1;
			addLoot(new_loot, p);
		}
		else if (ec->s == "currency") {
			addCurrency(ec->z, p);
		}
		map->loot.pop();
	}
}

/**
 * Monsters don't just drop loot equal to their level
 * The loot level spread is a bell curve
 */
int LootManager::lootLevel(int base_level) {

	int x = rand() % 100;
	int actual;

	// this loot bell curve is +/- 3 levels
	// percents: 5,10,20,30,20,10,5
	if (x <= 4) actual = base_level-3;
	else if (x <= 14) actual = base_level-2;
	else if (x <= 34) actual = base_level-1;
	else if (x <= 64) actual = base_level;
	else if (x <= 84) actual = base_level+1;
	else if (x <= 94) actual = base_level+2;
	else actual = base_level+3;

	if (actual < 1) actual = 0;
	if (actual > 20) actual = base_level;

	return actual;
}

/**
 * This function is called when there definitely is a piece of loot dropping
 * calls addLoot()
 */
void LootManager::determineLootByEnemy(const Enemy *e, Point pos) {
	ItemStack new_loot;
	std::vector<int> possible_ids;
	int common_chance = -1;

	int chance = rand() % 100;

	for (unsigned i=0; i<e->stats.loot.size(); i++) {
		if (possible_ids.empty()) {
			// find the rarest loot less than the chance roll
			if (chance < (e->stats.loot[i].chance * (hero->effects.bonus_item_find + 100)) / 100) {
				possible_ids.push_back(e->stats.loot[i].id);
				common_chance = e->stats.loot[i].chance;
				i=-1; // start searching from the beginning
				continue;
			}
		} else {
			// include loot with identical chances
			if (e->stats.loot[i].chance == common_chance)
				possible_ids.push_back(e->stats.loot[i].id);
		}
	}

	if (!possible_ids.empty()) {
		// if there was more than one item with the same chance, randomly pick one of them
		if (possible_ids.size() == 1) new_loot.item = possible_ids[0];
		else new_loot.item = possible_ids[rand() % (possible_ids.size()-1) + 1];
		new_loot.quantity = 1;

		// an item id of 0 means we should drop currency instead
		if (new_loot.item == 0) {
			int level = lootLevel(e->stats.level);
			int currency = rand() % (level * 2) + level;
			currency = (currency * (100 + hero->effects.bonus_currency)) / 100;
			addCurrency(currency, pos);
		} else {
			addLoot(new_loot, pos);
		}
	}
}

void LootManager::addLoot(ItemStack stack, Point pos) {
	// TODO: z-sort insert?
	Loot ld;
	ld.stack = stack;
	ld.pos.x = pos.x;
	ld.pos.y = pos.y;

	const string anim_id = items->items[stack.item].loot_animation;
	const string animationname = "animations/loot/" + anim_id + ".txt";
	ld.loadAnimation(animationname);
	ld.currency = 0;
	loot.push_back(ld);
	if (loot_flip) Mix_PlayChannel(-1, loot_flip, 0);
}

void LootManager::addCurrency(int count, Point pos) {
	Loot ld;
	ld.stack.item = 0;
	ld.stack.quantity = 0;
	ld.pos.x = pos.x;
	ld.pos.y = pos.y;

	int index = currency_range.size()-1;
	for (unsigned int i=0; i<currency_range.size(); i++) {
		if (count >= currency_range[i].low && (count <= currency_range[i].high || currency_range[i].high == -1)) {
			index = i;
			break;
		}
	}
	const string anim_id = currency_range[index].filename;
	const string animationname = "animations/loot/" + anim_id + ".txt";
	ld.loadAnimation(animationname);

	ld.currency = count;
	loot.push_back(ld);
	if (loot_flip) Mix_PlayChannel(-1, loot_flip, 0);
}

/**
 * Click on the map to pick up loot.  We need the camera position to translate
 * screen coordinates to map locations.  We need the hero position because
 * the hero has to be within range to pick up an item.
 */
ItemStack LootManager::checkPickup(Point mouse, Point cam, Point hero_pos, int &currency, MenuInventory *inv) {
	Point p;
	SDL_Rect r;
	ItemStack loot_stack;
	currency = 0;
	loot_stack.item = 0;
	loot_stack.quantity = 0;

	// I'm starting at the end of the loot list so that more recently-dropped
	// loot is picked up first.  If a player drops several loot in the same
	// location, picking it back up will work like a stack.
	vector<Loot>::iterator it;
	for (it = loot.end(); it != loot.begin(); ) {
		--it;

		// loot close enough to pickup?
		if (abs(hero_pos.x - it->pos.x) < LOOT_RANGE && abs(hero_pos.y - it->pos.y) < LOOT_RANGE && !it->isFlying()) {

			p = map_to_screen(it->pos.x, it->pos.y, cam.x, cam.y);

			r.w = 32;
			r.h = 48;
			r.x = p.x - 16;
			r.y = p.y - 32;

			// clicked in pickup hotspot?
			if (mouse.x > r.x && mouse.x < r.x+r.w &&
					mouse.y > r.y && mouse.y < r.y+r.h) {

				if (it->stack.item > 0 && !(inv->full(it->stack.item))) {
					loot_stack = it->stack;
					loot.erase(it);
					return loot_stack;
				}
				else if (it->stack.item > 0) {
					full_msg = true;
				}
				else if (it->currency > 0) {
					currency = it->currency;
					loot.erase(it);

					return loot_stack;
				}
			}
		}
	}
	return loot_stack;
}

/**
 * Autopickup loot if enabled in the engine
 * Currently, only currency is checked for autopickup
 */
ItemStack LootManager::checkAutoPickup(Point hero_pos, int &currency) {
	ItemStack loot_stack;
	currency = 0;
	loot_stack.item = 0;
	loot_stack.quantity = 0;

	vector<Loot>::iterator it;
	for (it = loot.end(); it != loot.begin(); ) {
		--it;
		if (abs(hero_pos.x - it->pos.x) < AUTOPICKUP_RANGE && abs(hero_pos.y - it->pos.y) < AUTOPICKUP_RANGE && !it->isFlying()) {
			if (it->currency > 0 && AUTOPICKUP_CURRENCY) {
				currency = it->currency;
				loot.erase(it);
				return loot_stack;
			}
		}
	}
	return loot_stack;
}

void LootManager::addRenders(vector<Renderable> &ren, vector<Renderable> &ren_dead) {
	vector<Loot>::iterator it;
	for (it = loot.begin(); it != loot.end(); ++it) {
		Renderable r = it->animation->getCurrentFrame(0);
		r.map_pos.x = it->pos.x;
		r.map_pos.y = it->pos.y;

		(it->animation->isLastFrame() ? ren_dead : ren).push_back(r);
	}
}

LootManager::~LootManager() {
	// remove all items in the item database
	for (unsigned int i=0; i < items->items.size(); i++) {
		string anim_id = items->items[i].loot_animation;
		if (anim_id == "") continue;
		string animationname = "animations/loot/" + anim_id + ".txt";
		anim->decreaseCount(animationname);
	}

	// currency
	for (unsigned int i=0; i<currency_range.size(); i++) {
		string animationname = "animations/loot/" + currency_range[i].filename + ".txt";
		anim->decreaseCount(animationname);
	}

	// remove items, so Loots get destroyed!
	loot.clear();

	anim->cleanUp();

	Mix_FreeChunk(loot_flip);

	lootManager = 0;
	delete tip;
}
