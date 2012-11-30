/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2011-2012 Nojan

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
 * class AStarNode
 *
 * A node used within A* algorithm
 */

#ifndef ASTARNODE_H
#define ASTARNODE_H

#include <list>
#include "Utils.h"

const int node_stride = 1; // minimal stride between nodes

class AStarNode
{
protected:
	// position
	int x;
	int y;

	// exact cost from first Node
	float g;
	// cost to last node
	float h;
	// Parent is where this Node come from.
	Point parent;

public:
	AStarNode();
	AStarNode(const int a, const int b);
	AStarNode(const Point &p);
	AStarNode(const AStarNode& copy);

	int getX() const;
	int getY() const;

	Point getParent() const;
	void setParent(const Point& p);

	// return a list of coordinates of all neighbours
	std::list<Point> getNeighbours(int limitX=0, int limitY=0) const;

	float getActualCost() const;
	void setActualCost(const float G);

	void setEstimatedCost(const float H);

	float getFinalCost() const;

	bool operator<(const AStarNode& n) const;
	bool operator==(const AStarNode& n) const;
	bool operator==(const Point& p) const;
	bool operator!=(const Point& p) const;
};

#endif // ASTARNODE_H
