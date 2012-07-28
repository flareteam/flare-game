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

#include "EnemyManager.h"
#include "LootManager.h"
#include "Menu.h"
#include "MenuInventory.h"
#include "SharedResources.h"

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

	tooltip_margin = 32; // pixels between loot drop center and label

	animation_count = 0;

	for (int i=0; i<64; i++) {
		flying_loot[i] = NULL;
		animation_id[i] = "";
	}

	loot_flip = NULL;

	// reset current map loot
	loot.clear();

	// reset loot table
	for (int lvl=0; lvl<15; lvl++) {
		loot_table_count[lvl] = 0;
		for (int num=0; num<256; num++) {
			loot_table[lvl][num] = 0;
		}
	}

	loadGraphics();
	calcTables();
	if (audio == true)
		loot_flip = Mix_LoadWAV(mods->locate("soundfx/flying_loot.ogg").c_str());
	full_msg = false;

	anim_loot_frames = 6;
	anim_loot_duration = 3;

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

	string anim_id;
	bool new_anim;

	// check all items in the item database
	for (unsigned int i=0; i < items->items.size(); i++) {
		anim_id = items->items[i].loot;

		new_anim = true;

		if (anim_id != "") {

			// do we have this animation loaded already?
			for (int j=0; j<animation_count; j++) {
				if (anim_id == animation_id[j]) new_anim = false;
			}

			if (new_anim) {
				flying_loot[animation_count] = IMG_Load(mods->locate("images/loot/" + anim_id + ".png").c_str());

				if (flying_loot[animation_count]) {
					animation_id[animation_count] = anim_id;
					animation_count++;
				}
			}
		}
	}

	// gold
	flying_gold[0] = IMG_Load(mods->locate("images/loot/coins5.png").c_str());
	flying_gold[1] = IMG_Load(mods->locate("images/loot/coins25.png").c_str());
	flying_gold[2] = IMG_Load(mods->locate("images/loot/coins100.png").c_str());

	// set magic pink transparency
	for (int i=0; i<animation_count; i++) {
		SDL_SetColorKey( flying_loot[i], SDL_SRCCOLORKEY, SDL_MapRGB(flying_loot[i]->format, 255, 0, 255) );

		// optimize
		SDL_Surface *cleanup = flying_loot[i];
		flying_loot[i] = SDL_DisplayFormatAlpha(flying_loot[i]);
		SDL_FreeSurface(cleanup);
	}
	for (int i=0; i<3; i++) {
		SDL_SetColorKey( flying_gold[i], SDL_SRCCOLORKEY, SDL_MapRGB(flying_gold[i]->format, 255, 0, 255) );

		// optimize
		SDL_Surface *cleanup = flying_gold[i];
		flying_gold[i] = SDL_DisplayFormatAlpha(flying_gold[i]);
		SDL_FreeSurface(cleanup);
	}
}

/**
 * Each item has a level, roughly associated with what level monsters drop that item.
 * Each item also has a quality which affects how often it drops.
 * Here we calculate loot probability by level so that when loot drops we
 * can quickly choose what loot should drop.
 */
void LootManager::calcTables() {

	int level;

	for (unsigned int i=0; i<items->items.size(); i++) {
		level = items->items[i].level;
		if (level > 0) {
			if (items->items[i].quality == ITEM_QUALITY_LOW) {
				for (int j=0; j<RARITY_LOW; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
			if (items->items[i].quality == ITEM_QUALITY_NORMAL) {
				for (int j=0; j<RARITY_NORMAL; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
			if (items->items[i].quality == ITEM_QUALITY_HIGH) {
				for (int j=0; j<RARITY_HIGH; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
			if (items->items[i].quality == ITEM_QUALITY_EPIC) {
				for (int j=0; j<RARITY_EPIC; j++) {
					loot_table[level][loot_table_count[level]] = i;
					loot_table_count[level]++;
				}
			}
		}
	}
}

void LootManager::handleNewMap() {
	loot.clear();
}

void LootManager::logic() {
	int max_frame = anim_loot_frames * anim_loot_duration - 1;

	vector<LootDef>::iterator it;
	for (it = loot.begin(); it != loot.end(); it++) {

		// animate flying loot
		if (it->frame < max_frame)
			it->frame++;

		if (it->frame == max_frame-1) {
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
 * If an item is flying, it hasn't completed its "flying loot" animation.
 * Only allow loot to be picked up if it is grounded.
 */
bool LootManager::isFlying(const LootDef &ld) {
	int max_frame = anim_loot_frames * anim_loot_duration - 1;
	if (ld.frame == max_frame) return false;
	return true;
}

/**
 * Show all tooltips for loot on the floor
 */
void LootManager::renderTooltips(Point cam) {
	Point dest;
	stringstream ss;

	int max_frame = anim_loot_frames * anim_loot_duration - 1;

	vector<LootDef>::iterator it;
	for (it = loot.begin(); it != loot.end(); it++) {
		if (it->frame == max_frame) {
			Point p = map_to_screen(it->pos.x, it->pos.y, cam.x, cam.y);
			dest.x = p.x;
			dest.y = p.y + TILE_H_HALF;

			// adjust dest.y so that the tooltip floats above the item
			dest.y -= tooltip_margin;

			// create tooltip data if needed
			if (it->tip.tip_buffer == NULL) {

				if (it->stack.item > 0) {
					it->tip = items->getShortTooltip(it->stack);
				}
				else {
					it->tip.num_lines = 1;
					it->tip.colors[0] = FONT_WHITE;
					ss << msg->get("%d Gold", it->gold);
					it->tip.lines[0] = ss.str();
					ss.str("");
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
			if (map->collider.valid_position(e->stats.pos.x, e->stats.pos.y, MOVEMENT_NORMAL))
				pos = e->stats.pos;

			// if no probability density function  is given, do a random loot
			if (e->stats.item_classes.empty())
				determineLoot(e->stats.level, pos);
			else
				determineLootByClass(e, pos);
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

		if (ec->s == "random") {
			determineLoot(ec->z, p);
		}
		else if (ec->s == "id") {
			new_loot.item = ec->z;
			new_loot.quantity = 1;
			addLoot(new_loot, p);
		}
		else if (ec->s == "currency") {
			addGold(ec->z, p);
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
 * base_level represents the average quality of this loot
 * calls addLoot()
 */
void LootManager::determineLoot(int base_level, Point pos) {
	int level = lootLevel(base_level);
	ItemStack new_loot;

	if (level > 0 && loot_table_count[level] > 0) {

		// coin flip whether the treasure is cash or items
		if (rand() % 2 == 0) {
			int roll = rand() % loot_table_count[level];
			new_loot.item = loot_table[level][roll];
			new_loot.quantity = rand() % items->items[new_loot.item].rand_loot + 1;
			addLoot(new_loot, pos);
		}
		else {
			// gold range is level to 3x level
			addGold(rand() % (level * 2) + level, pos);
		}
	}
}

void LootManager::determineLootByClass(const Enemy *e, Point pos) {
	// quality level of loot
	int level = lootLevel(e->stats.level);
	if (level <= 0)
		return;

	// roll a dice to select the type
	int typeSelector = rand() % e->stats.item_class_prob_sum;
	int typeSelectorIndex = 0;

	// look up type hit by dice with correct probabilities
	while (typeSelector >= e->stats.item_class_prob[typeSelectorIndex]) {
		typeSelector -= e->stats.item_class_prob[typeSelectorIndex];
		typeSelectorIndex++;
	}
	string item_class = e->stats.item_classes[typeSelectorIndex];

	if (item_class == "gold")
		addGold(rand() % (level * 2) + level, pos);
	else {
		// search for the itemclass
		unsigned int index;
		for (index = 0; index < items->item_class_names.size(); index++) {
			if (items->item_class_names[index] == item_class)
				break;
		}
		if (index == items->item_class_names.size()) {
			// item class name not found:
			cout << "item class " << item_class << " has no items." << endl;
			return;
		}

		if (level > 0 && items->item_class_items[index].size() > 0) {
			int roll = rand() % items->item_class_items[index].size();
			ItemStack new_loot;
			new_loot.item = items->item_class_items[index][roll];
			new_loot.quantity = rand() % items->items[new_loot.item].rand_loot + 1;
			addLoot(new_loot, pos);
		}
	}
}

/**
 * Choose a random item.
 * Useful for filling in a Vendor's wares.
 */
int LootManager::randomItem(int base_level) {
	int level = lootLevel(base_level);
	if (level > 0 && loot_table_count[level] > 0) {
		int roll = rand() % loot_table_count[level];
		return loot_table[level][roll];
	}
	return 0;
}

void LootManager::addLoot(ItemStack stack, Point pos) {
	// TODO: z-sort insert?
	LootDef ld;
	ld.stack = stack;
	ld.pos.x = pos.x;
	ld.pos.y = pos.y;
	ld.frame = 0;
	ld.gold = 0;
	loot.push_back(ld);
	if (loot_flip) Mix_PlayChannel(-1, loot_flip, 0);
}

void LootManager::addGold(int count, Point pos) {
	LootDef ld;
	ld.stack.item = 0;
	ld.stack.quantity = 0;
	ld.pos.x = pos.x;
	ld.pos.y = pos.y;
	ld.frame = 0;
	ld.gold = count;
	loot.push_back(ld);
	if (loot_flip) Mix_PlayChannel(-1, loot_flip, 0);
}

/**
 * Click on the map to pick up loot.  We need the camera position to translate
 * screen coordinates to map locations.  We need the hero position because
 * the hero has to be within range to pick up an item.
 */
ItemStack LootManager::checkPickup(Point mouse, Point cam, Point hero_pos, int &gold, MenuInventory *inv) {
	Point p;
	SDL_Rect r;
	ItemStack loot_stack;
	gold = 0;
	loot_stack.item = 0;
	loot_stack.quantity = 0;

	// I'm starting at the end of the loot list so that more recently-dropped
	// loot is picked up first.  If a player drops several loot in the same
	// location, picking it back up will work like a stack.
	vector<LootDef>::iterator it;
	for (it = loot.end(); it != loot.begin(); ) {
		it--;

		// loot close enough to pickup?
		if (abs(hero_pos.x - it->pos.x) < LOOT_RANGE && abs(hero_pos.y - it->pos.y) < LOOT_RANGE && !isFlying(*it)) {

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
				else if (it->gold > 0) {
					gold = it->gold;
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
 * Currently, only gold is checked for autopickup
 */
ItemStack LootManager::checkAutoPickup(Point cam, Point hero_pos, int &gold, MenuInventory *inv) {
	ItemStack loot_stack;
	gold = 0;
	loot_stack.item = 0;
	loot_stack.quantity = 0;

	vector<LootDef>::iterator it;
	for (it = loot.end(); it != loot.begin(); ) {
		it--;
		if (abs(hero_pos.x - it->pos.x) < AUTOPICKUP_RANGE && abs(hero_pos.y - it->pos.y) < AUTOPICKUP_RANGE && !isFlying(*it)) {
			if (it->gold > 0 && AUTOPICKUP_GOLD) {
				gold = it->gold;
				loot.erase(it);
				return loot_stack;
			}
		}
	}
	return loot_stack;
}

void LootManager::addRenders(vector<Renderable> &renderables) {
	vector<LootDef>::iterator it;
	for (it = loot.begin(); it != loot.end(); it++) {
		Renderable r;
		r.map_pos.x = it->pos.x;
		r.map_pos.y = it->pos.y;

		// Right now the animation settings (number of frames, speed, frame size)
		// are hard coded.  At least move these to consts in the header.

		r.src.x = (it->frame / anim_loot_duration) * 64;
		r.src.y = 0;
		r.src.w = 64;
		r.src.h = 128;
		r.offset.x = 32;
		r.offset.y = 112;
		r.object_layer = true;

		if (it->stack.item > 0) {
			// item
			for (int i=0; i<animation_count; i++) {
				if (items->items[it->stack.item].loot == animation_id[i])
					r.sprite = flying_loot[i];
			}
		}
		else if (it->gold > 0) {
			// gold
			if (it->gold <= 9)
				r.sprite = flying_gold[0];
			else if (it->gold <= 25)
				r.sprite = flying_gold[1];
			else
				r.sprite = flying_gold[2];
		}
		renderables.push_back(r);
	}
}

LootManager::~LootManager() {

	for (int i=0; i<64; i++)
		SDL_FreeSurface(flying_loot[i]);

	for (int i=0; i<3; i++)
		SDL_FreeSurface(flying_gold[i]);

	Mix_FreeChunk(loot_flip);

	// clear loot tooltips to free buffer memory
	loot.clear();

	lootManager = 0;
	delete tip;
}
