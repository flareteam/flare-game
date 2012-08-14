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

#include "CombatText.h"
#include "SharedResources.h"
#include "Settings.h"
#include <iostream>
#include <sstream>

CombatText::CombatText() {
	color_normal = font->getColor("combat_normal");
	color_crit = font->getColor("combat_crit");
	color_heal = font->getColor("combat_heal");
	color_shield = font->getColor("combat_shield");
}

// Global static pointer used to ensure a single instance of the class.
CombatText* CombatText::m_pInstance = NULL;

CombatText* CombatText::Instance() {
   if (!m_pInstance)
      m_pInstance = new CombatText;

   return m_pInstance;
}

void CombatText::setCam(Point location) {
    cam = location;
}

void CombatText::addMessage(std::string message, Point location, int displaytype) {
    if (COMBAT_TEXT) {
	    Point p = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
        Combat_Text_Item *c = new Combat_Text_Item();
        WidgetLabel *label = new WidgetLabel();
        c->pos = p;
        c->label = label;
        c->text = message;
        c->lifespan = 30;
        c->displaytype = displaytype;
        combat_text.push_back(*c);
        delete c;
    }
}

void CombatText::addMessage(int num, Point location, int displaytype) {
    if (COMBAT_TEXT) {
	    Point p = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
        Combat_Text_Item *c = new Combat_Text_Item();
        WidgetLabel *label = new WidgetLabel();
        c->pos = p;
        c->label = label;

        std::stringstream ss;
        ss << num;
        c->text = ss.str();

        c->lifespan = 30;
        c->displaytype = displaytype;
        combat_text.push_back(*c);
        delete c;
    }
}

void CombatText::render() {
	for(std::vector<Combat_Text_Item>::iterator it = combat_text.begin(); it != combat_text.end(); it++) {
        it->lifespan--;
        it->pos.y--;
        int type = it->displaytype;
        if (type == DISPLAY_DAMAGE)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, color_normal);
        else if (type == DISPLAY_CRIT || type == DISPLAY_MISS)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, color_crit);
        else if (type == DISPLAY_HEAL)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, color_heal);
        else if (type == DISPLAY_SHIELD)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, color_shield);
        if (it->lifespan > 0)
		    it->label->render();
    }
    // delete expired messages
    while (combat_text.size() > 0 && combat_text.begin()->lifespan <= 0) {
        combat_text.erase(combat_text.begin());
    }
}
