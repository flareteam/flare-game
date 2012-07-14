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
#include <vector>

/**
 * Describes a tile by its location \a src in the tileset sprite and
 * by the \a offset to be applied when rendering it on screen.
 * The offset is measured from upper left corner to the logical midpoint
 * of the tile at groundlevel.
 */
struct Tile_Def {
	SDL_Rect src;
	Point offset;
	Tile_Def() {
		src.x = src.y = src.w = src.h = 0;
		offset.x = offset.y = 0;
	}
};

struct Tile_Anim {
	std::vector<Point> pos;
	int frames;
	int current_frame;
	int duration;
	std::vector<int> frame_duration;
	Tile_Anim() {
		frames = 1;
		current_frame = 0;
		duration = 0;
	}
};

class TileSet {
private:
	void loadGraphics(const std::string& filename);
	void reset();

	Uint8 trans_r;
	Uint8 trans_g;
	Uint8 trans_b;
	bool alpha_background;
	std::string current_map;

public:
	// functions
	TileSet();
	~TileSet();
	void load(const std::string& filename);
	void logic();

	std::vector<Tile_Def> tiles;
	std::vector<Tile_Anim> anim;
	SDL_Surface *sprites;
};

#endif
