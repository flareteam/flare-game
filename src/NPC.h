/**
 * class NPC
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef NPC_H
#define NPC_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <string>
#include <fstream>
#include "Utils.h"
#include "UtilsParsing.h"
#include "ItemDatabase.h"
#include "ItemStorage.h"
#include "MapIso.h"

using namespace std;

const int NPC_VENDOR_MAX_STOCK = 80;
const int NPC_MAX_VOX = 8;
const int NPC_VOX_INTRO = 0;

const int NPC_MAX_DIALOG = 32;
const int NPC_MAX_EVENTS = 16;

class NPC {
private:

	ItemDatabase *items;
	MapIso *map;

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
	Renderable getRender();
	int chooseDialogNode();
	bool processDialog(int dialog_node, int &event_cursor);
	
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
