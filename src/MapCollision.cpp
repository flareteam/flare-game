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

#include "MapCollision.h"
#include "AStarNode.h"
#include <cfloat>

using namespace std;

MapCollision::MapCollision() {
}

void MapCollision::setmap(unsigned short _colmap[256][256]) {
	for (int i=0; i<256; i++) {
		for (int j=0; j<256; j++) {
			colmap[i][j] = _colmap[i][j];
		}
	}
	map_size.x = 0;
	map_size.y = 0;
}

/**
 * Process movement for cardinal (90 degree) and ordinal (45 degree) directions
 * If we encounter an obstacle at 90 degrees, stop.
 * If we encounter an obstacle at 45 or 135 degrees, slide.
 */
bool MapCollision::move(int &x, int &y, int step_x, int step_y, int dist) {

	bool diag = step_x && step_y;

	for (int i = dist; i--;) {
		if (is_empty(x + step_x, y + step_y)) {
			x+= step_x;
			y+= step_y;
		}
		else if (diag && is_empty(x + step_x, y)) { // slide along wall
			x+= step_x;
		}
		else if (diag && is_empty(x, y + step_y)) { // slide along wall
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

bool MapCollision::outsideMap(int tile_x, int tile_y) {
	if (tile_x < 0 || tile_y < 0 || tile_x >= map_size.x || tile_y >= map_size.y) return true;
	return false;
}

bool MapCollision::is_empty(int x, int y) {
	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	// check bounds, then check for collision
	return !outsideMap(tile_x, tile_y) && !colmap[tile_x][tile_y];
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

bool MapCollision::is_wall(int x, int y) {
	int tile_x = x >> TILE_SHIFT; // fast div
	int tile_y = y >> TILE_SHIFT; // fast div

	// bounds check
	if (outsideMap(tile_x, tile_y)) return true;

	if (colmap[tile_x][tile_y] == BLOCKS_ALL || colmap[tile_x][tile_y] == BLOCKS_ALL_HIDDEN)
		return true;
	return false;
}

/**
 * Does not have the "slide" submovement that move() features
 * Line can be arbitrary angles.
 */
bool MapCollision::line_check(int x1, int y1, int x2, int y2, int checktype) {
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


	if (checktype == CHECK_SIGHT) {
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
	else if (checktype == CHECK_MOVEMENT) {
		for (int i=0; i<steps; i++) {
			x += step_x;
			y += step_y;
			if (!is_empty(round(x), round(y))) {
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
	return line_check(x1, y1, x2, y2, CHECK_SIGHT);
}
bool MapCollision::line_of_movement(int x1, int y1, int x2, int y2) {
	return line_check(x1, y1, x2, y2, CHECK_MOVEMENT);

}

/**
* Compute a path from (x1,y1) to (x2,y2)
* Store waypoint inside path
* limit is the maximum number of explored node
* @return true if a path is found
*/
bool MapCollision::compute_path(Point start, Point end, vector<Point> &path, unsigned int limit) {
	// path must be empty
	if(!path.empty())
		path.clear();

	// destination must be valid
	if( !is_empty(end.x,end.y) )
		return false;

	// convert start & end to MapCollision precision
	start = map_to_collision(start);
	end = map_to_collision(end);

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
			if(colmap[neighbour.x][neighbour.y] > 0 || find(close.begin(), close.end(), neighbour)!=close.end())
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

	if( current.x != end.x || current.y != end.y )
		return false;
	else
	{
		// store path from end to start
		path.push_back(collision_to_map(end));
		while( current.x != start.x || current.y != start.y ) {
			path.push_back(collision_to_map(current));
			current = find(close.begin(), close.end(), current)->getParent();
		}
	}

	return !path.empty();
}

MapCollision::~MapCollision() {
}

