/*
Copyright 2011 Clint Bellanger and morris989

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
 * class MenuTalker
 */

#ifndef MENU_TALKER_H
#define MENU_TALKER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Utils.h"
#include "FontEngine.h"
#include "NPC.h"
#include "CampaignManager.h"
#include <string>
#include <sstream>
#include "WidgetButton.h"


class MenuTalker {
private:
	CampaignManager *camp;

	void loadGraphics();
	SDL_Surface *background;
	SDL_Surface *portrait;
	string hero_name;

	int dialog_node;

public:
	MenuTalker(CampaignManager *camp);
	~MenuTalker();

	NPC *npc;
	
	void chooseDialogNode();
	void logic();
	void render();
	void setHero(string name, string portrait_filename);
	
	bool visible;
	int event_cursor;
	bool accept_lock;

	WidgetButton *advanceButton;
	WidgetButton *closeButton;
	
};

#endif
