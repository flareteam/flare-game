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
 * class MenuInventory
 */

#include "FileParser.h"
#include "LootManager.h"
#include "Menu.h"
#include "MenuInventory.h"
#include "PowerManager.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"

#include <sstream>

using namespace std;


MenuInventory::MenuInventory(ItemManager *_items, StatBlock *_stats, PowerManager *_powers) {
	items = _items;
	stats = _stats;
	powers = _powers;
	MAX_EQUIPPED = 4;
	MAX_CARRIED = 64;
	visible = false;
	loadGraphics();

	currency = 0;

	drag_prev_src = -1;
	changed_equipment = true;
	changed_artifact = true;
	log_msg = "";

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// Load config settings
	SDL_Rect equipment_slot;
	FileParser infile;
	if(infile.open(mods->locate("menus/inventory.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "equipment_slot") {
				equipment_slot.x = eatFirstInt(infile.val,',');
				equipment_slot.y = eatFirstInt(infile.val,',');
				equipment_slot.w = equipment_slot.h = eatFirstInt(infile.val,',');
				equipped_area.push_back(equipment_slot);
				slot_type.push_back(eatFirstString(infile.val,','));
			} else if(infile.key == "slot_name") {
				slot_desc.push_back(eatFirstString(infile.val,','));
			} else if(infile.key == "carried_area") {
				carried_area.x = eatFirstInt(infile.val,',');
				carried_area.y = eatFirstInt(infile.val,',');
			} else if (infile.key == "carried_cols"){
				carried_cols = eatFirstInt(infile.val,',');
			} else if (infile.key == "carried_rows"){
				carried_rows = eatFirstInt(infile.val,',');
			} else if (infile.key == "caption"){
				title =  eatLabelInfo(infile.val);
			} else if (infile.key == "currency"){
				currency_lbl =  eatLabelInfo(infile.val);
			} else if (infile.key == "help"){
				help_pos.x = eatFirstInt(infile.val,',');
				help_pos.y = eatFirstInt(infile.val,',');
				help_pos.w = eatFirstInt(infile.val,',');
				help_pos.h = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/inventory.txt!\n");

	MAX_EQUIPPED = equipped_area.size();
	MAX_CARRIED = carried_cols * carried_rows;

	color_normal = font->getColor("menu_normal");
	color_high = font->getColor("menu_bonus");
}

void MenuInventory::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/inventory.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
	} else {
		// optimize
		SDL_Surface *cleanup = background;
		background = SDL_DisplayFormatAlpha(background);
		SDL_FreeSurface(cleanup);
	}
}

void MenuInventory::update() {
	for (int i=0; i<MAX_EQUIPPED; i++) {
		equipped_area[i].x += window_area.x;
		equipped_area[i].y += window_area.y;
	}

	carried_area.x += window_area.x;
	carried_area.y += window_area.y;
	carried_area.w = carried_cols*ICON_SIZE;
	carried_area.h = carried_rows*ICON_SIZE;

	inventory[EQUIPMENT].init(MAX_EQUIPPED, items, equipped_area, slot_type);
	inventory[CARRIED].init(MAX_CARRIED, items, carried_area, ICON_SIZE, carried_cols);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;
}

void MenuInventory::logic() {

	// if the player has just died, the penalty is half his current currency.
	if (stats->death_penalty) {
		currency = currency/2;
		stats->death_penalty = false;
	}

	// a copy of currency is kept in stats, to help with various situations
	stats->currency = currency;

	// check close button
	if (visible) {
		if (closeButton->checkClick()) {
			visible = false;
			snd->play(sfx_close);
		}
		if (drag_prev_src == -1) {
			clearHighlight();
		}
	}
}

void MenuInventory::render() {
	if (!visible) return;

	// background
	SDL_Rect dest = window_area;
	SDL_BlitSurface(background, NULL, screen, &dest);

	// close button
	closeButton->render();

	// text overlay
	WidgetLabel label;
	if (!title.hidden) {
		label.set(window_area.x+title.x, window_area.y+title.y, title.justify, title.valign, msg->get("Inventory"), color_normal, title.font_style);
		label.render();
	}
	if (!currency_lbl.hidden) {
		label.set(window_area.x+currency_lbl.x, window_area.y+currency_lbl.y, currency_lbl.justify, currency_lbl.valign, msg->get("%d %s", currency, CURRENCY), color_normal, currency_lbl.font_style);
		label.render();
	}

	inventory[EQUIPMENT].render();
	inventory[CARRIED].render();
}

int MenuInventory::areaOver(Point mouse) {
	if (isWithin(carried_area, mouse)) {
		return CARRIED;
	} else {
		for (unsigned int i=0; i<equipped_area.size(); i++) {
			if (isWithin(equipped_area[i], mouse)) {
				return EQUIPMENT;
			}
		}
	}
	return -1;
}

/**
 * If mousing-over an item with a tooltip, return that tooltip data.
 *
 * @param mouse The x,y screen coordinates of the mouse cursor
 */
TooltipData MenuInventory::checkTooltip(Point mouse) {
	int area;
	int slot;
	TooltipData tip;

	area = areaOver(mouse);
	if (area == -1) {
		if (mouse.x >= window_area.x + help_pos.x && mouse.y >= window_area.y+help_pos.y && mouse.x < window_area.x+help_pos.x+help_pos.w && mouse.y < window_area.y+help_pos.y+help_pos.h) {
			tip.addText(msg->get("Use SHIFT to move only one item."));
			tip.addText(msg->get("CTRL-click a carried item to sell it."));
		}
		return tip;
	}
	slot = inventory[area].slotOver(mouse);

	if (slot == -1)
		return tip;

	if (inventory[area][slot].item > 0) {
		tip = inventory[area].checkTooltip( mouse, stats, PLAYER_INV);
	}
	else if (area == EQUIPMENT && inventory[area][slot].item == 0) {
		tip.addText(msg->get(slot_desc[slot]));
	}

	return tip;
}

/**
 * Click-start dragging in the inventory
 */
ItemStack MenuInventory::click(InputState * input) {
	ItemStack item;
	item.item = 0;
	item.quantity = 0;

	drag_prev_src = areaOver(input->mouse);
	if( drag_prev_src > -1) {
		item = inventory[drag_prev_src].click(input);
		// if dragging equipment, prepare to change stats/sprites
		if (drag_prev_src == EQUIPMENT) {
			if (stats->humanoid) {
				updateEquipment( inventory[EQUIPMENT].drag_prev_slot);
			} else {
				itemReturn(item);
				item.item = 0;
				item.quantity = 0;
			}
		} else if (drag_prev_src == CARRIED && !inpt->pressing[CTRL] && !inpt->pressing[MAIN2]) {
			inventory[EQUIPMENT].highlightMatching(items->items[item.item].type);
		}
	}

	return item;
}

/**
 * Return dragged item to previous slot
 */
void MenuInventory::itemReturn( ItemStack stack) {
	if (drag_prev_src == -1) return;
	inventory[drag_prev_src].itemReturn( stack);
	// if returning equipment, prepare to change stats/sprites
	if (drag_prev_src == EQUIPMENT) {
		updateEquipment(inventory[EQUIPMENT].drag_prev_slot);
	}
	drag_prev_src = -1;
}

/**
 * Dragging and dropping an item can be used to rearrange the inventory
 * and equip items
 */
void MenuInventory::drop(Point mouse, ItemStack stack) {
	items->playSound(stack.item);

	int area = areaOver(mouse);
	if (area == -1) {
		// not dropped into a slot. Just return it to the previous slot.
		itemReturn(stack);
		return;
	}

	int slot = inventory[area].slotOver(mouse);
	if (slot == -1) {
		// not dropped into a slot. Just return it to the previous slot.
		itemReturn(stack);
		return;
	}

	int drag_prev_slot = inventory[drag_prev_src].drag_prev_slot;

	if (area == EQUIPMENT) { // dropped onto equipped item

		// make sure the item is going to the correct slot
		// note: equipment slots 0-3 correspond with item types 0-3
		// also check to see if the hero meets the requirements
		if (drag_prev_src == CARRIED && slot_type[slot] == items->items[stack.item].type && requirementsMet(stack.item) && stats->humanoid) {
			if( inventory[area][slot].item == stack.item) {
				// Merge the stacks
				add( stack, area, slot);
			}
			else if( inventory[drag_prev_src][drag_prev_slot].item == 0) {
				// Swap the two stacks
				itemReturn( inventory[area][slot]);
				inventory[area][slot] = stack;
				updateEquipment( slot);
			} else {
				itemReturn( stack);
			}
		}
		else {
			// equippable items only belong to one slot, for the moment
			itemReturn( stack); // cancel
		}
	}
	else if (area == CARRIED) {
		// dropped onto carried item

		if (drag_prev_src == CARRIED) {
			if (slot != drag_prev_slot) {
				if( inventory[area][slot].item == stack.item) {
					// Merge the stacks
					add( stack, area, slot);
				}
				else if( inventory[area][slot].item == 0) {
					// Drop the stack
					inventory[area][slot] = stack;
				}
				else if( inventory[drag_prev_src][drag_prev_slot].item == 0) { // Check if the previous slot is free (could still be used if SHIFT was used).
					// Swap the two stacks
					itemReturn( inventory[area][slot]);
					inventory[area][slot] = stack;
				} else {
					itemReturn( stack);
				}
			}
			else {
				itemReturn( stack); // cancel
			}
		}
		else {
			// note: equipment slots 0-3 correspond with item types 0-3
			// also check to see if the hero meets the requirements
			if (inventory[area][slot].item == stack.item || drag_prev_src == -1) {
				// Merge the stacks
				add( stack, area, slot);
			}
			else if( inventory[area][slot].item == 0) {
				// Drop the stack
				inventory[area][slot] = stack;
			}
			else if(
				inventory[EQUIPMENT][drag_prev_slot].item == 0
				&& inventory[CARRIED][slot].item != stack.item
				&& items->items[inventory[CARRIED][slot].item].type == slot_type[drag_prev_slot]
				&& requirementsMet(inventory[CARRIED][slot].item)
			) { // The whole equipped stack is dropped on an empty carried slot or on a wearable item
				// Swap the two stacks
				itemReturn( inventory[area][slot]);
				inventory[area][slot] = stack;
			}
			else {
				itemReturn( stack); // cancel
			}
		}
	}

	drag_prev_src = -1;
}

/**
 * Right-clicking on a usable item in the inventory causes it to activate.
 * e.g. drink a potion
 * e.g. equip an item
 */
void MenuInventory::activate(InputState * input) {
	ItemStack stack;
	Point nullpt;
	nullpt.x = nullpt.y = 0;

	// clicked a carried item
	int slot = inventory[CARRIED].slotOver(input->mouse);
	if (slot == -1)
		return;

	// can't interact with quest items
	if (items->items[inventory[CARRIED][slot].item].type == "quest") {
		return;
	}
	// use a consumable item
	else if (items->items[inventory[CARRIED][slot].item].type == "consumable") {

		//don't use untransform item if hero is not transformed
		if (powers->powers[items->items[inventory[CARRIED][slot].item].power].spawn_type == "untransform" && !stats->transformed) return;

		//check for power cooldown
		if (stats->hero_cooldown[items->items[inventory[CARRIED][slot].item].power] > 0) return;
		else stats->hero_cooldown[items->items[inventory[CARRIED][slot].item].power] = powers->powers[items->items[inventory[CARRIED][slot].item].power].cooldown;

		// if this item requires targeting it can't be used this way
		if (!powers->powers[items->items[inventory[CARRIED][slot].item].power].requires_targeting) {

			unsigned used_item_count = powers->used_items.size();
			unsigned used_equipped_item_count = powers->used_equipped_items.size();
			powers->activate(items->items[inventory[CARRIED][slot].item].power, stats, nullpt);
			// Remove any used items from the queue of items to be removed. We will destroy the items here.
			if (used_item_count < powers->used_items.size()) powers->used_items.pop_back();
			if (used_equipped_item_count < powers->used_equipped_items.size()) powers->used_equipped_items.pop_back();
			inventory[CARRIED].substract(slot);
		}
		else {
			// let player know this can only be used from the action bar
			log_msg = msg->get("This item can only be used from the action bar.");
		}

	}
	// equip an item
	else if (stats->humanoid) {
		int equip_slot = -1;
		// find first empty(or just first) slot for item to equip
		for (int i = 0; i < MAX_EQUIPPED; i++) {
			// first check for first empty
			if ((slot_type[i] == items->items[inventory[CARRIED][slot].item].type) &&
				(inventory[EQUIPMENT].storage[i].item == 0)) {
				equip_slot = i;
			}
		}
		if (equip_slot == -1) {
			// if empty not found, use just first
			for (int i = 0; i < MAX_EQUIPPED; i++) {
				if (slot_type[i] == items->items[inventory[CARRIED][slot].item].type) {
					equip_slot = i;
					break;
				}
			}
		}

		if (equip_slot != -1) {
			if (requirementsMet(inventory[CARRIED][slot].item)) {
				stack = click( input);
				if( inventory[EQUIPMENT][equip_slot].item == stack.item) {
					// Merge the stacks
					add( stack, EQUIPMENT, equip_slot);
				}
				else if( inventory[EQUIPMENT][equip_slot].item == 0) {
					// Drop the stack
					inventory[EQUIPMENT][equip_slot] = stack;
				}
				else {
					if( inventory[CARRIED][slot].item == 0) { // Don't forget this slot may have been emptied by the click()
						// Swap the two stacks
						itemReturn( inventory[EQUIPMENT][equip_slot]);
					}
					else {
						// Drop the equipped item anywhere
						add( inventory[EQUIPMENT][equip_slot]);
					}
					inventory[EQUIPMENT][equip_slot] = stack;
				}
				updateEquipment( equip_slot);
				items->playSound(inventory[EQUIPMENT][equip_slot].item);
			}
		} else fprintf(stderr, "Can't find equip slot, corresponding to type %s\n", items->items[inventory[CARRIED][slot].item].type.c_str());
	}

	drag_prev_src = -1;
}

/**
 * Insert item into first available carried slot, preferably in the optionnal specified slot
 *
 * @param ItemStack Stack of items
 * @param area Area number where it will try to store the item
 * @param slot Slot number where it will try to store the item
 */
void MenuInventory::add(ItemStack stack, int area, int slot) {
	items->playSound(stack.item);

	if (stack.item != 0) {
		if (area < 0) {
			area = CARRIED;
		}
		int max_quantity = items->items[stack.item].max_quantity;
		if (slot > -1 && inventory[area][slot].item != 0 && inventory[area][slot].item != stack.item) {
			// the proposed slot isn't available, search for another one
			slot = -1;
		}
		if (area == CARRIED) {
			// first search of stack to complete if the item is stackable
			if (slot == -1 && max_quantity > 1) {
				int i = 0;
				while ((inventory[area][i].item != stack.item
						|| inventory[area][i].quantity >= max_quantity)
						&& i < MAX_CARRIED)
					++i;
				if (i < MAX_CARRIED)
					slot = i;
			}
			// then an empty slot
			if (slot == -1) {
				int i = 0;
				while (inventory[area][i].item != 0 && i < MAX_CARRIED)
					i++;
				if (i < MAX_CARRIED)
					slot = i;
			}
		}
		if (slot != -1) {
			// Add
			int quantity_added = min( stack.quantity, max_quantity - inventory[area][slot].quantity);
			inventory[area][slot].item = stack.item;
			inventory[area][slot].quantity += quantity_added;
			stack.quantity -= quantity_added;
			// Add back the remaining
			if( stack.quantity > 0) {
				if( drag_prev_src > -1) {
					itemReturn( stack);
				} else {
					add( stack);
				}
			}
		}
		else {
			// No available slot, drop
		}
	}
}

/**
 * Remove one given item from the player's inventory.
 */
void MenuInventory::remove(int item) {
	if( !inventory[CARRIED].remove(item)) {
		inventory[EQUIPMENT].remove(item);
		applyEquipment(inventory[EQUIPMENT].storage);
	}
}

/**
 * Remove an equipped item from the player's inventory.
 */
void MenuInventory::removeEquipped(int item) {
	inventory[EQUIPMENT].remove(item);
	applyEquipment(inventory[EQUIPMENT].storage);
}

/**
 * Add currency to the current total
 */
void MenuInventory::addCurrency(int count) {
	currency += count;
	LootManager::getInstance()->playCurrencySound();
}

/**
 * Check if there is enough currency to buy the given stack, and if so remove it from the current total and add the stack.
 * (Handle the drop into the equipment area, but add() don't handle it well in all circonstances. MenuManager::logic() allow only into the carried area.)
 */
bool MenuInventory::buy(ItemStack stack, int tab) {
	int value_each;
	if (tab == VENDOR_BUY) value_each = items->items[stack.item].price;
	else value_each = items->items[stack.item].getSellPrice();

	int count = value_each * stack.quantity;
	if( currency >= count) {
		currency -= count;

		LootManager::getInstance()->playCurrencySound();
		return true;
	}
	else {
		return false;
	}
}

/**
 * Similar to sell(), but for use with stash
 */
bool MenuInventory::stashAdd(ItemStack stack) {
	// items that have no price cannot be stored
	if (items->items[stack.item].price == 0) return false;

	drag_prev_src = -1;
	return true;
}
/**
 * Sell a specific stack of items
 */
bool MenuInventory::sell(ItemStack stack) {
	// items that have no price cannot be sold
	if (items->items[stack.item].price == 0) return false;

	int value_each = items->items[stack.item].getSellPrice();
	int value = value_each * stack.quantity;
	currency += value;
	LootManager::getInstance()->playCurrencySound();
	drag_prev_src = -1;
	return true;
}

/**
 * Cannot pick up new items if the inventory is full.
 * Full means no more carrying capacity (equipped capacity is ignored)
 */
bool MenuInventory::full(int item) {
	return inventory[CARRIED].full(item);
}

/**
 * Get the number of the specified item carried (not equipped)
 */
int MenuInventory::getItemCountCarried(int item) {
	return inventory[CARRIED].count(item);
}

/**
 * Check to see if the given item is equipped
 */
bool MenuInventory::isItemEquipped(int item) {
	return inventory[EQUIPMENT].contain(item);
}

/**
 * Check requirements on an item
 */
bool MenuInventory::requirementsMet(int item) {
	if (items->items[item].req_stat == REQUIRES_PHYS) {
		return (stats->get_physical() >= items->items[item].req_val);
	}
	else if (items->items[item].req_stat == REQUIRES_MENT) {
		return (stats->get_mental() >= items->items[item].req_val);
	}
	else if (items->items[item].req_stat == REQUIRES_OFF) {
		return (stats->get_offense() >= items->items[item].req_val);
	}
	else if (items->items[item].req_stat == REQUIRES_DEF) {
		return (stats->get_defense() >= items->items[item].req_val);
	}
	// otherwise there is no requirement, so it is usable.
	return true;
}

void MenuInventory::updateEquipment(int slot) {

	if (slot == -1) {
		//FIXME What todo here
		//return;
		changed_equipment = true;
	}
	else if (slot_type[slot] != "artifact") {
		changed_equipment = true;
	}
	else {
		changed_artifact = true;
	}
}

/**
 * Given the equipped items, calculate the hero's stats
 */
void MenuInventory::applyEquipment(ItemStack *equipped) {

	unsigned bonus_counter;

	const vector<Item> &pc_items = items->items;
	int item_id;

	// calculate bonuses to basic stats, added by items
	bool checkRequired = true;
	while(checkRequired)
	{
		checkRequired = false;
		stats->offense_additional = stats->defense_additional = stats->physical_additional = stats->mental_additional = 0;
		for (int i = 0; i < MAX_EQUIPPED; i++) {
			item_id = equipped[i].item;
			const Item &item = pc_items[item_id];
			bonus_counter = 0;
			while (bonus_counter < item.bonus_stat.size() && item.bonus_stat[bonus_counter] != "") {
				if (item.bonus_stat[bonus_counter] == "offense")
					stats->offense_additional += item.bonus_val[bonus_counter];
				else if (item.bonus_stat[bonus_counter] == "defense")
					stats->defense_additional += item.bonus_val[bonus_counter];
				else if (item.bonus_stat[bonus_counter] == "physical")
					stats->physical_additional += item.bonus_val[bonus_counter];
				else if (item.bonus_stat[bonus_counter] == "mental")
					stats->mental_additional += item.bonus_val[bonus_counter];
				else if (item.bonus_stat[bonus_counter] == "all basic stats") {
					stats->offense_additional += item.bonus_val[bonus_counter];
					stats->defense_additional += item.bonus_val[bonus_counter];
					stats->physical_additional += item.bonus_val[bonus_counter];
					stats->mental_additional += item.bonus_val[bonus_counter];
				}
				bonus_counter++;
			}
		}

		// calculate bonuses. added by item sets
		vector<int> set;
		vector<int> quantity;
		vector<int>::iterator it;
		bonus_counter = 0;

		for (int i=0; i<MAX_EQUIPPED; i++) {
			item_id = equipped[i].item;
			it = find(set.begin(), set.end(), items->items[item_id].set);
			if (items->items[item_id].set > 0 && it != set.end()) {
				quantity[distance(set.begin(), it)] += 1;
			}
			else if (items->items[item_id].set > 0) {
				set.push_back(items->items[item_id].set);
				quantity.push_back(1);
			}
		}
		// calculate bonuses to basic stats, added by item sets
		ItemSet temp_set;
		for (unsigned k=0; k<set.size(); k++) {
			temp_set = items->item_sets[set[k]];
			for (bonus_counter=0; bonus_counter<temp_set.bonus.size(); bonus_counter++) {
				if (temp_set.bonus[bonus_counter].requirement != quantity[k]) continue;

				if (temp_set.bonus[bonus_counter].bonus_stat == "offense")
					stats->offense_additional += temp_set.bonus[bonus_counter].bonus_val;
				else if (temp_set.bonus[bonus_counter].bonus_stat == "defense")
					stats->defense_additional += temp_set.bonus[bonus_counter].bonus_val;
				else if (temp_set.bonus[bonus_counter].bonus_stat == "physical")
					stats->physical_additional += temp_set.bonus[bonus_counter].bonus_val;
				else if (temp_set.bonus[bonus_counter].bonus_stat == "mental")
					stats->mental_additional += temp_set.bonus[bonus_counter].bonus_val;
				else if (temp_set.bonus[bonus_counter].bonus_stat == "all basic stats") {
					stats->offense_additional += temp_set.bonus[bonus_counter].bonus_val;
					stats->defense_additional += temp_set.bonus[bonus_counter].bonus_val;
					stats->physical_additional += temp_set.bonus[bonus_counter].bonus_val;
					stats->mental_additional += temp_set.bonus[bonus_counter].bonus_val;
				}
			}
		}
		// check that each equipped item fit requirements
		for (int i = 0; i < MAX_EQUIPPED; i++) {
			if (!requirementsMet(equipped[i].item)) {
				add(equipped[i]);
				equipped[i].item = 0;
				equipped[i].quantity = 0;
				checkRequired = true;
			}
		}
	}

	// defaults
	for (unsigned i=0; i<stats->powers_list_items.size(); ++i) {
		int id = stats->powers_list_items[i];
		if (powers->powers[id].passive)
			stats->effects.removeEffectPassive(id);
	}
	stats->powers_list_items.clear();

	// the default for weapons/absorb are not added to equipped items
	// later this function they are applied if the defaults aren't met
	stats->calcBaseDmgAndAbs();

	// reset wielding vars
	stats->wielding_physical = false;
	stats->wielding_mental = false;
	stats->wielding_offense = false;

	// remove all effects and bonuses added by items
	stats->effects.clearItemEffects();

	applyItemStats(equipped);
	applyItemSetBonuses(equipped);

	// increase damage and absorb to minimum amounts
	if (stats->dmg_melee_min < stats->dmg_melee_min_default)
		stats->dmg_melee_min = stats->dmg_melee_min_default;
	if (stats->dmg_melee_max < stats->dmg_melee_max_default)
		stats->dmg_melee_max = stats->dmg_melee_max_default;
	if (stats->dmg_ranged_min < stats->dmg_ranged_min_default)
		stats->dmg_ranged_min = stats->dmg_ranged_min_default;
	if (stats->dmg_ranged_max < stats->dmg_ranged_max_default)
		stats->dmg_ranged_max = stats->dmg_ranged_max_default;
	if (stats->dmg_ment_min < stats->dmg_ment_min_default)
		stats->dmg_ment_min = stats->dmg_ment_min_default;
	if (stats->dmg_ment_max < stats->dmg_ment_max_default)
		stats->dmg_ment_max = stats->dmg_ment_max_default;
	if (stats->absorb_min < stats->absorb_min_default)
		stats->absorb_min = stats->absorb_min_default;
	if (stats->absorb_max < stats->absorb_max_default)
		stats->absorb_max = stats->absorb_max_default;

	// update stat display
	stats->refresh_stats = true;
}

void MenuInventory::applyItemStats(ItemStack *equipped) {
	unsigned bonus_counter;
	const vector<Item> &pc_items = items->items;
	int item_id;

	// apply stats from all items
	for (int i=0; i<MAX_EQUIPPED; i++) {
		item_id = equipped[i].item;
		const Item &item = pc_items[item_id];

		// apply base stats
		stats->dmg_melee_min += item.dmg_melee_min;
		stats->dmg_melee_max += item.dmg_melee_max;
		stats->dmg_ranged_min += item.dmg_ranged_min;
		stats->dmg_ranged_max += item.dmg_ranged_max;
		stats->dmg_ment_min += item.dmg_ment_min;
		stats->dmg_ment_max += item.dmg_ment_max;

		// TODO: add a separate wielding stat to items
		// e.g. we might want a ring that gives bonus ranged damage but
		// we still need a bow to shoot arrows.
		if (item.dmg_melee_max > 0) {
			stats->wielding_physical = true;
			if (item.power_mod != 0) {
				stats->melee_weapon_power = item.power_mod;
			}
		}
		if (item.dmg_ranged_max > 0) {
			stats->wielding_offense = true;
			if (item.power_mod != 0) {
				stats->ranged_weapon_power = item.power_mod;
			}
		}
		if (item.dmg_ment_max > 0) {
			stats->wielding_mental = true;
			if (item.power_mod != 0) {
				stats->mental_weapon_power = item.power_mod;
			}
		}

		// apply absorb bonus
		stats->absorb_min += item.abs_min;
		stats->absorb_max += item.abs_max;

		// apply various bonuses
		bonus_counter = 0;
		while (bonus_counter < item.bonus_stat.size() && item.bonus_stat[bonus_counter] != "") {
			int id = powers->getIdFromTag(item.bonus_stat[bonus_counter]);

			if (id > 0)
				stats->effects.addEffect(id, powers->powers[id].icon, 0, item.bonus_val[bonus_counter], powers->powers[id].effect_type, powers->powers[id].animation_name, powers->powers[id].effect_additive, true, -1, powers->powers[id].effect_render_above, 0);

			bonus_counter++;
		}

		// add item powers
		if (item.power > 0) stats->powers_list_items.push_back(item.power);

	}
}

void MenuInventory::applyItemSetBonuses(ItemStack *equipped) {
	// calculate bonuses. added by item sets
	vector<int> set;
	vector<int> quantity;
	vector<int>::iterator it;
	unsigned bonus_counter = 0;
	int item_id;

	for (int i=0; i<MAX_EQUIPPED; i++) {
		item_id = equipped[i].item;
		it = find(set.begin(), set.end(), items->items[item_id].set);
		if (items->items[item_id].set > 0 && it != set.end()) {
			quantity[distance(set.begin(), it)] += 1;
		}
		else if (items->items[item_id].set > 0) {
			set.push_back(items->items[item_id].set);
			quantity.push_back(1);
		}
	}
	// apply item set bonuses
	ItemSet temp_set;
	for (unsigned k=0; k<set.size(); k++) {
		temp_set = items->item_sets[set[k]];
		for (bonus_counter=0; bonus_counter<temp_set.bonus.size(); bonus_counter++) {
			if (temp_set.bonus[bonus_counter].requirement != quantity[k]) continue;

			int id = powers->getIdFromTag(temp_set.bonus[bonus_counter].bonus_stat);

			if (id > 0)
				stats->effects.addEffect(id, powers->powers[id].icon, 0, temp_set.bonus[bonus_counter].bonus_val, powers->powers[id].effect_type, powers->powers[id].animation_name, powers->powers[id].effect_additive, true, -1, powers->powers[id].effect_render_above, 0);
		}
	}
}

void MenuInventory::clearHighlight() {
	inventory[EQUIPMENT].highlightClear();
	inventory[CARRIED].highlightClear();
}

MenuInventory::~MenuInventory() {
	SDL_FreeSurface(background);
	delete closeButton;
}
