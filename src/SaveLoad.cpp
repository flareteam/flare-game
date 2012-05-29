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
#include "MenuActionBar.h"
#include "MenuCharacter.h"
#include "MenuInventory.h"
#include "MenuManager.h"
#include "MenuTalker.h"
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

		// current experience
		outfile << "xp=" << pc->stats.xp << "\n";

		// stat spec
		outfile << "build=" << pc->stats.physical_character << "," << pc->stats.mental_character << "," << pc->stats.offense_character << "," << pc->stats.defense_character << "\n";

		// current gold
		outfile << "gold=" << menu->inv->gold << "\n";

		// equipped gear
		outfile << "equipped=" << menu->inv->inventory[EQUIPMENT].getItems() << "\n";
		outfile << "equipped_quantity=" << menu->inv->inventory[EQUIPMENT].getQuantities() << "\n";

		// carried items
		outfile << "carried=" << menu->inv->inventory[CARRIED].getItems() << "\n";
		outfile << "carried_quantity=" << menu->inv->inventory[CARRIED].getQuantities() << "\n";

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
		
		// campaign data
		outfile << "campaign=";
		outfile << camp->getAll();
		
		outfile << endl;
		
		outfile.close();
	}
}

/**
 * When loading the game, load from file if possible
 */
void GameStatePlay::loadGame() {

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
			    pc->stats.permadeath = atoi(infile.val.c_str());
			}
			else if (infile.key == "option") {
				pc->stats.base = infile.nextValue();
				pc->stats.head = infile.nextValue();
				pc->stats.portrait = infile.nextValue();
			}
			else if (infile.key == "xp") pc->stats.xp = atoi(infile.val.c_str());
			else if (infile.key == "build") {
				pc->stats.physical_character = atoi(infile.nextValue().c_str());
				pc->stats.mental_character = atoi(infile.nextValue().c_str());
				pc->stats.offense_character = atoi(infile.nextValue().c_str());
				pc->stats.defense_character = atoi(infile.nextValue().c_str());
			}
			else if (infile.key == "gold") {
				menu->inv->gold = atoi(infile.val.c_str());
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
					map->teleport_destination.x = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					map->teleport_destination.y = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					map->teleportation = true;
				
					// prevent spawn.txt from putting us on the starting map
					map->clearEvents();
				}
				else {
					map->teleport_mapname = "spawn.txt";
					map->teleport_destination.x = 1;
					map->teleport_destination.y = 1;
					map->teleportation = true;
					
				}
			}
			else if (infile.key == "actionbar") {
				for (int i=0; i<12; i++)
					hotkeys[i] = atoi(infile.nextValue().c_str());
				menu->act->set(hotkeys);
			}
			else if (infile.key == "campaign") camp->setAll(infile.val);
		}
			
		infile.close();
	}

	// initialize vars
	pc->stats.recalc();
	menu->inv->applyEquipment(menu->inv->inventory[EQUIPMENT].storage);
	pc->stats.hp = pc->stats.maxhp;
	pc->stats.mp = pc->stats.maxmp;
	
	// reset character menu
	menu->chr->refreshStats();

	// just for aesthetics, turn the hero to face the camera
	pc->stats.direction = 6;

	// set up MenuTalker for this hero
	menu->talker->setHero(pc->stats.name, pc->stats.portrait);

	// load sounds (gender specific)
	pc->loadSounds();

}

