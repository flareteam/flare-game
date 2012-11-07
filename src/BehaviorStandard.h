/*
Copyright © 2012 Clint Bellanger

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

#ifndef BEHAVIOR_STANDARD_H
#define BEHAVIOR_STANDARD_H

#include "EnemyBehavior.h"

// forward declarations
class Enemy;
struct Point;

class BehaviorStandard : public EnemyBehavior {
private:

	// logic steps
	void doUpkeep();
	void findTarget();
	void checkPower();
	void checkMove();
	void updateState();
	
	// targeting vars
	int dist;
	Point pursue_pos;
	
public:
	BehaviorStandard(Enemy *_e);
	void logic();

};

#endif
