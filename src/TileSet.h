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
 * class TileSet
 *
 * TileSet storage and file loading
 */
 
#ifndef TILE_SET_H
#define TILE_SET_H

#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>

#include <string>

struct Tile_Def {
	SDL_Rect src;
	Point offset;
};

class TileSet {
private:
	void loadGraphics(const std::string& filename);
	int alpha_background;
	std::string current_map;

public:
	// functions
	TileSet();
	~TileSet();
	void load(const std::string& filename);
	
	Tile_Def tiles[1024];
	SDL_Surface *sprites;
};

#endif
