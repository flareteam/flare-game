/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
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
 * Save and Load functions for the GameStatePlay.
 *
 * I put these in a separate cpp file just to keep GameStatePlay.cpp devoted to its core.
 *
 * class GameStatePlay
 */

#include "Avatar.h"
#include "CampaignManager.h"
#include "FileParser.h"
#include "GameStatePlay.h"
#include "MapRenderer.h"
#include "Menu.h"
#include "MenuActionBar.h"
#include "MenuCharacter.h"
#include "MenuInventory.h"
#include "MenuManager.h"
#include "MenuPowers.h"
#include "MenuStash.h"
#include "MenuTalker.h"
#include "PowerManager.h"
#include "Settings.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

/**
 * Before exiting the game, save to file
 */
void GameStatePlay::saveGame() {

	// game slots are currently 1-4
	if (game_slot == 0) return;

	ofstream outfile;

	stringstream ss;
	ss.str("");
	ss << PATH_USER << "save" << game_slot << ".txt";

	outfile.open(ss.str().c_str(), ios::out);

	if (outfile.is_open()) {

		// hero name
		outfile << "name=" << pc->stats.name << "\n";

		// permadeath
		outfile << "permadeath=" << pc->stats.permadeath << "\n";

		// hero visual option
		outfile << "option=" << pc->stats.base << "," << pc->stats.head << "," << pc->stats.portrait << "\n";

		// hero class
		outfile << "class=" << pc->stats.character_class << "\n";

		// current experience
		outfile << "xp=" << pc->stats.xp << "\n";

		// hp and mp
		if (SAVE_HPMP) outfile << "hpmp=" << pc->stats.hp << "," << pc->stats.mp << "\n";

		// stat spec
		outfile << "build=" << pc->stats.physical_character << "," << pc->stats.mental_character << "," << pc->stats.offense_character << "," << pc->stats.defense_character << "\n";

		// current currency
		outfile << "currency=" << menu->inv->currency << "\n";

		// equipped gear
		outfile << "equipped_quantity=" << menu->inv->inventory[EQUIPMENT].getQuantities() << "\n";
		outfile << "equipped=" << menu->inv->inventory[EQUIPMENT].getItems() << "\n";

		// carried items
		outfile << "carried_quantity=" << menu->inv->inventory[CARRIED].getQuantities() << "\n";
		outfile << "carried=" << menu->inv->inventory[CARRIED].getItems() << "\n";

		// spawn point
		outfile << "spawn=" << map->respawn_map << "," << map->respawn_point.x/UNITS_PER_TILE << "," << map->respawn_point.y/UNITS_PER_TILE << "\n";

		// action bar
		outfile << "actionbar=";
		for (int i=0; i<12; i++) {
			if (pc->stats.transformed) outfile << menu->act->actionbar[i];
			else outfile << menu->act->hotkeys[i];
			if (i<11) outfile << ",";
		}
		outfile << "\n";

		//shapeshifter value
		if (pc->stats.transform_type == "untransform") outfile << "transformed=" << "\n";
		else outfile << "transformed=" << pc->stats.transform_type << "\n";

		// enabled powers
		outfile << "powers=";
		for (unsigned int i=0; i<menu->pow->powers_list.size(); i++) {
			if (i == 0) outfile << menu->pow->powers_list[i];
			else outfile << "," << menu->pow->powers_list[i];
		}
		outfile << "\n";

		// campaign data
		outfile << "campaign=";
		outfile << camp->getAll();

		outfile << endl;

		if (outfile.bad()) fprintf(stderr, "Unable to save the game. No write access or disk is full!\n");
		outfile.close();
		outfile.clear();
	}

	// Save stash
	ss.str("");
	ss << PATH_USER << "stash.txt";

	outfile.open(ss.str().c_str(), ios::out);

	if (outfile.is_open()) {
		outfile << "quantity=" << menu->stash->stock.getQuantities() << "\n";
		outfile << "item=" << menu->stash->stock.getItems() << "\n";

		outfile << endl;

		if (outfile.bad()) fprintf(stderr, "Unable to save stash. No write access or disk is full!\n");
		outfile.close();
		outfile.clear();
	}
}

/**
 * When loading the game, load from file if possible
 */
void GameStatePlay::loadGame() {
	int saved_hp = 0;
	int saved_mp = 0;

	// game slots are currently 1-4
	if (game_slot == 0) return;

	FileParser infile;
	int hotkeys[12];

	for (int i=0; i<12; i++) {
		hotkeys[i] = -1;
	}

	stringstream ss;
	ss.str("");
	ss << PATH_USER << "save" << game_slot << ".txt";

	if (infile.open(ss.str())) {
		while (infile.next()) {
			if (infile.key == "name") pc->stats.name = infile.val;
			else if (infile.key == "permadeath") {
				pc->stats.permadeath = toInt(infile.val);
			}
			else if (infile.key == "option") {
				pc->stats.base = infile.nextValue();
				pc->stats.head = infile.nextValue();
				pc->stats.portrait = infile.nextValue();
			}
			else if (infile.key == "class") {
				pc->stats.character_class = infile.nextValue();
			}
			else if (infile.key == "xp") {
				pc->stats.xp = toInt(infile.val);
				if (pc->stats.xp < 0) {
					fprintf(stderr, "XP value is out of bounds, setting to zero\n");
					pc->stats.xp = 0;
				}
			}
			else if (infile.key == "hpmp") {
				saved_hp = toInt(infile.nextValue());
				saved_mp = toInt(infile.nextValue());
				if (saved_hp < 0 || saved_hp > pc->stats.maxhp ||
					saved_mp < 0 || saved_mp > pc->stats.maxmp) {

					fprintf(stderr, "MP/HP value is out of bounds, setting to maximum\n");
					saved_hp = pc->stats.maxhp;
					saved_mp = pc->stats.maxmp;
				}
			}
			else if (infile.key == "build") {
				pc->stats.physical_character = toInt(infile.nextValue());
				pc->stats.mental_character = toInt(infile.nextValue());
				pc->stats.offense_character = toInt(infile.nextValue());
				pc->stats.defense_character = toInt(infile.nextValue());
				if (pc->stats.physical_character < 0 || pc->stats.physical_character > pc->stats.max_points_per_stat ||
					pc->stats.mental_character < 0 || pc->stats.mental_character > pc->stats.max_points_per_stat ||
					pc->stats.offense_character < 0 || pc->stats.offense_character > pc->stats.max_points_per_stat ||
					pc->stats.defense_character < 0 || pc->stats.defense_character > pc->stats.max_points_per_stat) {

					fprintf(stderr, "Some basic stats are out of bounds, setting to zero\n");
					pc->stats.physical_character = 0;
					pc->stats.mental_character = 0;
					pc->stats.offense_character = 0;
					pc->stats.defense_character = 0;
				}
			}
			else if (infile.key == "currency") {
				menu->inv->currency = toInt(infile.val);
				if (menu->inv->currency < 0) {
					fprintf(stderr, "Currency value out of bounds, setting to zero\n");
					menu->inv->currency = 0;
				}
			}
			else if (infile.key == "equipped") {
				menu->inv->inventory[EQUIPMENT].setItems(infile.val);
			}
			else if (infile.key == "equipped_quantity") {
				menu->inv->inventory[EQUIPMENT].setQuantities(infile.val);
			}
			else if (infile.key == "carried") {
				menu->inv->inventory[CARRIED].setItems(infile.val);
			}
			else if (infile.key == "carried_quantity") {
				menu->inv->inventory[CARRIED].setQuantities(infile.val);
			}
			else if (infile.key == "spawn") {
				map->teleport_mapname = infile.nextValue();

				if (fileExists(mods->locate("maps/" + map->teleport_mapname))) {
					map->teleport_destination.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					map->teleport_destination.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					map->teleportation = true;

					// prevent spawn.txt from putting us on the starting map
					map->clearEvents();
				}
				else {
					fprintf(stderr, "Unable to find maps/%s, loading spawn.txt\n", map->teleport_mapname.c_str());
					map->teleport_mapname = "spawn.txt";
					map->teleport_destination.x = 1;
					map->teleport_destination.y = 1;
					map->teleportation = true;

				}
			}
			else if (infile.key == "actionbar") {
				for (int i=0; i<12; i++) {
					hotkeys[i] = toInt(infile.nextValue());
					if (hotkeys[i] < 0) {
						fprintf(stderr, "Hotkey power on position %d has negative id, skipping\n", i);
						hotkeys[i] = 0;
					}
					else if ((unsigned)hotkeys[i] > powers->powers.size()-1) {
						fprintf(stderr, "Hotkey power id (%d) out of bounds 1-%d, skipping\n", hotkeys[i], (int)powers->powers.size());
						hotkeys[i] = 0;
					}
					else if (hotkeys[i] != 0 && powers->powers[hotkeys[i]].name == "") {
						fprintf(stderr, "Hotkey power with id=%d, found on position %d does not exist, skipping\n", hotkeys[i], i);
						hotkeys[i] = 0;
					}
				}
				menu->act->set(hotkeys);
			}
			else if (infile.key == "transformed") {
				pc->stats.transform_type = infile.nextValue();
				if (pc->stats.transform_type != "") pc->stats.transform_duration = -1;
			}
			else if (infile.key == "powers") {
				string power;
				while ( (power = infile.nextValue()) != "") {
					menu->pow->powers_list.push_back(toInt(power));
				}
			}
			else if (infile.key == "campaign") camp->setAll(infile.val);
		}

		infile.close();
	} else fprintf(stderr, "Unable to open %s!\n", ss.str().c_str());

	
	menu->inv->inventory[EQUIPMENT].fillEquipmentSlots();
	
	// Load stash
	ss.str("");
	ss << PATH_USER << "stash.txt";

	if (infile.open(ss.str())) {
		while (infile.next()) {
			if (infile.key == "item") {
				menu->stash->stock.setItems(infile.val);
			}
			else if (infile.key == "quantity") {
				menu->stash->stock.setQuantities(infile.val);
			}
		}
		infile.close();
	}  else fprintf(stderr, "Unable to open %s!\n", ss.str().c_str());

	// initialize vars
	pc->stats.recalc();
	menu->inv->applyEquipment(menu->inv->inventory[EQUIPMENT].storage);
	if (SAVE_HPMP) {
		pc->stats.hp = saved_hp;
		pc->stats.mp = saved_mp;
	} else {
		pc->stats.hp = pc->stats.maxhp;
		pc->stats.mp = pc->stats.maxmp;
	}

	// reset character menu
	menu->chr->refreshStats();

	// just for aesthetics, turn the hero to face the camera
	pc->stats.direction = 6;

	// set up MenuTalker for this hero
	menu->talker->setHero(pc->stats.name, pc->stats.portrait);

	// load sounds (gender specific)
	pc->loadSounds();

}

/**
 * Load a class definition, index
 */
void GameStatePlay::loadClass(int index) {
	// game slots are currently 1-4
	if (game_slot == 0) return;

	pc->stats.character_class = HERO_CLASSES[index].name;
	pc->stats.physical_character += HERO_CLASSES[index].physical;
	pc->stats.mental_character += HERO_CLASSES[index].mental;
	pc->stats.offense_character += HERO_CLASSES[index].offense;
	pc->stats.defense_character += HERO_CLASSES[index].defense;
	menu->inv->currency += HERO_CLASSES[index].currency;
	menu->inv->inventory[EQUIPMENT].setItems(HERO_CLASSES[index].equipment);
	for (unsigned i=0; i<HERO_CLASSES[index].powers.size(); i++) {
		menu->pow->powers_list.push_back(HERO_CLASSES[index].powers[i]);
	}
	menu->act->set(HERO_CLASSES[index].hotkeys);
	
	menu->inv->inventory[EQUIPMENT].fillEquipmentSlots();
	
	// initialize vars
	pc->stats.recalc();
	menu->inv->applyEquipment(menu->inv->inventory[EQUIPMENT].storage);

	// reset character menu
	menu->chr->refreshStats();

}

/*
 * This is used to load the stash when starting a new game
 */
void GameStatePlay::loadStash() {
	// Load stash
	FileParser infile;
	stringstream ss;
	ss.str("");
	ss << PATH_USER << "stash.txt";

	if (infile.open(ss.str())) {
		while (infile.next()) {
			if (infile.key == "item") {
				menu->stash->stock.setItems(infile.val);
			}
			else if (infile.key == "quantity") {
				menu->stash->stock.setQuantities(infile.val);
			}
		}
		infile.close();
	}  else fprintf(stderr, "Unable to open %s!\n", ss.str().c_str());
}
