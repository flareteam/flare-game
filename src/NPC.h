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
 * class NPC
 */

#ifndef NPC_H
#define NPC_H

#include "Entity.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include "Utils.h"
#include "ItemDatabase.h"
#include "ItemStorage.h"
#include "MapIso.h"

using namespace std;

const int NPC_VENDOR_MAX_STOCK = 80;
const int NPC_MAX_VOX = 8;
const int NPC_VOX_INTRO = 0;

const int NPC_MAX_DIALOG = 32;
const int NPC_MAX_EVENTS = 16;

class NPC : public Entity {
protected:
	ItemDatabase *items;

	// animation info
	SDL_Surface *sprites;
	int anim_frames;
	int anim_duration;
	int current_frame;
	
public:
	NPC(MapIso *_map, ItemDatabase *_items);
	~NPC();
	void load(string npc_id);
	void loadGraphics(string filename_sprites, string filename_portrait);
	void loadSound(string filename, int type);
	void logic();
	bool playSound(int type);
	int chooseDialogNode();
	bool processDialog(int dialog_node, int &event_cursor);
	virtual Renderable getRender();
	
	// general info
	string name;
	Point pos; // map position
	int level; // used in determining item quality
	
	// public animation info
	Point render_size;
	Point render_offset;

	// talker info
	SDL_Surface *portrait;
	bool talker;

	// vendor info
	bool vendor;
	ItemStorage stock;
	int stock_count;
	int random_stock;
	
	// vocals
	Mix_Chunk *vox_intro[NPC_MAX_VOX];
	int vox_intro_count;
	
	// story and dialog options
	Event_Component dialog[NPC_MAX_DIALOG][NPC_MAX_EVENTS];
	int dialog_count;
	
};

#endif
