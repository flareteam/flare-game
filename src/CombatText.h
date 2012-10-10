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

class WidgetLabel;

struct Combat_Text_Item {
	WidgetLabel *label;
	int lifespan;
	Point pos;
	Point src_pos;
	std::string text;
	int displaytype;
	int ydelta;
	bool from_hero;
};

class CombatText {
public:
	static CombatText* Instance();
	void render();
	void addMessage(std::string message, Point location, int displaytype, bool from_hero);
	void addMessage(int num, Point location, int displaytype, bool from_hero);
	void setCam(Point location);

private:
	Point cam;
	std::vector<Combat_Text_Item> combat_text;
	CombatText();
	CombatText(CombatText const&){};

	static CombatText* m_pInstance;

	SDL_Color msg_color[5];
	int duration;
	int speed;
};

#endif
