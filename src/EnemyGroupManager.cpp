/*
Copyright 2011 Thane Brimhall
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

using namespace std;


EnemyGroupManager* EnemyGroupManager::_instance = 0;


EnemyGroupManager::EnemyGroupManager()
{
	generate();
}

EnemyGroupManager::~EnemyGroupManager()
{
}

EnemyGroupManager& EnemyGroupManager::instance()
{
	if (_instance == 0) {
		_instance = new EnemyGroupManager;
	}
	return *(_instance);
}

void EnemyGroupManager::generate()
{
	string dir = PATH_DATA + string("enemies");
	vector<string> files;
	getFileList(dir, ".txt", files);
	for (int i = 0; i < files.size(); ++i) {
		parseEnemyFileAndStore(dir, files[i]);
	}
}

void EnemyGroupManager::parseEnemyFileAndStore(const string& dir, const string& filename)
{
	FileParser infile;
	if (infile.open(dir + "/" + filename)) {
		Enemy_Level new_enemy;
		new_enemy.type = filename.substr(0, filename.length()-4); //removes the ".txt" from the filename
		while (infile.next()) {
			if (infile.key == "level") {
				new_enemy.level = atoi(infile.val.c_str());
			}
			else if (infile.key == "categories") {
				string cat;
				while ( (cat = infile.nextValue()) != "") {
					_categories[cat].push_back(new_enemy);
				}
			}
		}
	}

	infile.close();
}

Enemy_Level EnemyGroupManager::getRandomEnemy(const std::string& category, int minlevel, int maxlevel) const
{
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
			enemyCandidates.push_back(new_enemy);
		}
	}

	if (enemyCandidates.empty()) {
		return Enemy_Level();
	} else {
		return enemyCandidates[rand() % enemyCandidates.size()];
	}
}
