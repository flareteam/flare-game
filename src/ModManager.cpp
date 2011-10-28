/*
Copyright 2011 Clint Bellanger

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
#include "UtilsFileSystem.h"

ModManager *mods;

ModManager::ModManager() {
	loc_cache.clear();
	mod_list.clear();

	loadModList();
}

/**
 * The mod list is in [PATH_DATA]/mods/mods.txt
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
	
	infile.open((PATH_DATA + "mods/mods.txt").c_str(), ios::in);
	while (!infile.eof()) {
		line = getLine(infile);
		
		// skip ahead if this line is empty
		if (line.length() == 0) continue;
		
		// skip comments
		starts_with = line.at(0);
		if (starts_with == "#") continue;
				
		mod_list.push_back(line);
	}
	infile.close();
}

/**
 * Find the location (mod file name) for this data file.
 * Use private loc_cache to prevent excessive disk I/O
 */
string ModManager::locate(string filename) {

	// if we have this location already cached, return it
	if (loc_cache.find(filename) != loc_cache.end()) {
		return loc_cache[filename];
	}
	
	// search through mods for the first instance of this filename
	
	return filename;
}

ModManager::~ModManager() {
}
