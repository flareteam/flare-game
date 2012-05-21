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

#define DISPLAY_DAMAGE 0
#define DISPLAY_CRIT 1
#define DISPLAY_HEAL 2
#define DISPLAY_MISS 3
#define DISPLAY_SHIELD 4

class WIdgetLabel;

struct Combat_Text_Item {
	WidgetLabel *label;
    int lifespan;
    Point pos;
    std::string text;
    int displaytype;
};

class CombatText {
public:
    static CombatText* Instance();
    void render();
    void addMessage(std::string message, Point location, int displaytype);
    void addMessage(int num, Point location, int displaytype);
    void setCam(Point location);

private:
    Point cam;
    std::vector<Combat_Text_Item> combat_text;
    CombatText(){};
    CombatText(CombatText const&){};

    static CombatText* m_pInstance;

};

#endif
