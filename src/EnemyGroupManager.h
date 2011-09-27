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
#ifndef ENEMYGROUPMANAGER_H
#define ENEMYGROUPMANAGER_H

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "Settings.h"
#include "MapIso.h"
#include "FileParser.h"
#include "UtilsFileSystem.h"

using namespace std;

struct Enemy_Level {
	string type;
	int level;
};

class EnemyGroupManager {
private:
	// variables
	map <string, vector<Enemy_Level> > category_list; 
	// functions
	void extract_and_sort(string filename);
public:
	// functions
	EnemyGroupManager(/*ARGS WILL GO HERE*/);
	~EnemyGroupManager();
	void generate();
	Enemy_Level random_enemy(string category, int minlevel, int maxlevel);
};

#endif
