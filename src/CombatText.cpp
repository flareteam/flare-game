/*
Copyright © 2011-2012 Thane Brimhall
Copyright © 2013 Henrik Andersson

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
	}
}

void CombatText::setCam(Point location) {
	cam = location;
}

void CombatText::addMessage(std::string message, Point location, int displaytype) {
	if (COMBAT_TEXT) {
		Combat_Text_Item *c = new Combat_Text_Item();
		WidgetLabel *label = new WidgetLabel();
		c->pos.x = location.x;
		c->pos.y = location.y;
		c->floating_offset = COMBAT_TEXT_STARTING_OFFSET;
		c->label = label;
		c->text = message;
		c->lifespan = duration;
		c->displaytype = displaytype;
		combat_text.push_back(*c);
		delete c;
	}
}

void CombatText::addMessage(int num, Point location, int displaytype) {
	if (COMBAT_TEXT) {
		std::stringstream ss;
		ss << num;
		addMessage(ss.str(), location, displaytype);
	}
}

void CombatText::render() {
	for(std::vector<Combat_Text_Item>::iterator it = combat_text.begin(); it != combat_text.end(); ++it) {

		it->lifespan--;
		it->floating_offset += speed;

		Point scr_pos;
		scr_pos = map_to_screen(it->pos.x, it->pos.y, cam.x, cam.y);
		scr_pos.y -= it->floating_offset;

		it->label->set(scr_pos.x, scr_pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, msg_color[it->displaytype]);

		if (it->lifespan > 0)
			it->label->render();

	}
	// delete expired messages
	while (combat_text.size() && combat_text.begin()->lifespan <= 0) {
		delete combat_text.begin()->label;
		combat_text.erase(combat_text.begin());
	}
}
