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
 * class MenuCharacter
 */

#ifndef MENU_CHARACTER_H
#define MENU_CHARACTER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "WidgetTooltip.h"
#include "InputState.h"
#include "WidgetButton.h"
#include "SharedResources.h"
#include "WidgetLabel.h"
#include <string>
#include <sstream>

const int CSTAT_NAME = 0;
const int CSTAT_LEVEL = 1;
const int CSTAT_PHYSICAL = 2;
const int CSTAT_HP = 3;
const int CSTAT_HPREGEN = 4;
const int CSTAT_MENTAL = 5;
const int CSTAT_MP = 6;
const int CSTAT_MPREGEN = 7;
const int CSTAT_OFFENSE = 8;
const int CSTAT_ACCURACYV1 = 9;
const int CSTAT_ACCURACYV5 = 10;
const int CSTAT_DEFENSE = 11;
const int CSTAT_AVOIDANCEV1 = 12;
const int CSTAT_AVOIDANCEV5 = 13;
const int CSTAT_DMGMAIN = 14;
const int CSTAT_DMGRANGED = 15;
const int CSTAT_CRIT = 16;
const int CSTAT_ABSORB = 17;
const int CSTAT_FIRERESIST = 18;
const int CSTAT_ICERESIST = 19;
const int CSTAT_COUNT = 20;

const int CPROF_P2 = 0;
const int CPROF_P3 = 1;
const int CPROF_P4 = 2;
const int CPROF_P5 = 3;
const int CPROF_M2 = 4;
const int CPROF_M3 = 5;
const int CPROF_M4 = 6;
const int CPROF_M5 = 7;
const int CPROF_O2 = 8;
const int CPROF_O3 = 9;
const int CPROF_O4 = 10;
const int CPROF_O5 = 11;
const int CPROF_D2 = 12;
const int CPROF_D3 = 13;
const int CPROF_D4 = 14;
const int CPROF_D5 = 15;
const int CPROF_COUNT = 16;

struct CharStat {
	WidgetLabel *label;
	WidgetLabel *value;
	SDL_Rect hover;
	TooltipData tip;
	
	void setHover(int x, int y, int w, int h) {
		hover.x=x;
		hover.y=y;
		hover.w=w;
		hover.h=h;
	}
};

struct CharProf {
	SDL_Rect hover;
	TooltipData tip;
	
	void setHover(int x, int y, int w, int h) {
		hover.x=x;
		hover.y=y;
		hover.w=w;
		hover.h=h;
	}
};

class MenuCharacter {
private:
	StatBlock *stats;

	SDL_Surface *background;
	SDL_Surface *proficiency;
	SDL_Surface *upgrade;
	WidgetButton *closeButton;	
	WidgetLabel *labelCharacter;
	CharStat cstat[CSTAT_COUNT];
	CharProf cprof[CPROF_COUNT];

	void displayProficiencies(int value, int y);
	void loadGraphics();
	int bonusColor(int stat);
	
public:
	MenuCharacter(StatBlock *stats);
	~MenuCharacter();
	void logic();
	void render();
	void refreshStats();
	TooltipData checkTooltip();
	bool checkUpgrade();

	bool visible;
    bool newPowerNotification;

};

#endif
