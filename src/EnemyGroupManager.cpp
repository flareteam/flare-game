/*
Copyright © 2011-2012 Thane Brimhall
		Manuel A. Fernandez Montecelo <manuel.montezelo@gmail.com>

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

#include "EnemyGroupManager.h"
#include "FileParser.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsFileSystem.h"

using namespace std;


EnemyGroupManager* EnemyGroupManager::_instance = 0;


EnemyGroupManager::EnemyGroupManager() {
	generate();
}

EnemyGroupManager::~EnemyGroupManager() {
}

EnemyGroupManager& EnemyGroupManager::instance() {
	if (_instance == 0) {
		_instance = new EnemyGroupManager;
	}
	return *(_instance);
}

void EnemyGroupManager::generate() {

	// load each enemies folder. Individual enemies can be overwritten with mods.
	for (unsigned int i = 0; i < mods->mod_list.size(); i++) {

		string dir = PATH_DATA + "mods/" + mods->mod_list[i] + "/enemies";

		vector<string> files;
		getFileList(dir, ".txt", files);
		for (size_t j = 0; j < files.size(); ++j) {
			parseEnemyFileAndStore(files[j]);
		}
	}
}

void EnemyGroupManager::parseEnemyFileAndStore(const string& filename) {
	FileParser infile;
	if (infile.open(mods->locate("enemies/" + filename))) {
		Enemy_Level new_enemy;
		new_enemy.type = filename.substr(0, filename.length()-4); //removes the ".txt" from the filename
		while (infile.next()) {
			if (infile.key == "level") {
				new_enemy.level = atoi(infile.val.c_str());
			}
			else if (infile.key == "rarity") {
				new_enemy.rarity = infile.val.c_str();
			}
			else if (infile.key == "categories") {
				string cat;
				while ( (cat = infile.nextValue()) != "") {
					_categories[cat].push_back(new_enemy);
				}
			}
		}
		infile.close();
	}
}

Enemy_Level EnemyGroupManager::getRandomEnemy(const std::string& category, int minlevel, int maxlevel) const {
	vector<Enemy_Level> enemyCategory;
	map<string, vector<Enemy_Level> >::const_iterator it = _categories.find(category);
	if (it != _categories.end()) {
		enemyCategory = it->second;
	} else {
		return Enemy_Level();
	}

	// load only the data that fit the criteria
	vector<Enemy_Level> enemyCandidates;
	for (size_t i = 0; i < enemyCategory.size(); ++i) {
		Enemy_Level new_enemy = enemyCategory[i];
		if ((new_enemy.level >= minlevel) && (new_enemy.level <= maxlevel)) {
			// add more than one time to increase chance of getting
			// this enemy as result, "rarity" property
			int add_times = 0;
			if (new_enemy.rarity == "common") {
				add_times = 6;
			} else if (new_enemy.rarity == "uncommon") {
				add_times = 3;
			} else if (new_enemy.rarity == "rare") {
				add_times = 1;
			} else {
				fprintf(stderr,
					"ERROR: 'rarity' property for enemy '%s' not valid (common|uncommon|rare): %s\n",
					new_enemy.type.c_str(), new_enemy.rarity.c_str());
			}

			// do add, the given number of times
			for (int j = 0; j < add_times; ++j) {
				enemyCandidates.push_back(new_enemy);
			}
		}
	}

	if (enemyCandidates.empty()) {
		return Enemy_Level();
	} else {
		return enemyCandidates[rand() % enemyCandidates.size()];
	}
}
