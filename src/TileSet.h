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

/**
 * class TileSet
 *
 * TileSet storage and file loading
 */
 
#ifndef TILE_SET_H
#define TILE_SET_H

#include <iostream>
#include <fstream>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "Utils.h"

using namespace std;


struct Tile_Def {
	SDL_Rect src;
	Point offset;
};

class TileSet {
private:
	void loadGraphics(string filename);
	
	string current_map;
public:
	// functions
	TileSet();
	~TileSet();
	void load(string filename);
	
	Tile_Def tiles[256];
	SDL_Surface *sprites;


};

#endif
