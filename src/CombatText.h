/*
Copyright © 2011-2012 Thane Brimhall

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
 * class CombatText
 *
 * The CombatText class displays floating damage numbers and miss messages
 * above the targets.
 *
 */


#pragma once
#ifndef COMBAT_TEXT_H
#define COMBAT_TEXT_H

#include "Utils.h"
#include "WidgetLabel.h"

#include <vector>
#include <string>

#define COMBAT_MESSAGE_GIVEDMG 0
#define COMBAT_MESSAGE_TAKEDMG 1
#define COMBAT_MESSAGE_CRIT 2
#define COMBAT_MESSAGE_MISS 3
#define COMBAT_MESSAGE_BUFF 4

// TODO: move this to the combat text engine file
// Map positions are between the feet of the entity at the floor level.
// This offset starts combat text "above" the common enemy height.
#define COMBAT_TEXT_STARTING_OFFSET 48;

class WidgetLabel;

class Combat_Text_Item {
public:
	WidgetLabel *label;
	int lifespan;
	Point pos;
	int floating_offset;
	std::string text;
	int displaytype;
};

class CombatText {
public:
	CombatText();

	void render();
	void addMessage(std::string message, Point location, int displaytype);
	void addMessage(int num, Point location, int displaytype);
	void setCam(Point location);

private:
	Point cam;
	std::vector<Combat_Text_Item> combat_text;

	SDL_Color msg_color[5];
	int duration;
	int speed;
};

#endif
