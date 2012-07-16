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
 * class MenuPowers
 */

#include "Menu.h"
#include "FileParser.h"
#include "MenuPowers.h"
#include "SharedResources.h"
#include "PowerManager.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetLabel.h"
#include "WidgetTooltip.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace std;
MenuPowers *menuPowers = NULL;
MenuPowers *MenuPowers::getInstance() {
	return menuPowers;
}


MenuPowers::MenuPowers(StatBlock *_stats, PowerManager *_powers, SDL_Surface *_icons) {
	stats = _stats;
	powers = _powers;
	icons = _icons;

	overlay_disabled = NULL;

	visible = false;
	loadGraphics();

	points_left = 0;
	pressed = false;

	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		power_cell[i].id = -1;
		power_cell[i].pos.x = 0;
		power_cell[i].pos.y = 0;
		power_cell[i].requires_mentdef = 0;
		power_cell[i].requires_mentoff = 0;
		power_cell[i].requires_physoff = 0;
		power_cell[i].requires_physdef = 0;
		power_cell[i].requires_defense = 0;
		power_cell[i].requires_offense = 0;
		power_cell[i].requires_physical = 0;
		power_cell[i].requires_mental = 0;
		power_cell[i].requires_level = 0;
		power_cell[i].requires_power = -1;
		power_cell[i].requires_point = false;
	}

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// Read powers data from config file 
	FileParser infile;
	int counter = -1;
	if (infile.open(mods->locate("menus/powers.txt"))) {
	  while (infile.next()) {
		infile.val = infile.val + ',';

		if (infile.key == "id") {
			counter++;
			power_cell[counter].id = eatFirstInt(infile.val, ',');
		} else if (infile.key == "position") {
			power_cell[counter].pos.x = eatFirstInt(infile.val, ',');
			power_cell[counter].pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_physoff") {
			power_cell[counter].requires_physoff = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_physdef") {
			power_cell[counter].requires_physdef = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_mentoff") {
			power_cell[counter].requires_mentoff = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_mentdef") {
			power_cell[counter].requires_mentdef = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_defense") {
			power_cell[counter].requires_defense = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_offense") {
			power_cell[counter].requires_offense = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_physical") {
			power_cell[counter].requires_physical = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_mental") {
			power_cell[counter].requires_mental = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_point") {
			if (infile.val == "true,") power_cell[counter].requires_point = true;
		} else if (infile.key == "requires_level") {
			power_cell[counter].requires_level = eatFirstInt(infile.val, ',');
		} else if (infile.key == "requires_power") {
			power_cell[counter].requires_power = eatFirstInt(infile.val, ',');
		} else if (infile.key == "closebutton_pos") {
			close_pos.x = eatFirstInt(infile.val, ',');
			close_pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "unspent_points_pos") {
			unspent_pos.x = eatFirstInt(infile.val, ',');
			unspent_pos.y = eatFirstInt(infile.val, ',');
		}

	  }
	} else fprintf(stderr, "Unable to open powers_menu.txt!\n");
	infile.close();

	menuPowers = this;
}

void MenuPowers::update() {
	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		slots[i].w = slots[i].h = 32;
		slots[i].x = window_area.x + power_cell[i].pos.x;
		slots[i].y = window_area.y + power_cell[i].pos.y;
	}

	label_powers.set(window_area.x+160, window_area.y+8, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Powers"), FONT_WHITE);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;

	stat_up.set(window_area.x+unspent_pos.x, window_area.y+unspent_pos.y, JUSTIFY_CENTER, VALIGN_TOP, "", FONT_GREEN);
	points_left = stats->level - powers_list.size();
}

void MenuPowers::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/powers.png").c_str());
	powers_tree = IMG_Load(mods->locate("images/menus/powers_tree.png").c_str());
	powers_unlock = IMG_Load(mods->locate("images/menus/powers_unlock.png").c_str());
	overlay_disabled = IMG_Load(mods->locate("images/menus/disabled.png").c_str());
	
	if(!background || !powers_tree || !powers_unlock) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = powers_tree;
	powers_tree = SDL_DisplayFormatAlpha(powers_tree);
	SDL_FreeSurface(cleanup);

	cleanup = powers_unlock;
	powers_unlock = SDL_DisplayFormatAlpha(powers_unlock);
	SDL_FreeSurface(cleanup);
	
	if (overlay_disabled != NULL) {
		cleanup = overlay_disabled;
		overlay_disabled = SDL_DisplayFormatAlpha(overlay_disabled);
		SDL_FreeSurface(cleanup);
	}
}

/**
 * generic render 32-pixel icon
 */
void MenuPowers::renderIcon(int icon_id, int x, int y) {
	SDL_Rect icon_src;
	SDL_Rect icon_dest;

	icon_dest.x = x;
	icon_dest.y = y;
	icon_src.w = icon_src.h = icon_dest.w = icon_dest.h = 32;
	icon_src.x = (icon_id % 16) * 32;
	icon_src.y = (icon_id / 16) * 32;
	SDL_BlitSurface(icons, &icon_src, screen, &icon_dest);
}

/**
 * With great power comes great stat requirements.
 */
bool MenuPowers::requirementsMet(int power_index) {

	// power_index can be -1 during recursive call if requires_power is not defined.
	// Power with index -1 doesn't exist and is always enabled
	if (power_index == -1) return true;

	// Find cell with our power
	int id;
	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		if (power_cell[i].id == power_index) {
		id = i;
		break;
		}
	}

	// If power_id is saved into vector, it's unlocked anyway
	if (find(powers_list.begin(), powers_list.end(), power_index) != powers_list.end()) return true;

	// Check the rest requirements
	if ((stats->physoff >= power_cell[id].requires_physoff) &&
		(stats->physdef >= power_cell[id].requires_physdef) &&
		(stats->mentoff >= power_cell[id].requires_mentoff) &&
		(stats->mentdef >= power_cell[id].requires_mentdef) &&
		(stats->get_defense() >= power_cell[id].requires_defense) &&
		(stats->get_offense() >= power_cell[id].requires_offense) &&
		(stats->get_physical() >= power_cell[id].requires_physical) &&
		(stats->get_mental() >= power_cell[id].requires_mental) &&
		(stats->level >= power_cell[id].requires_level) &&
		 requirementsMet(power_cell[id].requires_power) &&
		!power_cell[id].requires_point) return true;
	return false;
}

/**
 * Check if we can unlock power.
 */
bool MenuPowers::powerUnlockable(int power_index) {

	// power_index can be -1 during recursive call if requires_power is not defined.
	// Power with index -1 doesn't exist and is always enabled
	if (power_index == -1) return true;

	// Find cell with our power
	int id;
	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		if (power_cell[i].id == power_index) {
		id = i;
		break;
		}
	}

	// Check requirements
	if ((stats->physoff >= power_cell[id].requires_physoff) &&
		(stats->physdef >= power_cell[id].requires_physdef) &&
		(stats->mentoff >= power_cell[id].requires_mentoff) &&
		(stats->mentdef >= power_cell[id].requires_mentdef) &&
		(stats->get_defense() >= power_cell[id].requires_defense) &&
		(stats->get_offense() >= power_cell[id].requires_offense) &&
		(stats->get_physical() >= power_cell[id].requires_physical) &&
		(stats->get_mental() >= power_cell[id].requires_mental) &&
		(stats->level >= power_cell[id].requires_level) &&
		 requirementsMet(power_cell[id].requires_power)) return true;
	return false;
}

/**
 * Click-to-drag a power (to the action bar)
 */
int MenuPowers::click(Point mouse) {

	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		if (isWithin(slots[i], mouse) && (power_cell[i].id != -1)) {
			if (requirementsMet(power_cell[i].id)) return power_cell[i].id;
			else return -1;
		}
	}
	return -1;
}

/**
 * Unlock a power
 */
void MenuPowers::unlock_click(Point mouse) {
	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		if (isWithin(slots[i], mouse) && (power_cell[i].id != -1) && (powerUnlockable(power_cell[i].id)) && points_left > 0 && power_cell[i].requires_point) {
			powers_list.push_back(power_cell[i].id);
			points_left = stats->level - powers_list.size();
		}
	}
}

void MenuPowers::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}
}

void MenuPowers::render() {
	if (!visible) return;

	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);
	SDL_BlitSurface(powers_tree, &src, screen, &dest);

	SDL_Rect disabled_src;
	disabled_src.x = disabled_src.y = 0;
	disabled_src.w = disabled_src.h = 32;

	// power icons
	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		bool power_in_vector = false;

		// Continue if slot is not filled with data
		if (power_cell[i].id == -1) continue;

		if (find(powers_list.begin(), powers_list.end(), power_cell[i].id) != powers_list.end()) power_in_vector = true;

		renderIcon(powers->powers[power_cell[i].id].icon, window_area.x + power_cell[i].pos.x, window_area.y + power_cell[i].pos.y);

		// highlighting
		if (power_in_vector || requirementsMet(power_cell[i].id)) {
			displayBuild(power_cell[i].id);
		}
		else {
		
			if (overlay_disabled != NULL) {
				SDL_BlitSurface(overlay_disabled, &disabled_src, screen, &slots[i]);
			}
		}
	}

	// close button
	closeButton->render();

	// text overlay
	label_powers.render();

	// stats
	stringstream ss;

	ss.str("");
	points_left = stats->level - powers_list.size();
	if (points_left !=0) {
		ss << "Unspent power points:" << " " << points_left;
	}
	stat_up.set(ss.str());
	stat_up.render();
}

/**
 * Highlight unlocked powers
 */
void MenuPowers::displayBuild(int power_id) {
	SDL_Rect src_unlock;
//	SDL_Rect dest;

	src_unlock.x = 0;
	src_unlock.y = 0;
	src_unlock.w = 32;
	src_unlock.h = 32;

	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		if (power_cell[i].id == power_id) {
//			dest.x = window_area.x + power_cell[i].pos.x;
//			dest.y = window_area.y + power_cell[i].pos.y;
			SDL_BlitSurface(powers_unlock, &src_unlock, screen, &slots[i]);
		}
	}
}

/**
 * Show mouseover descriptions of disciplines and powers
 */
TooltipData MenuPowers::checkTooltip(Point mouse) {

	TooltipData tip;

		for (int i=0; i<POWER_SLOTS_COUNT; i++) {
			if (isWithin(slots[i], mouse) && (power_cell[i].id != -1)) {
				tip.lines[tip.num_lines++] = powers->powers[power_cell[i].id].name;
				tip.lines[tip.num_lines++] = powers->powers[power_cell[i].id].description;

				if (powers->powers[power_cell[i].id].requires_physical_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires a physical weapon");
				else if (powers->powers[power_cell[i].id].requires_mental_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires a mental weapon");
				else if (powers->powers[power_cell[i].id].requires_offense_weapon)
					tip.lines[tip.num_lines++] = msg->get("Requires an offense weapon");


				// add requirement
				if ((power_cell[i].requires_physoff > 0) && (stats->physoff < power_cell[i].requires_physoff)) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Physical Offense %d", power_cell[i].requires_physoff);
				} else if((power_cell[i].requires_physoff > 0) && (stats->physoff >= power_cell[i].requires_physoff)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Physical Offense %d", power_cell[i].requires_physoff);
				}
				if ((power_cell[i].requires_physdef > 0) && (stats->physdef < power_cell[i].requires_physdef)) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Physical Defense %d", power_cell[i].requires_physdef);
				} else if ((power_cell[i].requires_physdef > 0) && (stats->physdef >= power_cell[i].requires_physdef)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Physical Defense %d", power_cell[i].requires_physdef);
				}
				if ((power_cell[i].requires_mentoff > 0) && (stats->mentoff < power_cell[i].requires_mentoff)) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Mental Offense %d", power_cell[i].requires_mentoff);
				} else if ((power_cell[i].requires_mentoff > 0) && (stats->mentoff >= power_cell[i].requires_mentoff)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Mental Offense %d", power_cell[i].requires_mentoff);
				}
				if ((power_cell[i].requires_mentdef > 0) && (stats->mentdef < power_cell[i].requires_mentdef)) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Mental Defense %d", power_cell[i].requires_mentdef);
				} else if ((power_cell[i].requires_mentdef > 0) && (stats->mentdef >= power_cell[i].requires_mentdef)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Mental Defense %d", power_cell[i].requires_mentdef);
				}
				if ((power_cell[i].requires_offense > 0) && (stats->get_offense() < power_cell[i].requires_offense)) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Offense %d", power_cell[i].requires_offense);
				} else if ((power_cell[i].requires_offense > 0) && (stats->get_offense() >= power_cell[i].requires_offense)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Offense %d", power_cell[i].requires_offense);
				}
				if ((power_cell[i].requires_defense > 0) && (stats->get_defense() < power_cell[i].requires_defense)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Defense %d", power_cell[i].requires_defense);
				} else if ((power_cell[i].requires_defense > 0) && (stats->get_defense() >= power_cell[i].requires_defense)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Defense %d", power_cell[i].requires_defense);
				}
				if ((power_cell[i].requires_physical > 0) && (stats->get_physical() < power_cell[i].requires_physical)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Physical %d", power_cell[i].requires_physical);
				} else if ((power_cell[i].requires_physical > 0) && (stats->get_physical() >= power_cell[i].requires_physical)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Physical %d", power_cell[i].requires_physical);
				}
				if ((power_cell[i].requires_mental > 0) && (stats->get_mental() < power_cell[i].requires_mental)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Mental %d", power_cell[i].requires_mental);
				} else if ((power_cell[i].requires_mental > 0) && (stats->get_mental() >= power_cell[i].requires_mental)) {
					tip.lines[tip.num_lines++] = msg->get("Requires Mental %d", power_cell[i].requires_mental);
				}

				// Draw required Level Tooltip
				if ((power_cell[i].requires_level > 0) && stats->level < power_cell[i].requires_level) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Level %d", power_cell[i].requires_level);
				}
				else if ((power_cell[i].requires_level > 0) && stats->level >= power_cell[i].requires_level) {
					tip.lines[tip.num_lines++] = msg->get("Requires Level %d", power_cell[i].requires_level);
				}

				// Draw required Skill Point Tooltip
				if ((power_cell[i].requires_point) &&
					!(find(powers_list.begin(), powers_list.end(), power_cell[i].id) != powers_list.end()) &&
					(points_left < 1)) {
						tip.colors[tip.num_lines] = FONT_RED;
						tip.lines[tip.num_lines++] = msg->get("Requires %d Skill Point", power_cell[i].requires_point);
				}
				else if ((power_cell[i].requires_point) &&
					!(find(powers_list.begin(), powers_list.end(), power_cell[i].id) != powers_list.end()) &&
					(points_left > 0))
						tip.lines[tip.num_lines++] = msg->get("Requires %d SKill Point", power_cell[i].requires_point);

				// Draw unlock power Tooltip
				if (power_cell[i].requires_point && 
					!(find(powers_list.begin(), powers_list.end(), power_cell[i].id) != powers_list.end()) &&
					(points_left > 0) &&
					powerUnlockable(power_cell[i].id) && (points_left > 0)) {
						tip.colors[tip.num_lines] = FONT_GREEN;
						tip.lines[tip.num_lines++] = msg->get("Click to Unlock");
					}


				// Required Power Tooltip
				if ((power_cell[i].requires_power != -1) && !(requirementsMet(power_cell[i].id))) {
					tip.colors[tip.num_lines] = FONT_RED;
					tip.lines[tip.num_lines++] = msg->get("Requires Power: %s", powers->powers[power_cell[i].requires_power].name);
				}
				else if ((power_cell[i].requires_power != -1) && (requirementsMet(power_cell[i].id))) {
					tip.lines[tip.num_lines++] = msg->get("Requires Power: %s", powers->powers[power_cell[i].requires_power].name);
				}

				// add mana cost
				if (powers->powers[power_cell[i].id].requires_mp > 0) {
					tip.lines[tip.num_lines++] = msg->get("Costs %d MP", powers->powers[power_cell[i].id].requires_mp);
				}
				// add cooldown time
				if (powers->powers[power_cell[i].id].cooldown > 0) {
					tip.lines[tip.num_lines++] = msg->get("Cooldown: %d seconds", powers->powers[power_cell[i].id].cooldown / 1000.0);
				}

				return tip;
			}
		}

	return tip;
}

MenuPowers::~MenuPowers() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(powers_unlock);
	SDL_FreeSurface(overlay_disabled);
	
	delete closeButton;
	menuPowers = NULL;
}

/**
 * Return true if required stats for power usage are met. Else return false.
 */
bool MenuPowers::meetsUsageStats(unsigned powerid) {

	// Find cell with our power
	int id;
	for (int i=0; i<POWER_SLOTS_COUNT; i++) {
		if (power_cell[i].id == (int)powerid) {
		id = i;
		break;
		}
	}

	return stats->physoff >= power_cell[id].requires_physoff	
		&& stats->physdef >= power_cell[id].requires_physdef
		&& stats->mentoff >= power_cell[id].requires_mentoff
		&& stats->mentdef >= power_cell[id].requires_mentdef
		&& stats->get_defense() >= power_cell[id].requires_defense
		&& stats->get_offense() >= power_cell[id].requires_offense
		&& stats->get_mental() >= power_cell[id].requires_mental
		&& stats->get_physical() >= power_cell[id].requires_physical;
}
