/*
Copyright 2011 Thane Brimhall

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
 * class EnemyGroupManager
 *
 * Loads Enemies into category lists and manages spawning randomized groups of enemies
 */

#include "EnemyGroupManager.h"

EnemyGroupManager::EnemyGroupManager() {
}
EnemyGroupManager::~EnemyGroupManager() {
}


// Fills the array with the enemy data
void EnemyGroupManager::generate() {
	string dir = PATH_DATA + string("enemies");
	vector<string> files = vector<string>();
	getFileList(dir,".txt",files);
	for (int i = 0; i < files.size(); i++) {
		extract_and_sort(files[i]);
	}
}


//NYI
void EnemyGroupManager::extract_and_sort(string filename) {
	FileParser infile;
	Enemy_Level new_enemy;
	vector<string> categories;

	if (infile.open(PATH_DATA + "enemies/" + filename)) {
		new_enemy.type = filename.substr(0,filename.length()-4); //removes the ".txt" from the filename
		while (infile.next()) {
			if(infile.key == "level") {
				new_enemy.level = atoi(infile.val.c_str());
			}
			else if(infile.key == "categories") {
				string cat = "";
				while ( (cat = infile.nextValue()) != "") {
					categories.push_back(cat);
				}
			}
		}
	}
	//push the enemy data into each category it belongs to
	for (int i = 0; i < categories.size(); i++){
		category_list[categories[i]].push_back(new_enemy);
	}

	infile.close();
	return;
}

// Returns a random monster that fits the category and level range
Enemy_Level EnemyGroupManager::random_enemy(string category, int minlevel, int maxlevel) {
	Enemy_Level new_enemy;
	vector<Enemy_Level> enemy_list;
	//load only the data that fit the criteria
	for (int i = 0; i < category_list[category].size(); i++){
		new_enemy = category_list[category][i];
		if ( (new_enemy.level >= minlevel) && (new_enemy.level <= maxlevel)){
			enemy_list.push_back(new_enemy);
		}
	}
	if (enemy_list.size() == 0) return new_enemy;
	return enemy_list[rand() % enemy_list.size()];
}










