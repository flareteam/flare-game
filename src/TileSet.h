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


#pragma once
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
	// Number of frames in this animation. if 0 no animation.
	// 1 makes no sense as it would produce astatic animation.
	unsigned short frames;
	unsigned short current_frame; // is in range 0..(frames-1)
	unsigned short duration; // how long the current frame is already displayed in ticks.
	std::vector<Point> pos; // position of each image.
	std::vector<unsigned short> frame_duration; // duration of each image in ticks. 0 will be treated the same as 1.
	Tile_Anim() {
		frames = 0;
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

	// oversize of the largest tile available, in number of tiles.
	int max_size_x;
	int max_size_y;
};

#endif
