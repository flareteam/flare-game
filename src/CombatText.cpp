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
#include "FileParser.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsParsing.h"
#include <iostream>
#include <sstream>

CombatText::CombatText() {
	msg_color[COMBAT_MESSAGE_GIVEDMG] = font->getColor("combat_givedmg");
	msg_color[COMBAT_MESSAGE_TAKEDMG] = font->getColor("combat_takedmg");
	msg_color[COMBAT_MESSAGE_CRIT] = font->getColor("combat_crit");
	msg_color[COMBAT_MESSAGE_BUFF] = font->getColor("combat_buff");
	msg_color[COMBAT_MESSAGE_MISS] = font->getColor("combat_miss");

	duration = 30;
	speed = 1;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("engine/combat_text.txt"))) {
		while(infile.next()) {
			if(infile.key == "duration") {
				duration = toInt(infile.val);
			} else if(infile.key == "speed") {
				speed = toInt(infile.val);
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/combat_text.txt!\n");

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

void CombatText::addMessage(std::string message, Point location, int displaytype, bool from_hero) {
	if (COMBAT_TEXT) {
		Combat_Text_Item *c = new Combat_Text_Item();
		WidgetLabel *label = new WidgetLabel();
		c->pos = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
		c->src_pos = location;
		c->label = label;
		c->text = message;
		c->lifespan = duration;
		c->displaytype = displaytype;
		c->from_hero = from_hero;
		combat_text.push_back(*c);
		delete c;
	}
}

void CombatText::addMessage(int num, Point location, int displaytype, bool from_hero) {
	if (COMBAT_TEXT) {
		Combat_Text_Item *c = new Combat_Text_Item();
		WidgetLabel *label = new WidgetLabel();
		c->pos = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
		c->src_pos = location;
		c->label = label;
		c->from_hero = from_hero;

		std::stringstream ss;
		ss << num;
		c->text = ss.str();

		c->lifespan = duration;
		c->displaytype = displaytype;
		combat_text.push_back(*c);
		delete c;
	}
}

void CombatText::render() {
	for(std::vector<Combat_Text_Item>::iterator it = combat_text.begin(); it != combat_text.end(); ++it) {
		it->lifespan--;

		// check if we need to position the text relative to the map
		if (!it->from_hero) {
			it->ydelta += speed;
			it->pos = map_to_screen(it->src_pos.x - UNITS_PER_TILE, it->src_pos.y - UNITS_PER_TILE, cam.x, cam.y);
			it->pos.y -= it->ydelta;
		} else {
			it->pos.y -= speed;
		}

		it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, msg_color[it->displaytype]);

		if (it->lifespan > 0)
			it->label->render();

	}
	// delete expired messages
	while (combat_text.size() && combat_text.begin()->lifespan <= 0) {
		combat_text.erase(combat_text.begin());
	}
}
