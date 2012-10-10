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

#ifndef MENU_HEROCLASS_H
#define MENU_HEROCLASS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "WidgetLabel.h"
#include "WidgetTooltip.h"

#include <string>
#include <sstream>

class MenuActionBar;
class MenuInventory;
class StatBlock;
class WidgetButton;
class WidgetLabel;
class WidgetListBox;

class MenuHeroClass : public Menu {
private:
	StatBlock *stats;
	MenuInventory *inv;
	MenuActionBar *act;

	SDL_Surface *background;
	WidgetButton *confirmButton;
	WidgetLabel *labelHeroClass;
	WidgetListBox *classList;

	void loadGraphics();
	void setClass();
	std::string getItemNames(std::string items);
	std::string getStatIncreases(int index);

	// label and widget positions
	LabelInfo title;
	Point confirm_pos;
	Point classlist_pos;


public:
	MenuHeroClass(StatBlock *_stats, MenuInventory *_inv, MenuActionBar *_act);
	~MenuHeroClass();
	void update();
	void logic();
	void render();

};

#endif
