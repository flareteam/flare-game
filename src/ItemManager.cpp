/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
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
 * class ItemManager
 */

#include "ItemManager.h"
#include "FileParser.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"
#include "WidgetLabel.h"

#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <sstream>

using namespace std;

/**
 * Resizes vector vec, so it can fit index id.
 */
template <typename Ty_>
static inline void ensureFitsId(vector<Ty_>& vec, int id)
{
	// id's are always greater or equal 1;
	if (id < 1) return;

	typedef typename vector<Ty_>::size_type VecSz;

	if (vec.size() <= VecSz(id+1))
		vec.resize(id+1);
}

/**
 * Trims vector allocated memory to its size.
 *
 * Emulates C++2011 vector::shrink_to_fit().
 * It is sometimes also called "swap trick".
 */
template <typename Ty_>
static inline void shrinkVecToFit(std::vector<Ty_>& vec)
{
	if (vec.capacity() != vec.size())
		std::vector<Ty_>(vec).swap(vec);
}

ItemManager::ItemManager()
	: color_normal(font->getColor("item_normal"))
	, color_low(font->getColor("item_low"))
	, color_high(font->getColor("item_high"))
	, color_epic(font->getColor("item_epic"))
	, color_bonus(font->getColor("item_bonus"))
	, color_penalty(font->getColor("item_penalty"))
	, color_requirements_not_met(font->getColor("requirements_not_met"))
	, color_flavor(font->getColor("item_flavor"))
{
	// NB: 20 is arbitrary picked number, but it looks like good start.
	items.reserve(20);
	item_sets.reserve(5);

	loadAll();
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

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/items/types.txt";

		if (fileExists(test_path)) {
			this->loadTypes(test_path);
		}

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/items/sets.txt";

		if (fileExists(test_path)) {
			this->loadSets(test_path);
		}
	}

	/*
	 * Shrinks the items vector to the absolute needed size.
	 *
	 * While loading the items, the item vector grows dynamically. To have
	 * no much time overhead for reallocating the vector, a new reallocation
	 * is twice as large as the needed item id, which means in the worst case
	 * the item vector was reallocated for loading the last element, so the
	 * vector is twice as large as needed. This memory is definitly not used,
	 * so we can free it.
	 */
	shrinkVecToFit(items);
	shrinkVecToFit(item_sets);

	if (items.empty()) fprintf(stderr, "No items were found.\n");
	if (item_sets.empty()) printf("No item sets were found.\n");
}

/**
 * Load a specific items file
 *
 * @param filename The full path and name of the file to load
 */
void ItemManager::load(const string& filename) {
	FileParser infile;
	if (!infile.open(filename)) {
		fprintf(stderr, "Unable to open %s!\n", filename.c_str());
		return;
	}

	int id = 0;
	bool id_line = false;
	while (infile.next()) {
		if (infile.key == "id") {
			id_line = true;
			id = toInt(infile.val);
			ensureFitsId(items, id+1);
		} else id_line = false;

		if (id < 1) {
			if (id_line) fprintf(stderr, "Item index out of bounds 1-%d, skipping\n", INT_MAX);
			continue;
		}
		if (id_line) continue;

		assert(items.size() > std::size_t(id));

		if (infile.key == "name")
			items[id].name = msg->get(infile.val);
		else if (infile.key == "flavor")
			items[id].flavor = msg->get(infile.val);
		else if (infile.key == "level")
			items[id].level = toInt(infile.val);
		else if (infile.key == "icon") {
			items[id].icon = toInt(infile.nextValue());
		}
		else if (infile.key == "quality") {
			if (infile.val == "low")
				items[id].quality = ITEM_QUALITY_LOW;
			else if (infile.val == "high")
				items[id].quality = ITEM_QUALITY_HIGH;
			else if (infile.val == "epic")
				items[id].quality = ITEM_QUALITY_EPIC;
		}
		else if (infile.key == "item_type") {
				items[id].type = infile.val;
		}
		else if (infile.key == "dmg_melee") {
			items[id].dmg_melee_min = toInt(infile.nextValue());
			if (infile.val.length() > 0)
				items[id].dmg_melee_max = toInt(infile.nextValue());
			else
				items[id].dmg_melee_max = items[id].dmg_melee_min;
		}
		else if (infile.key == "dmg_ranged") {
			items[id].dmg_ranged_min = toInt(infile.nextValue());
			if (infile.val.length() > 0)
				items[id].dmg_ranged_max = toInt(infile.nextValue());
			else
				items[id].dmg_ranged_max = items[id].dmg_ranged_min;
		}
		else if (infile.key == "dmg_ment") {
			items[id].dmg_ment_min = toInt(infile.nextValue());
			if (infile.val.length() > 0)
				items[id].dmg_ment_max = toInt(infile.nextValue());
			else
				items[id].dmg_ment_max = items[id].dmg_ment_min;
		}
		else if (infile.key == "abs") {
			items[id].abs_min = toInt(infile.nextValue());
			if (infile.val.length() > 0)
				items[id].abs_max = toInt(infile.nextValue());
			else
				items[id].abs_max = items[id].abs_min;
		}
		else if (infile.key == "req") {
			string s = infile.nextValue();
			if (s == "p")
				items[id].req_stat = REQUIRES_PHYS;
			else if (s == "m")
				items[id].req_stat = REQUIRES_MENT;
			else if (s == "o")
				items[id].req_stat = REQUIRES_OFF;
			else if (s == "d")
				items[id].req_stat = REQUIRES_DEF;
			items[id].req_val = toInt(infile.nextValue());
		}
		else if (infile.key == "bonus") {
			items[id].bonus_stat.push_back(infile.nextValue());
			items[id].bonus_val.push_back(toInt(infile.nextValue()));
		}
		else if (infile.key == "soundfx") {
			items[id].sfx = snd->load(infile.val, "ItemManager");
		}
		else if (infile.key == "gfx")
			items[id].gfx = infile.val;
		else if (infile.key == "loot_animation")
			items[id].loot_animation = infile.val;
		else if (infile.key == "power") {
			if (toInt(infile.val) > 0) {
				items[id].power = toInt(infile.val);
			}
			else fprintf(stderr, "Power index inside item %d definition out of bounds 1-%d, skipping item\n", id, INT_MAX);
		}
		else if (infile.key == "power_mod")
			items[id].power_mod = toInt(infile.val);
		else if (infile.key == "power_desc")
			items[id].power_desc = msg->get(infile.val);
		else if (infile.key == "price")
			items[id].price = toInt(infile.val);
		else if (infile.key == "price_sell")
			items[id].price_sell = toInt(infile.val);
		else if (infile.key == "max_quantity")
			items[id].max_quantity = toInt(infile.val);
		else if (infile.key == "rand_loot")
			items[id].rand_loot = toInt(infile.val);
		else if (infile.key == "rand_vendor")
			items[id].rand_vendor = toInt(infile.val);
		else if (infile.key == "pickup_status")
			items[id].pickup_status = infile.val;
		else if (infile.key == "stepfx")
			items[id].stepfx = infile.val;
		else if (infile.key == "class") {
			string classname = infile.nextValue();
			while (classname != "") {
				unsigned pos; // find the position where this classname is stored:
				for (pos = 0; pos < item_class_names.size(); pos++) {
					if (item_class_names[pos] == classname)
						break;
				}
				// if it was not found, add it to the end.
				// pos is already the correct index.
				if (pos == item_class_names.size()) {
					item_class_names.push_back(classname);
					item_class_items.push_back(vector<unsigned int>());
				}
				// add item id to the item list of that class:
				item_class_items[pos].push_back(id);
				classname = infile.nextValue();
			}
		}

	}
	infile.close();
}

void ItemManager::loadTypes(const string& filename) {
	FileParser infile;
	string type,description;
	type = description = "";

	if (infile.open(filename)) {
		while (infile.next()) {
			if (infile.key == "name") type = infile.val;
			else if (infile.key == "description") description = infile.val;

			if (type != "" && description != "") {
				item_types[type] = description;
				type = description = "";
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open %s!\n", filename.c_str());
}

string ItemManager::getItemType(std::string _type) {
	map<string,string>::iterator it,end;
	for (it=item_types.begin(), end=item_types.end(); it!=end; ++it) {
		if (_type.compare(it->first) == 0) return it->second;
	}
	// If all else fails, return the original string
	return _type;
}

void ItemManager::loadSets(const string& filename) {
	FileParser infile;
	if (!infile.open(filename)) {
		fprintf(stderr, "Unable to open %s!\n", filename.c_str());
		return;
	}

	int id = 0;
	bool id_line;
	while (infile.next()) {
		if (infile.key == "id") {
			id_line = true;
			id = toInt(infile.val);
			ensureFitsId(item_sets, id+1);
		} else id_line = false;

		if (id < 1) {
			if (id_line) fprintf(stderr, "Item set index out of bounds 1-%d, skipping\n", INT_MAX);
			continue;
		}
		if (id_line) continue;

		assert(item_sets.size() > std::size_t(id));

		if (infile.key == "name") {
			item_sets[id].name = msg->get(infile.val);
		}
		else if (infile.key == "items") {
			string item_id = infile.nextValue();
			while (item_id != "") {
				int temp_id = toInt(item_id);
				if (temp_id > 0 && temp_id < static_cast<int>(items.size())) {
					items[temp_id].set = id;
					item_sets[id].items.push_back(temp_id);
				} else {
					const int maxsize = static_cast<int>(items.size()-1);
					const char* cname = item_sets[id].name.c_str();
					fprintf(stderr, "Item index inside item set %s definition out of bounds 1-%d, skipping item\n", cname, maxsize);
				}
				item_id = infile.nextValue();
			}
		}
		else if (infile.key == "color") {
			item_sets[id].color.r = toInt(infile.nextValue());
			item_sets[id].color.g = toInt(infile.nextValue());
			item_sets[id].color.b = toInt(infile.nextValue());
		}
		else if (infile.key == "bonus") {
			Set_bonus bonus;
			bonus.requirement = toInt(infile.nextValue());
			bonus.bonus_stat = infile.nextValue();
			bonus.bonus_val = toInt(infile.nextValue());
			item_sets[id].bonus.push_back(bonus);
		}
	}
	infile.close();
}

/**
 * Icon sets
 */
void ItemManager::loadIcons() {

	icons = IMG_Load(mods->locate("images/icons/icons.png").c_str());

	if (!icons) {
		fprintf(stderr, "Couldn't load icons: %s\n", IMG_GetError());
	} else {
		// optimize
		SDL_Surface *cleanup = icons;
		icons = SDL_DisplayFormatAlpha(icons);
		SDL_FreeSurface(cleanup);
	}
}

/**
 * Renders icons at small size or large size
 * Also display the stack size
 */
void ItemManager::renderIcon(ItemStack stack, int x, int y, int size) {
	if (!icons) return;

	dest.x = x;
	dest.y = y;
	src.w = src.h = dest.w = dest.h = size;

	if (stack.item > 0) {
		int columns = icons->w / ICON_SIZE;
		src.x = (items[stack.item].icon % columns) * size;
		src.y = (items[stack.item].icon / columns) * size;
		SDL_BlitSurface(icons, &src, screen, &dest);
	}

	if (stack.quantity > 1 || items[stack.item].max_quantity > 1) {
		// stackable item : show the quantity
		stringstream ss;
		ss << stack.quantity;

		WidgetLabel label;
		label.set(dest.x + 2, dest.y + 2, JUSTIFY_LEFT, VALIGN_TOP, ss.str(), color_normal);
		label.render();
	}
}

void ItemManager::playSound(int item, Point pos) {
  snd->play(items[item].sfx, GLOBAL_VIRTUAL_CHANNEL, pos, false);
}

TooltipData ItemManager::getShortTooltip(ItemStack stack) {
	stringstream ss;
	TooltipData tip;
	SDL_Color color = color_normal;

	if (stack.item == 0) return tip;

	// color quality
	if (items[stack.item].set > 0) {
		color = item_sets[items[stack.item].set].color;
	}
	else if (items[stack.item].quality == ITEM_QUALITY_LOW) {
		color = color_low;
	}
	else if (items[stack.item].quality == ITEM_QUALITY_HIGH) {
		color = color_high;
	}
	else if (items[stack.item].quality == ITEM_QUALITY_EPIC) {
		color = color_epic;
	}

	// name
	if (stack.quantity > 1) {
		ss << stack.quantity << " " << items[stack.item].name;
	} else {
		ss << items[stack.item].name;
	}
	tip.addText(ss.str(), color);

	return tip;
}

/**
 * Create detailed tooltip showing all relevant item info
 */
TooltipData ItemManager::getTooltip(int item, StatBlock *stats, int context) {
	TooltipData tip;
	SDL_Color color = color_normal;

	if (item == 0) return tip;

	// color quality
	if (items[item].set > 0) {
		color = item_sets[items[item].set].color;
	}
	else if (items[item].quality == ITEM_QUALITY_LOW) {
		color = color_low;
	}
	else if (items[item].quality == ITEM_QUALITY_HIGH) {
		color = color_high;
	}
	else if (items[item].quality == ITEM_QUALITY_EPIC) {
		color = color_epic;
	}

	// name
	tip.addText(items[item].name, color);

	// level
	if (items[item].level != 0) {
		tip.addText(msg->get("Level %d", items[item].level));
	}

	// type
	if (items[item].type != "other") {
		tip.addText(msg->get(getItemType(items[item].type)));
	}

	// damage
	if (items[item].dmg_melee_max > 0) {
		if (items[item].dmg_melee_min < items[item].dmg_melee_max)
			tip.addText(msg->get("Melee damage: %d-%d", items[item].dmg_melee_min, items[item].dmg_melee_max));
		else
			tip.addText(msg->get("Melee damage: %d", items[item].dmg_melee_max));
	}
	if (items[item].dmg_ranged_max > 0) {
		if (items[item].dmg_ranged_min < items[item].dmg_ranged_max)
			tip.addText(msg->get("Ranged damage: %d-%d", items[item].dmg_ranged_min, items[item].dmg_ranged_max));
		else
			tip.addText(msg->get("Ranged damage: %d", items[item].dmg_ranged_max));
	}
	if (items[item].dmg_ment_max > 0) {
		if (items[item].dmg_ment_min < items[item].dmg_ment_max)
			tip.addText(msg->get("Mental damage: %d-%d", items[item].dmg_ment_min, items[item].dmg_ment_max));
		else
			tip.addText(msg->get("Mental damage: %d", items[item].dmg_ment_max));
	}

	// absorb
	if (items[item].abs_max > 0) {
		if (items[item].abs_min < items[item].abs_max)
			tip.addText(msg->get("Absorb: %d-%d", items[item].abs_min, items[item].abs_max));
		else
			tip.addText(msg->get("Absorb: %d", items[item].abs_max));
	}

	// bonuses
	unsigned bonus_counter = 0;
	string modifier;
	while (bonus_counter < items[item].bonus_val.size() && items[item].bonus_stat[bonus_counter] != "") {
		if (items[item].bonus_stat[bonus_counter] == "speed") {
			modifier = msg->get("%d%% Speed", items[item].bonus_val[bonus_counter]);
			if (items[item].bonus_val[bonus_counter] >= 100) color = color_bonus;
			else color = color_penalty;
		} else {
			if (items[item].bonus_val[bonus_counter] > 0) {
				modifier = msg->get("Increases %s by %d",
						items[item].bonus_val[bonus_counter],
						msg->get(items[item].bonus_stat[bonus_counter]));

				color = color_bonus;
			}
			else {
				modifier = msg->get("Decreases %s by %d",
						items[item].bonus_val[bonus_counter],
						msg->get(items[item].bonus_stat[bonus_counter]));

				color = color_penalty;
			}
		}
		tip.addText(modifier, color);
		bonus_counter++;
	}

	// power
	if (items[item].power_desc != "") {
		tip.addText(items[item].power_desc, color_bonus);
	}

	// requirement
	if (items[item].req_val > 0) {
		if (items[item].req_stat == REQUIRES_PHYS) {
			if (stats->get_physical() < items[item].req_val) color = color_requirements_not_met;
			else color = color_normal;
			tip.addText(msg->get("Requires Physical %d", items[item].req_val), color);
		}
		else if (items[item].req_stat == REQUIRES_MENT) {
			if (stats->get_mental() < items[item].req_val) color = color_requirements_not_met;
			else color = color_normal;
			tip.addText(msg->get("Requires Mental %d", items[item].req_val), color);
		}
		else if (items[item].req_stat == REQUIRES_OFF) {
			if (stats->get_offense() < items[item].req_val) color = color_requirements_not_met;
			else color = color_normal;
			tip.addText(msg->get("Requires Offense %d", items[item].req_val), color);
		}
		else if (items[item].req_stat == REQUIRES_DEF) {
			if (stats->get_defense() < items[item].req_val) color = color_requirements_not_met;
			else color = color_normal;
			tip.addText(msg->get("Requires Defense %d", items[item].req_val), color);
		}
	}

	// flavor text
	if (items[item].flavor != "") {
		tip.addText(items[item].flavor, color_flavor);
	}

	// buy or sell price
	if (items[item].price > 0) {

		int price_per_unit;
		if (context == VENDOR_BUY) {
			price_per_unit = items[item].price;
			if (stats->currency < items[item].price) color = color_requirements_not_met;
			else color = color_normal;
			if (items[item].max_quantity <= 1)
				tip.addText(msg->get("Buy Price: %d %s", price_per_unit, CURRENCY), color);
			else
				tip.addText(msg->get("Buy Price: %d %s each", price_per_unit, CURRENCY), color);
		} else if (context == VENDOR_SELL) {
			price_per_unit = items[item].getSellPrice();
			if (stats->currency < price_per_unit) color = color_requirements_not_met;
			else color = color_normal;
			if (items[item].max_quantity <= 1)
				tip.addText(msg->get("Buy Price: %d %s", price_per_unit, CURRENCY), color);
			else
				tip.addText(msg->get("Buy Price: %d %s each", price_per_unit, CURRENCY), color);
		} else if (context == PLAYER_INV) {
			price_per_unit = items[item].getSellPrice();
			if (price_per_unit == 0) price_per_unit = 1;
			if (items[item].max_quantity <= 1)
				tip.addText(msg->get("Sell Price: %d %s", price_per_unit, CURRENCY));
			else
				tip.addText(msg->get("Sell Price: %d %s each", price_per_unit, CURRENCY));
		}
	}

	if (items[item].set > 0) {
			// item set bonuses
			ItemSet set = item_sets[items[item].set];
			bonus_counter = 0;
			modifier = "";

			tip.addText("\n" + msg->get("Set: ") + msg->get(item_sets[items[item].set].name), set.color);

			while (bonus_counter < set.bonus.size() && set.bonus[bonus_counter].bonus_stat != "") {
				if (set.bonus[bonus_counter].bonus_val > 0) {
					modifier = msg->get("%d items: ", set.bonus[bonus_counter].requirement) + msg->get("Increases %s by %d", set.bonus[bonus_counter].bonus_val, msg->get(set.bonus[bonus_counter].bonus_stat));
				}
				else {
					modifier = msg->get("%d items: ", set.bonus[bonus_counter].requirement) + msg->get("Decreases %s by %d", set.bonus[bonus_counter].bonus_val, msg->get(set.bonus[bonus_counter].bonus_stat));
				}
				tip.addText(modifier, set.color);
				bonus_counter++;
			}
	}

	return tip;
}

ItemManager::~ItemManager() {
	SDL_FreeSurface(icons);
}

/**
 * Compare two item stack to be able to sorting them on their item_id in the vendors' stock
 */
bool ItemStack::operator > (const ItemStack &param) const {
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

int Item::getSellPrice() {
	int new_price = 0;
	if (price_sell != 0)
		new_price = price_sell;
	else
		new_price = static_cast<int>(price * VENDOR_RATIO);
	if (new_price == 0) new_price = 1;

	return new_price;
}

