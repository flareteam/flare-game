/*
Copyright © 2012 Justin Jacobs

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
 * class MenuHeroClass
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuActionBar.h"
#include "MenuHeroClass.h"
#include "MenuInventory.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetListBox.h"


using namespace std;


MenuHeroClass::MenuHeroClass(StatBlock *_stats, MenuInventory *_inv, MenuActionBar *_act) {
	stats = _stats;
	inv = _inv;
	act = _act;

	visible = false;

	confirmButton = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	// menu title
	labelHeroClass = new WidgetLabel();

	// class list
	classList = new WidgetListBox(HERO_CLASSES.size(), 12, mods->locate("images/menus/buttons/listbox_char.png"));
	classList->can_deselect = false;
	classList->selected[0] = true;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/heroclass.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "caption") {
				title = eatLabelInfo(infile.val);
			} else if(infile.key == "classlist") {
				classlist_pos.x = eatFirstInt(infile.val,',');
				classlist_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "confirm") {
				confirm_pos.x = eatFirstInt(infile.val,',');
				confirm_pos.y = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/heroclass.txt!\n");

	loadGraphics();
}

void MenuHeroClass::update() {

	// confirm button
	confirmButton->pos.x = window_area.x + confirm_pos.x;
	confirmButton->pos.y = window_area.y + confirm_pos.y;
	confirmButton->label = msg->get("OK");
	confirmButton->refresh();

	// menu title
	labelHeroClass->set(window_area.x+title.x, window_area.y+title.y, title.justify, title.valign, msg->get("Pick a class"), font->getColor("menu_normal"), title.font_style);

	// class list
	classList->pos.x = window_area.x+classlist_pos.x;
	classList->pos.y = window_area.y+classlist_pos.y;

	for (unsigned i=0; i<HERO_CLASSES.size(); i++) {
		std::string name;
		std::string tooltip;

		name = msg->get(HERO_CLASSES[i].name);

		tooltip = name;
		if (HERO_CLASSES[i].description != "") tooltip += "\n" + msg->get(HERO_CLASSES[i].description);
		tooltip += getStatIncreases(i);
		tooltip += "\n\n" + msg->get("%d %s", HERO_CLASSES[i].currency, CURRENCY);
		tooltip += getItemNames(HERO_CLASSES[i].equipment);

		classList->append(name,tooltip);
	}

}

void MenuHeroClass::setClass() {
	int index = classList->getSelected();

	// name
	stats->character_class = HERO_CLASSES[index].name;

	// attribute boosts
	stats->physical_character += HERO_CLASSES[index].physical;
	stats->mental_character += HERO_CLASSES[index].mental;
	stats->offense_character += HERO_CLASSES[index].offense;
	stats->defense_character += HERO_CLASSES[index].defense;
	stats->recalc();

	// currency
	inv->currency += HERO_CLASSES[index].currency;

	// equipped items
	inv->inventory[EQUIPMENT].setItems(HERO_CLASSES[index].equipment);
	inv->inventory[EQUIPMENT].fillEquipmentSlots();
	inv->updateEquipment();

	// action bar
	act->set(HERO_CLASSES[index].hotkeys);

	visible = false;
	stats->picked_class = true;
}

std::string MenuHeroClass::getStatIncreases(int index) {
	int physical = HERO_CLASSES[index].physical;
	int mental = HERO_CLASSES[index].mental;
	int offense = HERO_CLASSES[index].offense;
	int defense = HERO_CLASSES[index].defense;

	std::string s;

	if (physical > 0 || mental > 0 || offense > 0 || defense > 0) {
		s += "\n\n";

		if (physical > 0) s += msg->get("+%d Physical", physical) + "\n";
		if (mental > 0) s += msg->get("+%d Mental", mental) + "\n";
		if (offense > 0) s += msg->get("+%d Offense", offense) + "\n";
		if (defense > 0) s += msg->get("+%d Defense", defense) + "\n";

		// Remove the last newline
		s = s.substr(0,s.length()-1);
	}

	return s;
}

std::string MenuHeroClass::getItemNames(std::string items) {
	std::string names;

	if (items.length() > 0) {
		names += "\n\n" + msg->get("Equipment:") + "\n";
		std::string id = "";
		items = items + ',';
		while ((id = eatFirstString(items, ',')) != "") {
			names += inv->getItemName(toInt(id)) + "\n";
		}
		// Remove the last newline
		names = names.substr(0,names.length()-1);
	}

	return names;
}

void MenuHeroClass::loadGraphics() {

	// TODO use a unique background image instead of MenuLog's ?
	background = IMG_Load(mods->locate("images/menus/heroclass.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
		exit(1);
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

}

void MenuHeroClass::logic() {
	if (!stats->picked_class) visible = true;
	else visible = false;

	if (!visible) return;

	if (HERO_CLASSES.size() == 1) setClass();

	if (confirmButton->checkClick()) setClass();

	classList->checkClick();
}

void MenuHeroClass::render() {
	if (!visible) return;

	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);

	// confirm button
	confirmButton->render();

	// title
	labelHeroClass->render();

	// class list
	classList->render();
}


MenuHeroClass::~MenuHeroClass() {
	SDL_FreeSurface(background);
	delete confirmButton;
	delete labelHeroClass;
	delete classList;
}
