/*
Copyright © 2011-2012 Clint Bellanger

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
 * class NPC
 */

#ifndef NPC_H
#define NPC_H

#include "Entity.h"
#include "ItemStorage.h"
#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>

class ItemManager;
class MapRenderer;

const int NPC_VENDOR_MAX_STOCK = 80;
const int NPC_VOX_INTRO = 0;
const int NPC_VOX_QUEST = 1;
const int NPC_NO_DIALOG_AVAIL = -1;

class NPC : public Entity {
protected:
	ItemManager *items;

public:
	NPC(MapRenderer *_map, ItemManager *_items);
	~NPC();
	void load(const std::string& npc_id, int hero_level);
	void loadGraphics(const std::string& filename_sprites, const std::string& filename_portrait);
	int loadSound(const std::string& filename, int type);
	void logic();
	bool playSound(int type, int id=-1);
	int chooseDialogNode();
	bool processDialog(unsigned int dialog_node, unsigned int& event_cursor);
	virtual Renderable getRender();

	// general info
	std::string name;
	Point pos; // map position
	int level; // used in determining item quality

	int direction;

	// talker info
	SDL_Surface *portrait;
	bool talker;

	// vendor info
	bool vendor;
	ItemStorage stock;
	int stock_count;
	int random_stock;

	// vocals
	std::vector<Mix_Chunk*> vox_intro;
	std::vector<Mix_Chunk*> vox_quests;

	// story and dialog options
	// outer vector is addressing the dialog and the inner vector is
	// addressing the events during one dialog
	std::vector<std::vector<Event_Component> > dialog;
};

#endif
