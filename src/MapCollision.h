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

/*
 * MapCollision.h
 * RPGEngine
 *
 * Handle collisions between objects and the map
 */

#ifndef MAP_COLLISION_H
#define MAP_COLLISION_H

#include "Utils.h"

#include <algorithm>
#include <cstdlib>
#include <vector>

// collision tile types
const int BLOCKS_ALL = 1;
const int BLOCKS_MOVEMENT = 2;
const int BLOCKS_ALL_HIDDEN = 3;
const int BLOCKS_MOVEMENT_HIDDEN = 4;

// collision check types
const int CHECK_MOVEMENT = 1;
const int CHECK_SIGHT = 2;

class MapCollision {
private:

	bool line_check(int x1, int y1, int x2, int y2, int checktype);

public:
	MapCollision();
	~MapCollision();
	void setmap(unsigned short _colmap[256][256]);
	bool move(int &x, int &y, int step_x, int step_y, int dist);
	bool outsideMap(int tile_x, int tile_y);
	bool is_empty(int x, int y);
	int is_one_step_around(int x, int y, int xidr, int ydir);
	bool is_wall(int x, int y);

	bool line_of_sight(int x1, int y1, int x2, int y2);
	bool line_of_movement(int x1, int y1, int x2, int y2);
	bool compute_path(Point start, Point end, std::vector<Point> &path, unsigned int limit = 100);

	unsigned short colmap[256][256];
	Point map_size;

	int result_x;
	int result_y;

private:
	bool inline is_sidestepable(int tile_x, int tile_y, int offx2, int offy2);
};

#endif
