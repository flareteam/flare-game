/**
 * Save and Load functions for the GameEngine.
 *
 * I put these in a separate cpp file just to keep GameEngine.cpp devoted to its core.
 *
 * TODO: handle stackable items
 *
 * class GameEngine
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "GameEngine.h"
#include "UtilsParsing.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

/**
 * Before exiting the game, save to file
 */
void GameEngine::saveGame() {

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

		// current experience
		outfile << "xp=" << pc->stats.xp << "\n";

		// stat spec
		outfile << "build=" << pc->stats.physical << "," << pc->stats.mental << "," << pc->stats.offense << "," << pc->stats.defense << "\n";

		// current gold
		outfile << "gold=" << menu->inv->gold << "\n";

		// equipped gear
		outfile << "equipped=";
		for (int i=0; i<MAX_EQUIPPED; i++) {
			outfile << menu->inv->inventory[EQUIPMENT][i].item;
			if (i<MAX_EQUIPPED-1) outfile << ",";
			else outfile << "\n";
		}

		// equipped gear quantity
		outfile << "equipped_quantity=";
		for (int i=0; i<MAX_EQUIPPED; i++) {
			outfile << menu->inv->inventory[EQUIPMENT][i].quantity;
			if (i<MAX_EQUIPPED-1) outfile << ",";
			else outfile << "\n";
		}

		// carried items
		outfile << "carried=";
		for (int i=0; i<MAX_CARRIED; i++) {
			outfile << menu->inv->inventory[CARRIED][i].item;
			if (i<MAX_CARRIED-1) outfile << ",";
			else outfile << "\n";
		}
 
		// carried items quantity
		outfile << "carried_quantity=";
		for (int i=0; i<MAX_CARRIED; i++) {
			outfile << menu->inv->inventory[CARRIED][i].quantity;
			if (i<MAX_CARRIED-1) outfile << ",";
			else outfile << "\n";
		}

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
void GameEngine::loadGame() {

	// game slots are currently 1-4
	if (game_slot == 0) return;

	ifstream infile;
	string line;
	string key;
	string val;
	string starts_with;
	int hotkeys[12];
	
	for (int i=0; i<12; i++) {
		hotkeys[i] = -1;
	}

	stringstream ss;
	ss.str("");
	ss << "saves/save" << game_slot << ".txt";
	
	infile.open(ss.str().c_str(), ios::in);

	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);

			if (line.length() > 0) {
				starts_with = line.at(0);
				
				if (starts_with != "#") { // skip comments
					
					// this is data.  treatment depends on section type
					parse_key_pair(line, key, val);          
					key = trim(key, ' ');
					val = trim(val, ' ');
				
					if (key == "name") {
						pc->stats.name = val;
					}
					else if (key == "xp") {
						pc->stats.xp = atoi(val.c_str());
					}
					else if (key == "build") {
						val = val + ",";
						pc->stats.physical = eatFirstInt(val, ',');
						pc->stats.mental = eatFirstInt(val, ',');
						pc->stats.offense = eatFirstInt(val, ',');
						pc->stats.defense = eatFirstInt(val, ',');
					}
					else if (key == "gold") {
						menu->inv->gold = atoi(val.c_str());
					}
					else if (key == "equipped") {
						val = val + ",";
						for (int i=0; i<MAX_EQUIPPED; i++) {
							menu->inv->inventory[EQUIPMENT][i].item = eatFirstInt(val, ',');
							if( menu->inv->inventory[EQUIPMENT][i].item != 0) menu->inv->inventory[EQUIPMENT][i].quantity = 1;
							else menu->inv->inventory[EQUIPMENT][i].quantity = 0;
						}
					}
					else if (key == "equipped_quantity") {
						val = val + ",";
						for (int i=0; i<MAX_EQUIPPED; i++) {
							menu->inv->inventory[EQUIPMENT][i].quantity = eatFirstInt(val, ',');
						}
					}
					else if (key == "carried") {
						val = val + ",";
						for (int i=0; i<MAX_CARRIED; i++) {
							menu->inv->inventory[CARRIED][i].item = eatFirstInt(val, ',');
							if( menu->inv->inventory[CARRIED][i].item != 0) menu->inv->inventory[CARRIED][i].quantity = 1;
							else menu->inv->inventory[CARRIED][i].quantity = 0;
						}
					}
					else if (key == "carried_quantity") {
						val = val + ",";
						for (int i=0; i<MAX_CARRIED; i++) {
							menu->inv->inventory[CARRIED][i].quantity = eatFirstInt(val, ',');
						}
					}
					else if (key == "spawn") {
						val = val + ",";
						map->teleport_mapname = eatFirstString(val, ',');
						map->teleport_destination.x = eatFirstInt(val, ',') * UNITS_PER_TILE + UNITS_PER_TILE/2;
						map->teleport_destination.y = eatFirstInt(val, ',') * UNITS_PER_TILE + UNITS_PER_TILE/2;
						map->teleportation = true;
						
						// prevent spawn.txt from putting us on the starting map
						map->clearEvents();
					}
					else if (key == "actionbar") {
						val = val + ",";
						for (int i=0; i<12; i++)
							hotkeys[i] = eatFirstInt(val, ',');
						menu->act->set(hotkeys);
					}
					else if (key == "campaign") {
						camp->setAll(val);
					}
				}
			}
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
