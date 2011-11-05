/*
Copyright 2011 Clint Bellanger

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
 * class MenuPowers
 */

#ifndef MENU_POWERS_H
#define MENU_POWERS_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "MenuTooltip.h"
#include "PowerManager.h"
#include "WidgetButton.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>

using namespace std;

class MenuPowers {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;
	StatBlock *stats;
	PowerManager *powers;

	SDL_Surface *background;
	SDL_Surface *powers_step;
	SDL_Surface *powers_unlock;
	WidgetButton *closeButton;
	
	void loadGraphics();
	void displayBuild(int value, int x);

public:
	MenuPowers(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, StatBlock *_stats, PowerManager *_powers);
	~MenuPowers();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool requirementsMet(int power_index);
	int click(Point mouse);
	
	bool visible;
	SDL_Rect slots[20]; // the location of power slots

};

#endif
