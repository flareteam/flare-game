/**
 * Save and Load functions for the GameStatePlay.
 *
 * I put these in a separate cpp file just to keep GameStatePlay.cpp devoted to its core.
 *
 * TODO: handle stackable items
 *
 * class GameStatePlay
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "GameStatePlay.h"
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
	ss << "saves/save" << game_slot << ".txt";

	outfile.open(ss.str().c_str(), ios::out);

	if (outfile.is_open()) {

		// hero name
		outfile << "name=" << pc->stats.name << "\n";
		
		// hero base (e.g. human male) and look (e.g. option1)
		outfile << "base=" << pc->stats.base << "\n";
		outfile << "look=" << pc->stats.look << "\n";

		// current experience
		outfile << "xp=" << pc->stats.xp << "\n";

		// stat spec
		outfile << "build=" << pc->stats.physical << "," << pc->stats.mental << "," << pc->stats.offense << "," << pc->stats.defense << "\n";

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
			outfile << menu->act->hotkeys[i];
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
	ss << "saves/save" << game_slot << ".txt";

	if (infile.open(ss.str())) {
		while (infile.next()) {
			if (infile.key == "name") pc->stats.name = infile.val;
			else if (infile.key == "base") pc->stats.base = infile.val;
			else if (infile.key == "look") pc->stats.look = infile.val;
			else if (infile.key == "xp") pc->stats.xp = atoi(infile.val.c_str());
			else if (infile.key == "build") {
				pc->stats.physical = atoi(infile.nextValue().c_str());
				pc->stats.mental = atoi(infile.nextValue().c_str());
				pc->stats.offense = atoi(infile.nextValue().c_str());
				pc->stats.defense = atoi(infile.nextValue().c_str());
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
				map->teleport_destination.x = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
				map->teleport_destination.y = atoi(infile.nextValue().c_str()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
				map->teleportation = true;
				
				// prevent spawn.txt from putting us on the starting map
				map->clearEvents();
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
	menu->items->applyEquipment(&pc->stats, menu->inv->inventory[EQUIPMENT].storage);
	pc->stats.hp = pc->stats.maxhp;
	pc->stats.mp = pc->stats.maxmp;
			
	// just for aesthetics, turn the hero to face the camera
	pc->stats.direction = 6;

}

