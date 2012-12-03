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

#include "ModManager.h"
#include "Settings.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"
#include <SDL.h>
#include <fstream>
#include <algorithm>

using namespace std;


ModManager::ModManager() {
	loc_cache.clear();
	mod_list.clear();

	loadModList();
}

/**
 * The mod list is in either:
 * 1. [PATH_CONF]/mods.txt
 * 2. [PATH_DATA]/mods/mods.txt
 * The mods.txt file shows priority/load order for mods
 *
 * File format:
 * One mod folder name per line
 * Later mods override previous mods
 */
void ModManager::loadModList() {
	ifstream infile;
	string line;
	string starts_with;
	vector<string> mod_dirs;
	bool found_any_mod = false;

	getDirList(PATH_DATA + "mods", mod_dirs);

	// Add the fallback mod by default
	if (find(mod_dirs.begin(), mod_dirs.end(), FALLBACK_MOD) != mod_dirs.end()) {
		mod_list.push_back(FALLBACK_MOD);
		found_any_mod = true;
	} else {
		fprintf(stderr, "Mod \"%s\" not found, skipping\n", FALLBACK_MOD);
	}

	// Add all other mods.
	string place1 = PATH_CONF + "mods.txt";
	string place2 = PATH_DATA + "mods/mods.txt";

	infile.open(place1.c_str(), ios::in);

	if (!infile.is_open()) {
		infile.open(place2.c_str(), ios::in);
	}
	if (!infile.is_open()) {
		fprintf(stderr, "Error during ModManager::loadModList() -- couldn't open mods.txt, to be located at \n");
		fprintf(stderr, "%s\n%s\n\n", place1.c_str(), place2.c_str());
	}

	while (infile.good()) {
		line = getLine(infile);

		// skip ahead if this line is empty
		if (line.length() == 0) continue;

		// skip comments
		starts_with = line.at(0);
		if (starts_with == "#") continue;

		// add the mod if it exists in the mods folder
		if (find(mod_dirs.begin(), mod_dirs.end(), line) != mod_dirs.end()) {
			mod_list.push_back(line);
			found_any_mod = true;
		} else {
			fprintf(stderr, "Mod \"%s\" not found, skipping\n", line.c_str());
		}
	}
	infile.close();
	if (!found_any_mod && mod_list.size() == 1) {
		fprintf(stderr, "Couldn't locate any Flare mod. ");
		fprintf(stderr, "Check if the game data are installed correctly. ");
		fprintf(stderr, "Expected to find the data in the $XDG_DATA_DIRS path, ");
		fprintf(stderr, "in /usr/local/share/flare/mods, ");
		fprintf(stderr, "or in the same folder as the executable. ");
		fprintf(stderr, "Try placing the mods folder in one of these locations.\n");
	}
}

/**
 * Find the location (mod file name) for this data file.
 * Use private loc_cache to prevent excessive disk I/O
 */
string ModManager::locate(const string& filename) {

	// if we have this location already cached, return it
	if (loc_cache.find(filename) != loc_cache.end()) {
		return loc_cache[filename];
	}

	// search through mods for the first instance of this filename
	string test_path;

	for (unsigned int i = mod_list.size(); i > 0; i--) {
		test_path = PATH_DATA + "mods/" + mod_list[i-1] + "/" + filename;
		if (fileExists(test_path)) {
			loc_cache[filename] = test_path;
			return test_path;
		}
	}

	// all else failing, simply return the filename
	return PATH_DATA + filename;
}

ModManager::~ModManager() {
}
