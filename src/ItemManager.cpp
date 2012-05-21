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
 * class ItemManager
 */

#include "ItemManager.h"
#include "FileParser.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsFileSystem.h"
#include "WidgetLabel.h"

#include <sstream>
#include <fstream>

using namespace std;


ItemManager::ItemManager() {

	items = new Item[MAX_ITEM_ID];

	for (int i=0; i<MAX_ITEM_ID; i++) {
		items[i].bonus_stat = new string[ITEM_MAX_BONUSES];
		items[i].bonus_val = new int[ITEM_MAX_BONUSES];
		for (int j=0; j<ITEM_MAX_BONUSES; j++) {
			items[i].bonus_stat[j] = "";
			items[i].bonus_val[j] = 0;
		}
	}

	vendor_ratio = 4; // this means scrap/vendor pays 1/4th price to buy items from hero
	loadAll();
	loadSounds();
	loadIcons();
}

/**
 * Load all items files in all mods
 */
void ItemManager::loadAll() {

	string test_path;

	// load each items.txt file. Individual item IDs can be overwritten with mods.
	for (unsigned int i = 0; i < mods->mod_list.size(); i++) {

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/items/items.txt";

		if (fileExists(test_path)) {
			this->load(test_path);
		}
	}

}

/**
 * Load a specific items file
 *
 * @param filename The full path and name of the file to load
 */
void ItemManager::load(const string& filename) {
	FileParser infile;
	int id = 0;
	string s;
	int bonus_counter = 0;

	if (infile.open(filename)) {

		while (infile.next()) {
			if (infile.key == "id") {
				id = atoi(infile.val.c_str());

				// new item, reset bonus counter
				bonus_counter = 0;
			}
			else if (infile.key == "name")
				items[id].name = msg->get(infile.val);
			else if (infile.key == "level")
				items[id].level = atoi(infile.val.c_str());
			else if (infile.key == "icon") {
				items[id].icon32 = atoi(infile.nextValue().c_str());
				items[id].icon64 = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "quality") {
				if (infile.val == "low")
					items[id].quality = ITEM_QUALITY_LOW;
				else if (infile.val == "high")
					items[id].quality = ITEM_QUALITY_HIGH;
				else if (infile.val == "epic")
					items[id].quality = ITEM_QUALITY_EPIC;
			}
			else if (infile.key == "type") {
				if (infile.val == "main")
					items[id].type = ITEM_TYPE_MAIN;
				else if (infile.val == "body")
					items[id].type = ITEM_TYPE_BODY;
				else if (infile.val == "off")
					items[id].type = ITEM_TYPE_OFF;
				else if (infile.val == "artifact")
					items[id].type = ITEM_TYPE_ARTIFACT;
				else if (infile.val == "consumable")
					items[id].type = ITEM_TYPE_CONSUMABLE;
				else if (infile.val == "gem")
					items[id].type = ITEM_TYPE_GEM;
				else if (infile.val == "quest")
					items[id].type = ITEM_TYPE_QUEST;
			}
			else if (infile.key == "dmg") {
				items[id].dmg_min = atoi(infile.nextValue().c_str());
				if (infile.val.length() > 0)
					items[id].dmg_max = atoi(infile.nextValue().c_str());
				else
					items[id].dmg_max = items[id].dmg_min;
			}
			else if (infile.key == "abs") {
				items[id].abs_min = atoi(infile.nextValue().c_str());
				if (infile.val.length() > 0)
					items[id].abs_max = atoi(infile.nextValue().c_str());
				else
					items[id].abs_max = items[id].abs_min;
			}
			else if (infile.key == "req") {
				s = infile.nextValue();
				if (s == "p")
					items[id].req_stat = REQUIRES_PHYS;
				else if (s == "m")
					items[id].req_stat = REQUIRES_MENT;
				else if (s == "o")
					items[id].req_stat = REQUIRES_OFF;
				else if (s == "d")
					items[id].req_stat = REQUIRES_DEF;
				items[id].req_val = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "bonus") {
				if (bonus_counter < ITEM_MAX_BONUSES) {
					items[id].bonus_stat[bonus_counter] = infile.nextValue();
					items[id].bonus_val[bonus_counter] = atoi(infile.nextValue().c_str());
					bonus_counter++;
				}
			}
			else if (infile.key == "sfx") {
				if (infile.val == "book")
					items[id].sfx = SFX_BOOK;
				else if (infile.val == "cloth")
					items[id].sfx = SFX_CLOTH;
				else if (infile.val == "coins")
					items[id].sfx = SFX_COINS;
				else if (infile.val == "gem")
					items[id].sfx = SFX_GEM;
				else if (infile.val == "leather")
					items[id].sfx = SFX_LEATHER;
				else if (infile.val == "metal")
					items[id].sfx = SFX_METAL;
				else if (infile.val == "page")
					items[id].sfx = SFX_PAGE;
				else if (infile.val == "maille")
					items[id].sfx = SFX_MAILLE;
				else if (infile.val == "object")
					items[id].sfx = SFX_OBJECT;
				else if (infile.val == "heavy")
					items[id].sfx = SFX_HEAVY;
				else if (infile.val == "wood")
					items[id].sfx = SFX_WOOD;
				else if (infile.val == "potion")
					items[id].sfx = SFX_POTION;
			}
			else if (infile.key == "gfx")
				items[id].gfx = infile.val;
			else if (infile.key == "loot")
				items[id].loot = infile.val;
			else if (infile.key == "power")
				items[id].power = atoi(infile.val.c_str());
			else if (infile.key == "power_mod")
				items[id].power_mod = atoi(infile.val.c_str());
			else if (infile.key == "power_desc")
				items[id].power_desc = msg->get(infile.val);
			else if (infile.key == "price")
				items[id].price = atoi(infile.val.c_str());
			else if (infile.key == "price_sell")
				items[id].price_sell = atoi(infile.val.c_str());
			else if (infile.key == "max_quantity")
				items[id].max_quantity = atoi(infile.val.c_str());
			else if (infile.key == "rand_loot")
				items[id].rand_loot = atoi(infile.val.c_str());
			else if (infile.key == "rand_vendor")
				items[id].rand_vendor = atoi(infile.val.c_str());
			else if (infile.key == "pickup_status")
				items[id].pickup_status = infile.val;
			else if (infile.key == "stepfx")
				items[id].stepfx = infile.val;

		}
		infile.close();
	}
}

void ItemManager::loadSounds() {

    if (audio == true) {
        sfx[SFX_BOOK] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_book.ogg").c_str());
        sfx[SFX_CLOTH] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_cloth.ogg").c_str());
        sfx[SFX_COINS] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_coins.ogg").c_str());
        sfx[SFX_GEM] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_gem.ogg").c_str());
        sfx[SFX_LEATHER] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_leather.ogg").c_str());
        sfx[SFX_METAL] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_metal.ogg").c_str());
        sfx[SFX_PAGE] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_page.ogg").c_str());
        sfx[SFX_MAILLE] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_maille.ogg").c_str());
        sfx[SFX_OBJECT] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_object.ogg").c_str());
        sfx[SFX_HEAVY] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_heavy.ogg").c_str());
        sfx[SFX_WOOD] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_wood.ogg").c_str());
        sfx[SFX_POTION] = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_potion.ogg").c_str());
    } else {
        sfx[SFX_BOOK] = NULL;
        sfx[SFX_CLOTH] = NULL;
        sfx[SFX_COINS] = NULL;
        sfx[SFX_GEM] = NULL;
        sfx[SFX_LEATHER] = NULL;
        sfx[SFX_METAL] = NULL;
        sfx[SFX_PAGE] = NULL;
        sfx[SFX_MAILLE] = NULL;
        sfx[SFX_OBJECT] = NULL;
        sfx[SFX_HEAVY] = NULL;
        sfx[SFX_WOOD] = NULL;
    }
}

/**
 * Icon sets
 */
void ItemManager::loadIcons() {

	icons32 = IMG_Load(mods->locate("images/icons/icons32.png").c_str());
	icons64 = IMG_Load(mods->locate("images/icons/icons64.png").c_str());

	if(!icons32 || !icons64) {
		fprintf(stderr, "Couldn't load icons: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = icons32;
	icons32 = SDL_DisplayFormatAlpha(icons32);
	SDL_FreeSurface(cleanup);

	cleanup = icons64;
	icons64 = SDL_DisplayFormatAlpha(icons64);
	SDL_FreeSurface(cleanup);
}

/**
 * Renders icons at 32px size or 64px size
 * Also display the stack size
 */
void ItemManager::renderIcon(ItemStack stack, int x, int y, int size) {
	int columns;

	dest.x = x;
	dest.y = y;
	src.w = src.h = dest.w = dest.h = size;
	if (size == ICON_SIZE_32) {
		columns = icons32->w / 32;
		src.x = (items[stack.item].icon32 % columns) * size;
		src.y = (items[stack.item].icon32 / columns) * size;
		SDL_BlitSurface(icons32, &src, screen, &dest);
	}
	else if (size == ICON_SIZE_64) {
		columns = icons64->w / 64;
		src.x = (items[stack.item].icon64 % columns) * size;
		src.y = (items[stack.item].icon64 / columns) * size;
		SDL_BlitSurface(icons64, &src, screen, &dest);
	}

	if( stack.quantity > 1 || items[stack.item].max_quantity > 1) {
		// stackable item : show the quantity
		stringstream ss;
		ss << stack.quantity;

		WidgetLabel label;
		label.set(dest.x + 2, dest.y + 2, JUSTIFY_LEFT, VALIGN_TOP, ss.str(), FONT_WHITE);
		label.render();
	}
}

void ItemManager::playSound(int item) {
    if (items[item].sfx != SFX_NONE)
        if (sfx[items[item].sfx])
            Mix_PlayChannel(-1, sfx[items[item].sfx], 0);
}

void ItemManager::playCoinsSound() {
    if (sfx[SFX_COINS])
        Mix_PlayChannel(-1, sfx[SFX_COINS], 0);
}

TooltipData ItemManager::getShortTooltip(ItemStack stack) {
	stringstream ss;
	TooltipData tip;

	if (stack.item == 0) return tip;

	// name
	if( stack.quantity > 1) {
		ss << stack.quantity << " " << items[stack.item].name;
	} else {
		ss << items[stack.item].name;
	}
	tip.lines[tip.num_lines++] = ss.str();

	// color quality
	if (items[stack.item].quality == ITEM_QUALITY_LOW) {
		tip.colors[0] = FONT_GRAY;
	}
	else if (items[stack.item].quality == ITEM_QUALITY_HIGH) {
		tip.colors[0] = FONT_GREEN;
	}
	else if (items[stack.item].quality == ITEM_QUALITY_EPIC) {
		tip.colors[0] = FONT_BLUE;
	}

	return tip;
}

/**
 * Create detailed tooltip showing all relevant item info
 */
TooltipData ItemManager::getTooltip(int item, StatBlock *stats, bool vendor_view) {
	TooltipData tip;

	if (item == 0) return tip;

	// name
	tip.lines[tip.num_lines++] = items[item].name;

	// color quality
	if (items[item].quality == ITEM_QUALITY_LOW) {
		tip.colors[0] = FONT_GRAY;
	}
	else if (items[item].quality == ITEM_QUALITY_HIGH) {
		tip.colors[0] = FONT_GREEN;
	}
	else if (items[item].quality == ITEM_QUALITY_EPIC) {
		tip.colors[0] = FONT_BLUE;
	}

	// level
	if (items[item].level != 0) {
		tip.lines[tip.num_lines++] = msg->get("Level %d", items[item].level);
	}

	// type
	if (items[item].type != ITEM_TYPE_OTHER) {
		if (items[item].type == ITEM_TYPE_MAIN)
			tip.lines[tip.num_lines++] = msg->get("Main Hand");
		else if (items[item].type == ITEM_TYPE_BODY)
			tip.lines[tip.num_lines++] = msg->get("Body");
		else if (items[item].type == ITEM_TYPE_OFF)
			tip.lines[tip.num_lines++] = msg->get("Off Hand");
		else if (items[item].type == ITEM_TYPE_ARTIFACT)
			tip.lines[tip.num_lines++] = msg->get("Artifact");
		else if (items[item].type == ITEM_TYPE_CONSUMABLE)
			tip.lines[tip.num_lines++] = msg->get("Consumable");
		else if (items[item].type == ITEM_TYPE_GEM)
			tip.lines[tip.num_lines++] = msg->get("Gem");
		else if (items[item].type == ITEM_TYPE_QUEST)
			tip.lines[tip.num_lines++] = msg->get("Quest Item");
	}

	// damage
	if (items[item].dmg_max > 0) {
		if (items[item].req_stat == REQUIRES_PHYS) {
			if (items[item].dmg_min < items[item].dmg_max)
				tip.lines[tip.num_lines++] = msg->get("Melee damage: %d-%d", items[item].dmg_min, items[item].dmg_max);
			else
				tip.lines[tip.num_lines++] = msg->get("Melee damage: %d", items[item].dmg_max);
		}
		else if (items[item].req_stat == REQUIRES_MENT) {
			if (items[item].dmg_min < items[item].dmg_max)
				tip.lines[tip.num_lines++] = msg->get("Mental damage: %d-%d", items[item].dmg_min, items[item].dmg_max);
			else
				tip.lines[tip.num_lines++] = msg->get("Mental damage: %d", items[item].dmg_max);
		}
		else if (items[item].req_stat == REQUIRES_OFF) {
			if (items[item].dmg_min < items[item].dmg_max)
				tip.lines[tip.num_lines++] = msg->get("Ranged damage: %d-%d", items[item].dmg_min, items[item].dmg_max);
			else
				tip.lines[tip.num_lines++] = msg->get("Ranged damage: %d", items[item].dmg_max);
		}
	}

	// absorb
	if (items[item].abs_max > 0) {
		if (items[item].abs_min < items[item].abs_max)
			tip.lines[tip.num_lines++] = msg->get("Absorb: %d-%d", items[item].abs_min, items[item].abs_max);
		else
			tip.lines[tip.num_lines++] = msg->get("Absorb: %d", items[item].abs_max);
	}

	// bonuses
	int bonus_counter = 0;
	string modifier;
	while (items[item].bonus_stat[bonus_counter] != "") {
		if (items[item].bonus_val[bonus_counter] > 0) {
			modifier = msg->get("Increases %s by %d", items[item].bonus_val[bonus_counter], msg->get(items[item].bonus_stat[bonus_counter]));
			tip.colors[tip.num_lines] = FONT_GREEN;
		}
		else {
			modifier = msg->get("Decreases %s by %d", items[item].bonus_val[bonus_counter], msg->get(items[item].bonus_stat[bonus_counter]));
			tip.colors[tip.num_lines] = FONT_RED;
		}
		tip.lines[tip.num_lines++] = modifier;
		bonus_counter++;
		if (bonus_counter == ITEM_MAX_BONUSES) break;
	}

	// power
	if (items[item].power_desc != "") {
		tip.colors[tip.num_lines] = FONT_GREEN;
		tip.lines[tip.num_lines++] = items[item].power_desc;
	}

	// requirement
	if (items[item].req_val > 0) {
		if (items[item].req_stat == REQUIRES_PHYS) {
			if (stats->get_physical() < items[item].req_val) tip.colors[tip.num_lines] = FONT_RED;
			tip.lines[tip.num_lines++] = msg->get("Requires Physical %d", items[item].req_val);
		}
		else if (items[item].req_stat == REQUIRES_MENT) {
			if (stats->get_mental() < items[item].req_val) tip.colors[tip.num_lines] = FONT_RED;
			tip.lines[tip.num_lines++] = msg->get("Requires Mental %d", items[item].req_val);
		}
		else if (items[item].req_stat == REQUIRES_OFF) {
			if (stats->get_offense() < items[item].req_val) tip.colors[tip.num_lines] = FONT_RED;
			tip.lines[tip.num_lines++] = msg->get("Requires Offense %d", items[item].req_val);
		}
		else if (items[item].req_stat == REQUIRES_DEF) {
			if (stats->get_defense() < items[item].req_val) tip.colors[tip.num_lines] = FONT_RED;
			tip.lines[tip.num_lines++] = msg->get("Requires Defense %d", items[item].req_val);
		}
	}

	// buy or sell price
	if (items[item].price > 0) {

		if (vendor_view) {
			if (stats->gold < items[item].price) tip.colors[tip.num_lines] = FONT_RED;
			if (items[item].max_quantity <= 1)
				tip.lines[tip.num_lines++] = msg->get("Buy Price: %d gold", items[item].price);
			else
				tip.lines[tip.num_lines++] = msg->get("Buy Price: %d gold each", items[item].price);
		}
		else {
			int price_per_unit;
			if(items[item].price_sell != 0)
				price_per_unit = items[item].price_sell;
			else
				price_per_unit = items[item].price/vendor_ratio;
			if (price_per_unit == 0) price_per_unit = 1;
			if (items[item].max_quantity <= 1)
				tip.lines[tip.num_lines++] = msg->get("Sell Price: %d gold", price_per_unit);
			else
				tip.lines[tip.num_lines++] = msg->get("Sell Price: %d gold each", price_per_unit);
		}

	}

	return tip;
}

ItemManager::~ItemManager() {

	SDL_FreeSurface(icons32);
	SDL_FreeSurface(icons64);

    if (audio == true) {
        for (int i=0; i<12; i++) {
            if (sfx[i])
                Mix_FreeChunk(sfx[i]);
        }
    }

	for (int i=0; i<MAX_ITEM_ID; i++) {
		delete[] items[i].bonus_stat;
		delete[] items[i].bonus_val;
	}

	delete[] items;

}

/**
 * Compare two item stack to be able to sorting them on their item_id in the vendors' stock
 */
bool ItemStack::operator > (ItemStack param) {
	if (item == 0 && param.item > 0) {
		// Make the empty slots the last while sorting
		return true;
	} else if (item > 0 && param.item == 0) {
		// Make the empty slots the last while sorting
		return false;
	} else {
		return item > param.item;
	}
}

