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
 * class MenuInventory
 */

#include "FileParser.h"
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
	MAX_CARRIED = 64;
	visible = false;
	loadGraphics();

	gold = 0;

	drag_prev_src = -1;
	changed_equipment = true;
	changed_artifact = true;
	log_msg = "";

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/inventory.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "equipped_area") {
				equipped_area.x = eatFirstInt(infile.val,',');
				equipped_area.y = eatFirstInt(infile.val,',');
				equipped_area.w = eatFirstInt(infile.val,',');
				equipped_area.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "carried_area") {
				carried_area.x = eatFirstInt(infile.val,',');
				carried_area.y = eatFirstInt(infile.val,',');
			} else if (infile.key == "carried_cols"){
				carried_cols = eatFirstInt(infile.val,',');
			} else if (infile.key == "carried_rows"){
				carried_rows = eatFirstInt(infile.val,',');
			} else if (infile.key == "title"){
				title =  eatLabelInfo(infile.val);
			} else if (infile.key == "main_hand_label"){
				main_lbl =  eatLabelInfo(infile.val);
			} else if (infile.key == "body_label"){
				body_lbl =  eatLabelInfo(infile.val);
			} else if (infile.key == "off_hand_label"){
				off_lbl =  eatLabelInfo(infile.val);
			} else if (infile.key == "artifact_label"){
				artifact_lbl =  eatLabelInfo(infile.val);
			} else if (infile.key == "gold"){
				gold_lbl =  eatLabelInfo(infile.val);
			} else if (infile.key == "help"){
				help_pos.x = eatFirstInt(infile.val,',');
				help_pos.y = eatFirstInt(infile.val,',');
				help_pos.w = eatFirstInt(infile.val,',');
				help_pos.h = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open inventory.txt!\n");

	MAX_CARRIED = carried_cols * carried_rows;

	color_normal = font->getColor("menu_normal");
}

void MenuInventory::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/inventory.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);
}

void MenuInventory::update() {
	equipped_area.x += window_area.x;
	equipped_area.y += window_area.y;

	carried_area.x += window_area.x;
	carried_area.y += window_area.y;
	carried_area.w = carried_cols*ICON_SIZE_SMALL;
	carried_area.h = carried_rows*ICON_SIZE_SMALL;

	inventory[EQUIPMENT].init(MAX_EQUIPPED, items, equipped_area, ICON_SIZE_LARGE, 4);
	inventory[CARRIED].init(MAX_CARRIED, items, carried_area, ICON_SIZE_SMALL, carried_cols);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;
}

void MenuInventory::logic() {

	// if the player has just died, the penalty is half his current gold.
	if (stats->death_penalty) {
		gold = gold/2;
		stats->death_penalty = false;
	}

	// a copy of gold is kept in stats, to help with various situations
	stats->gold = gold;

	// check close button
	if (visible) {
		if (closeButton->checkClick()) {
			visible = false;
		}
	}
}

void MenuInventory::render() {
	if (!visible) return;

	SDL_Rect src;

	// background
	SDL_Rect dest = window_area;
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background, NULL, screen, &dest);

	// close button
	closeButton->render();

	// text overlay
	WidgetLabel label;
	if (!title.hidden) {
		label.set(window_area.x+title.x, window_area.y+title.y, title.justify, title.valign, msg->get("Inventory"), color_normal);
		label.render();
	}
	if (!main_lbl.hidden) {
		label.set(window_area.x+main_lbl.x, window_area.y+main_lbl.y, main_lbl.justify, main_lbl.valign, msg->get("Main Hand"), color_normal);
		label.render();
	}
	if (!body_lbl.hidden) {
		label.set(window_area.x+body_lbl.x, window_area.y+body_lbl.y, body_lbl.justify, body_lbl.valign, msg->get("Body"), color_normal);
		label.render();
	}
	if (!off_lbl.hidden) {
		label.set(window_area.x+off_lbl.x, window_area.y+off_lbl.y, off_lbl.justify, off_lbl.valign, msg->get("Off Hand"), color_normal);
		label.render();
	}
	if (!artifact_lbl.hidden) {
		label.set(window_area.x+artifact_lbl.x, window_area.y+artifact_lbl.y, artifact_lbl.justify, artifact_lbl.valign, msg->get("Artifact"), color_normal);
		label.render();
	}
	if (!gold_lbl.hidden) {
		label.set(window_area.x+gold_lbl.x, window_area.y+gold_lbl.y, gold_lbl.justify, gold_lbl.valign, msg->get("%d Gold", gold), color_normal);
		label.render();
	}

	inventory[EQUIPMENT].render();
	inventory[CARRIED].render();
}

int MenuInventory::areaOver(Point mouse) {
	if (isWithin(equipped_area, mouse)) {
		return EQUIPMENT;
	}
	else if (isWithin(carried_area, mouse)) {
		return CARRIED;
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
	TooltipData tip;

	area = areaOver( mouse);
	if( area > -1) {
		tip = inventory[area].checkTooltip( mouse, stats, false);
	}
	else if (mouse.x >= window_area.x + help_pos.x && mouse.y >= window_area.y+help_pos.y && mouse.x < window_area.x+help_pos.x+help_pos.w && mouse.y < window_area.y+help_pos.y+help_pos.h) {
		tip.lines[tip.num_lines++] = msg->get("Use SHIFT to move only one item.");
		tip.lines[tip.num_lines++] = msg->get("CTRL-click a carried item to sell it.");
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
			updateEquipment( inventory[EQUIPMENT].drag_prev_slot);
		}
	}

	return item;
}

/**
 * Return dragged item to previous slot
 */
void MenuInventory::itemReturn( ItemStack stack) {
	inventory[drag_prev_src].itemReturn( stack);
	// if returning equipment, prepare to change stats/sprites
	if (drag_prev_src == EQUIPMENT) {
		updateEquipment( inventory[EQUIPMENT].drag_prev_slot);
	}
	drag_prev_src = -1;
}

/**
 * Dragging and dropping an item can be used to rearrange the inventory
 * and equip items
 */
void MenuInventory::drop(Point mouse, ItemStack stack) {
	int area;
	int slot;
	int drag_prev_slot;

	items->playSound(stack.item);

	area = areaOver( mouse);
	slot = inventory[area].slotOver(mouse);
	drag_prev_slot = inventory[drag_prev_src].drag_prev_slot;

	if (area == EQUIPMENT) { // dropped onto equipped item

		// make sure the item is going to the correct slot
		// note: equipment slots 0-3 correspond with item types 0-3
		// also check to see if the hero meets the requirements
		if (drag_prev_src == CARRIED && slot == items->items[stack.item].type && requirementsMet(stack.item)) {
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
			if (inventory[area][slot].item == stack.item) {
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
				&& items->items[inventory[CARRIED][slot].item].type == drag_prev_slot
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
	else {
		itemReturn( stack); // not dropped into a slot. Just return it to the previous slot.
	}

	drag_prev_src = -1;
}

/**
 * Right-clicking on a usable item in the inventory causes it to activate.
 * e.g. drink a potion
 * e.g. equip an item
 */
void MenuInventory::activate(InputState * input) {
	int slot;
	int equip_slot;
	ItemStack stack;
	Point nullpt;
	nullpt.x = nullpt.y = 0;

	// clicked a carried item
	slot = inventory[CARRIED].slotOver(input->mouse);

	// use a consumable item
	if (items->items[inventory[CARRIED][slot].item].type == ITEM_TYPE_CONSUMABLE) {

		//don't use untransform item if hero is not transformed
		if (powers->powers[items->items[inventory[CARRIED][slot].item].power].spawn_type == "untransform" && !stats->transformed) return;

		// if this item requires targeting it can't be used this way
		if (!powers->powers[items->items[inventory[CARRIED][slot].item].power].requires_targeting) {

			powers->activate(items->items[inventory[CARRIED][slot].item].power, stats, nullpt);
			// intercept used_item flag.  We will destroy the item here.
			powers->used_item = -1;
			inventory[CARRIED].substract(slot);
		}
		else {
			// let player know this can only be used from the action bar
			log_msg = msg->get("This item can only be used from the action bar.");
		}

	}
	// equip an item
	else {
		equip_slot = items->items[inventory[CARRIED][slot].item].type;
		if (equip_slot == ITEM_TYPE_MAIN ||
			 equip_slot == ITEM_TYPE_BODY ||
			 equip_slot == ITEM_TYPE_OFF ||
			 equip_slot == ITEM_TYPE_ARTIFACT) {
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
		}
	}
}

/**
 * Insert item into first available carried slot, preferably in the optionnal specified slot
 *
 * @param ItemStack Stack of items
 * @param area Area number where it will try to store the item
 * @param slot Slot number where it will try to store the item
 */
void MenuInventory::add(ItemStack stack, int area, int slot) {
	int max_quantity;
	int quantity_added;
	int i;

	items->playSound(stack.item);

	if( stack.item != 0) {
		if( area < 0) {
			area = CARRIED;
		}
		max_quantity = items->items[stack.item].max_quantity;
		if( slot > -1 && inventory[area][slot].item != 0 && inventory[area][slot].item != stack.item) {
			// the proposed slot isn't available, search for another one
			slot = -1;
		}
		if( area == CARRIED) {
			// first search of stack to complete if the item is stackable
			i = 0;
			while( max_quantity > 1 && slot == -1 && i < MAX_CARRIED) {
				if (inventory[area][i].item == stack.item && inventory[area][i].quantity < max_quantity) {
					slot = i;
				}
				i++;
			}
			// then an empty slot
			i = 0;
			while( slot == -1 && i < MAX_CARRIED) {
				if (inventory[area][i].item == 0) {
					slot = i;
				}
				i++;
			}
		}
		if( slot != -1) {
			// Add
			quantity_added = min( stack.quantity, max_quantity - inventory[area][slot].quantity);
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
			// TODO: We should drop on the floor an item we can't store
		}
	}
}

/**
 * Remove one given item from the player's inventory.
 */
void MenuInventory::remove(int item) {
	if( ! inventory[CARRIED].remove(item)) {
		inventory[EQUIPMENT].remove(item);
	}
}

/**
 * Add gold to the current total
 */
void MenuInventory::addGold(int count) {
	gold += count;
	items->playCoinsSound();
}

/**
 * Check if there is enough gold to buy the given stack, and if so remove it from the current total and add the stack.
 * (Handle the drop into the equipment area, but add() don't handle it well in all circonstances. MenuManager::logic() allow only into the carried area.)
 */
bool MenuInventory::buy(ItemStack stack, Point mouse) {
	int area;
	int slot = -1;
	int count = items->items[stack.item].price * stack.quantity;

	if( gold >= count) {
		gold -= count;

		area = areaOver( mouse);
		if( area > -1) {
			slot = inventory[area].slotOver( mouse);
		}
		if( slot > -1) {
			add( stack, area, slot);
		}
		else {
			add(stack);
		}
		items->playCoinsSound();
		return true;
	}
	else {
		return false;
	}
}

/**
 * Similar to buy(), but for use with stash
 */
bool MenuInventory::stashRemove(ItemStack stack, Point mouse) {
	int area;
	int slot = -1;

	area = areaOver( mouse);
	if( area > -1) {
		slot = inventory[area].slotOver( mouse);
	}
	if( slot > -1) {
		add( stack, area, slot);
	}
	else {
		add(stack);
	}
	return true;
}

/**
 * Similar to sell(), but for use with stash
 */
bool MenuInventory::stashAdd(ItemStack stack) {
	// items that have no price cannot be stored
	if (items->items[stack.item].price == 0) return false;

	return true;
}
/**
 * Sell a specific stack of items
 */
bool MenuInventory::sell(ItemStack stack) {
	// items that have no price cannot be sold
	if (items->items[stack.item].price == 0) return false;

	int value_each;
	if(items->items[stack.item].price_sell != 0)
		value_each = items->items[stack.item].price_sell;
	else
		value_each = items->items[stack.item].price / items->vendor_ratio;
	if (value_each == 0) value_each = 1;
	int value = value_each * stack.quantity;
	gold += value;
	items->playCoinsSound();
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
	if (slot < SLOT_ARTIFACT) {
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

	int prev_hp = stats->hp;
	int prev_mp = stats->mp;

	const vector<Item> &pc_items = items->items;

	// calculate bonuses to basic stats and check that each equipped item fit requirements
	bool checkRequired = true;
	while(checkRequired)
	{
		checkRequired = false;
		stats->offense_additional = stats->defense_additional = stats->physical_additional = stats->mental_additional = 0;
		for (int i = 0; i < 4; i++) {
			int item_id = equipped[i].item;
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
		for (int i = 0; i < 4; i++) {
			if (!requirementsMet(equipped[i].item)) {
				add(equipped[i]);
				equipped[i].item = 0;
				equipped[i].quantity = 0;
				checkRequired = true;
			}
		}
	}

	// defaults
	stats->recalc();
	stats->offense_additional = stats->defense_additional = stats->physical_additional = stats->mental_additional = 0;
	stats->dmg_melee_min = stats->dmg_melee_min_default;
	stats->dmg_melee_max = stats->dmg_melee_max_default;
	stats->dmg_ranged_min = stats->dmg_ranged_min_default;
	stats->dmg_ranged_max = stats->dmg_ranged_max_default;
	stats->dmg_ment_min = stats->dmg_ment_min_default;
	stats->dmg_ment_max = stats->dmg_ment_max_default;
	stats->absorb_min = stats->absorb_min_default;
	stats->absorb_max = stats->absorb_max_default;
	stats->speed = stats->speed_default;
	stats->dspeed = stats->dspeed_default;
	stats->vulnerable_fire = 100;
	stats->vulnerable_ice = 100;

	// reset wielding vars
	stats->wielding_physical = false;
	stats->wielding_mental = false;
	stats->wielding_offense = false;

	// main hand weapon
	int item_id = equipped[SLOT_MAIN].item;
	if (item_id > 0) {
		const Item &item = pc_items[item_id];
		if (item.req_stat == REQUIRES_PHYS) {
			stats->dmg_melee_min = item.dmg_min;
			stats->dmg_melee_max = item.dmg_max;
			stats->melee_weapon_power = item.power_mod;
			stats->wielding_physical = true;
		}
		else if (item.req_stat == REQUIRES_MENT) {
			stats->dmg_ment_min = item.dmg_min;
			stats->dmg_ment_max = item.dmg_max;
			stats->mental_weapon_power = item.power_mod;
			stats->wielding_mental = true;
		}
	}
	// off hand item
	item_id = equipped[SLOT_OFF].item;
	if (item_id > 0) {
		const Item &item = pc_items[item_id];
		if (item.req_stat == REQUIRES_OFF) {
			stats->dmg_ranged_min = item.dmg_min;
			stats->dmg_ranged_max = item.dmg_max;
			stats->ranged_weapon_power = item.power_mod;
			stats->wielding_offense = true;
		}
		else if (item.req_stat == REQUIRES_DEF) {
			stats->absorb_min += item.abs_min;
			stats->absorb_max += item.abs_max;
		}
	}
	// body item
	item_id = equipped[SLOT_BODY].item;
	if (item_id > 0) {
		const Item &item = pc_items[item_id];
		stats->absorb_min += item.abs_min;
		stats->absorb_max += item.abs_max;
	}

	// apply bonuses from all items
	for (int i=0; i<4; i++) {
		item_id = equipped[i].item;
		const Item &item = pc_items[item_id];
		bonus_counter = 0;
		while (bonus_counter < item.bonus_stat.size() && item.bonus_stat[bonus_counter] != "") {

			if (item.bonus_stat[bonus_counter] == "HP")
				stats->maxhp += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "HP regen")
				stats->hp_per_minute += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "MP")
				stats->maxmp += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "MP regen")
				stats->mp_per_minute += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "accuracy")
				stats->accuracy += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "avoidance")
				stats->avoidance += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "crit")
				stats->crit += item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "speed") {
				stats->speed += item.bonus_val[bonus_counter];
				// speed bonuses are in multiples of 3
				// 3 ordinal, 2 diagonal is rounding pythagorus
				stats->dspeed += ((item.bonus_val[bonus_counter]) * 2) /3;
			}
			else if (item.bonus_stat[bonus_counter] == "fire resist")
				stats->vulnerable_fire -= item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "ice resist")
				stats->vulnerable_ice -= item.bonus_val[bonus_counter];
			else if (item.bonus_stat[bonus_counter] == "offense")
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

	// apply previous hp/mp
	if (prev_hp < stats->maxhp)
		stats->hp = prev_hp;
	else
		stats->hp = stats->maxhp;

	if (prev_mp < stats->maxmp)
		stats->mp = prev_mp;
	else
		stats->mp = stats->maxmp;

}

MenuInventory::~MenuInventory() {
	SDL_FreeSurface(background);
	delete closeButton;
}
