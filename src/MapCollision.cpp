/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller

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

#include "AStarNode.h"
#include "MapCollision.h"
#include "Settings.h"
#include <cfloat>

using namespace std;

MapCollision::MapCollision() {
}

void MapCollision::setmap(const unsigned short _colmap[][256], unsigned short w, unsigned short h) {
	for (int i=0; i<w; i++) {
		for (int j=0; j<h; j++) {
			colmap[i][j] = _colmap[i][j];
		}
	}
	map_size.x = w;
	map_size.y = h;
}

/**
 * Process movement for cardinal (90 degree) and ordinal (45 degree) directions
 * If we encounter an obstacle at 90 degrees, stop.
 * If we encounter an obstacle at 45 or 135 degrees, slide.
 */
bool MapCollision::move(int &x, int &y, int step_x, int step_y, int dist, int movement_type) {

	bool diag = step_x && step_y;

	for (int i = dist; i--;) {
		if (valid_position(x + step_x, y + step_y, movement_type)) {
			x+= step_x;
			y+= step_y;
		}
		else if (diag && valid_position(x + step_x, y, movement_type)) { // slide along wall
			x+= step_x;
		}
		else if (diag && valid_position(x, y + step_y, movement_type)) { // slide along wall
			y+= step_y;
		}
		else { // is there a singular obstacle or corner we can step around?
			// only works if we are moving straight
			if (diag) return false;

			int way_around = is_one_step_around(x, y, step_x, step_y);

			if (!way_around) {
				return false;
			}

			if (step_x) {
				y+= way_around;
			} else {
				x+= way_around;
			}
		}
	}
	return true;
}

/**
 * Determines whether the grid position is outside the map boundary
 */
bool MapCollision::outsideMap(int tile_x, int tile_y) {
	return (tile_x < 0 || tile_y < 0 || tile_x >= map_size.x || tile_y >= map_size.y);
}

/**
 * A map space is empty if it contains no blocking type
 * A position outside the map boundary is not empty
 */
bool MapCollision::is_empty(int x, int y) {
	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	// bounds check
	if (outsideMap(tile_x, tile_y)) return false;

	// collision type check
	return (colmap[tile_x][tile_y] == BLOCKS_NONE);
}

/**
 * A map space is a wall if it contains a wall blocking type (normal or hidden)
 * A position outside the map boundary is a wall
 */
bool MapCollision::is_wall(int x, int y) {
	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	// bounds check
	if (outsideMap(tile_x, tile_y)) return true;

	// collision type check
	return (colmap[tile_x][tile_y] == BLOCKS_ALL || colmap[tile_x][tile_y] == BLOCKS_ALL_HIDDEN);
}

/**
 * Is this a valid tile for an entity with this movement type?
 */
bool MapCollision::valid_tile(int tile_x, int tile_y, int movement_type) {

	// outside the map isn't valid
	if (outsideMap(tile_x,tile_y)) return false;

	// occupied by an entity isn't valid
	if (colmap[tile_x][tile_y] == BLOCKS_ENTITIES) return false;

	// intangible creatures can be everywhere
	if (movement_type == MOVEMENT_INTANGIBLE) return true;

	// flying creatures can't be in walls
	if (movement_type == MOVEMENT_FLYING) {
		return (!(colmap[tile_x][tile_y] == BLOCKS_ALL || colmap[tile_x][tile_y] == BLOCKS_ALL_HIDDEN));
	}

	// normal creatures can only be in empty spaces
	return (colmap[tile_x][tile_y] == BLOCKS_NONE);
}

/**
 * Is this a valid position for an entity with this movement type?
 */
bool MapCollision::valid_position(int x, int y, int movement_type) {

	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	return valid_tile(tile_x, tile_y, movement_type);
}



bool inline MapCollision::is_sidestepable(int tile_x, int tile_y, int offx, int offy) {
	return !outsideMap(tile_x + offx, tile_y + offy) && !colmap[tile_x + offx][tile_y + offy];
}

/**
 * If we have encountered a collision (i.e., is_empty(x, y) already said no), then see if we've
 * hit an object/wall where there is a path around it by one step.  This is to avoid getting
 * "caught" on the corners of a jagged wall.
 *
 * @return if no side-step path exists, the return value is zero.  Otherwise,
 *         it is the coodinate modifier value for the opposite coordinate
 *         (i.e., if xdir was zero and ydir was non-zero, the return value
 *         should be applied to xdir)
 */
int MapCollision::is_one_step_around(int x, int y, int xdir, int ydir) {
	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div
	int ret = 0;

	if (xdir) {
		if (is_sidestepable(tile_x, tile_y, xdir, -1)) {
			ret = 1;
		}
		if (is_sidestepable(tile_x, tile_y, xdir,  1)) {
			ret |= 2;
		}
		if (ret == 3) { // If we can go either way, choose the route that shortest

			// translation: ret = y % UNITS_PER_TILE > UNITS_PER_TILE / 2 ? 1 : -1;
			// realistically, if we were using compile time constants, the compiler
			// would generate pretty much those instructions.
			ret = (y & (UNITS_PER_TILE - 1)) < UNITS_PER_TILE >> 1 ? 1 : -1;
		}
	} else {
		if (is_sidestepable(tile_x, tile_y, -1, ydir)) {
			ret = 1;
		}
		if (is_sidestepable(tile_x, tile_y,  1, ydir)) {
			ret |= 2;
		}
		if (ret == 3) {
			ret = (x & (UNITS_PER_TILE - 1)) < UNITS_PER_TILE >> 1 ? 1 : -1;
		}
	}

	return !ret ? 0 : (ret == 1 ? -1 : 1);
}


/**
 * Does not have the "slide" submovement that move() features
 * Line can be arbitrary angles.
 */
bool MapCollision::line_check(int x1, int y1, int x2, int y2, int check_type, int movement_type) {
	float x = (float)x1;
	float y = (float)y1;
	float dx = (float)abs(x2 - x1);
	float dy = (float)abs(y2 - y1);
	float step_x;
	float step_y;
	int steps = (int)max(dx, dy);


	if (dx > dy) {
		step_x = 1;
		step_y = dy / dx;
	}
	else {
		step_y = 1;
		step_x = dx / dy;
	}
	// fix signs
	if (x1 > x2) step_x = -step_x;
	if (y1 > y2) step_y = -step_y;


	if (check_type == CHECK_SIGHT) {
		for (int i=0; i<steps; i++) {
			x += step_x;
			y += step_y;
			if (is_wall(round(x), round(y))) {
				result_x = round(x -= step_x);
				result_y = round(y -= step_y);
				return false;
			}
		}
	}
	else if (check_type == CHECK_MOVEMENT) {
		for (int i=0; i<steps; i++) {
			x += step_x;
			y += step_y;
			if (!valid_position(round(x), round(y), movement_type)) {
				result_x = round(x -= step_x);
				result_y = round(y -= step_y);
				return false;
			}
		}
	}

	result_x = x2;
	result_y = y2;
	return true;
}

bool MapCollision::line_of_sight(int x1, int y1, int x2, int y2) {
	return line_check(x1, y1, x2, y2, CHECK_SIGHT, 0);
}
bool MapCollision::line_of_movement(int x1, int y1, int x2, int y2, int movement_type) {

	// intangible entities can always move
	if (movement_type == MOVEMENT_INTANGIBLE) return true;

	// if the target is blocking, clear it temporarily
	int tile_x = x2 >> TILE_SHIFT;
	int tile_y = y2 >> TILE_SHIFT;
	bool target_blocks = false;
	if (colmap[tile_x][tile_y] == BLOCKS_ENTITIES) {
		target_blocks = true;
		unblock(x2,y2);
	}

	bool has_movement = line_check(x1, y1, x2, y2, CHECK_MOVEMENT, movement_type);

	if (target_blocks) block(x2,y2);
	return has_movement;

}

/**
* Compute a path from (x1,y1) to (x2,y2)
* Store waypoint inside path
* limit is the maximum number of explored node
* @return true if a path is found
*/
bool MapCollision::compute_path(Point start_pos, Point end_pos, vector<Point> &path, int movement_type, unsigned int limit) {

	// path must be empty
	if (!path.empty())
		path.clear();

	// convert start & end to MapCollision precision
	Point start = map_to_collision(start_pos);
	Point end = map_to_collision(end_pos);

	// if the target square has an entity, temporarily clear it to compute the path
	bool target_blocks = false;
	if (colmap[end.x][end.y] == BLOCKS_ENTITIES) {
		target_blocks = true;
		unblock(end_pos.x, end_pos.y);
	}

	Point current = start;
	AStarNode node(start);
	node.setActualCost(0);
	node.setEstimatedCost(calcDist(start,end));
	node.setParent(current);

	list<AStarNode> open;
	list<AStarNode> close;

	open.push_back(node);

	while( !open.empty() && close.size() < limit ) {
		float lowest_score = FLT_MAX;
		// find lowest score available inside open, make it current node and move it to close
		list<AStarNode>::iterator lowest_it;
		for (list<AStarNode>::iterator it=open.begin(); it != open.end(); ++it) {
			if(it->getFinalCost() < lowest_score) {
				lowest_score = it->getFinalCost();
				lowest_it = it;
			}
		}
		node = *lowest_it;
		current.x = node.getX();
		current.y = node.getY();
		close.push_back(node);
		open.erase(lowest_it);

		if ( current.x == end.x && current.y == end.y )
			break; //path found !

		list<Point> neighbours = node.getNeighbours(256,256); //256 is map max size

		// for every neighbour of current node
		for (list<Point>::iterator it=neighbours.begin(); it != neighbours.end(); ++it)	{
			Point neighbour = *it;

			// if neighbour is not free of any collision, or already in close, skip it
			if(!valid_tile(neighbour.x,neighbour.y,movement_type) || find(close.begin(), close.end(), neighbour)!=close.end())
				continue;

			list<AStarNode>::iterator i = find(open.begin(), open.end(), neighbour);
			// if neighbour isn't inside open, add it as a new Node
			if(i==open.end()) {
				AStarNode newNode(neighbour.x,neighbour.y);
				newNode.setActualCost(node.getActualCost()+calcDist(current,neighbour));
				newNode.setParent(current);
				newNode.setEstimatedCost(calcDist(neighbour,end));
				open.push_back(newNode);
			}
			// else, update it's cost if better
			else if(node.getActualCost()+node_stride < i->getActualCost()) {
				i->setActualCost(node.getActualCost()+node_stride);
				i->setParent(current);
			}
		}
	}

	if( current.x != end.x || current.y != end.y ) {

		// reblock target if needed
		if (target_blocks) block(end_pos.x, end_pos.y);

		return false;
	}
	else
	{
		// store path from end to start
		path.push_back(collision_to_map(end));
		while( current.x != start.x || current.y != start.y ) {
			path.push_back(collision_to_map(current));
			current = find(close.begin(), close.end(), current)->getParent();
		}
	}

	// reblock target if needed
	if (target_blocks) block(end_pos.x, end_pos.y);

	return !path.empty();
}

void MapCollision::block(int x, int y) {

	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	if (colmap[tile_x][tile_y] == BLOCKS_NONE) {
		colmap[tile_x][tile_y] = BLOCKS_ENTITIES;
	}

}

void MapCollision::unblock(int x, int y) {

	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	if (colmap[tile_x][tile_y] == BLOCKS_ENTITIES) {
		colmap[tile_x][tile_y] = BLOCKS_NONE;
	}

}

MapCollision::~MapCollision() {
}

