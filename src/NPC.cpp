/**
 * class NPC
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "NPC.h"

NPC::NPC(MapIso *_map, ItemDatabase *_items) : Entity(_map) {
	items = _items;

	// init general vars
	name = "";
	pos.x = pos.y = 0;
	
	// init animation info
	sprites = NULL;
	render_size.x = render_size.y = 0;
	render_offset.x = render_offset.y = 0;
	anim_frames = 0;
	anim_duration = 0;
	current_frame = 0;
	
	// init vendor info
	vendor = false;
	stock.init(NPC_VENDOR_MAX_STOCK, _items);
	stock_count = 0;
	random_stock = 0;
	vox_intro_count = 0;
	
	for (int i=0; i<NPC_MAX_VOX; i++) {
		vox_intro[i] = NULL;
	}

	// init talker info
	portrait = NULL;
	talker = false;

	for (int i=0; i<NPC_MAX_DIALOG; i++) {
		for (int j=0; j<NPC_MAX_EVENTS; j++) {
			dialog[i][j].type = "";
			dialog[i][j].s = "";
			dialog[i][j].x = 0;
			dialog[i][j].y = 0;
			dialog[i][j].z = 0;
		}
	}
	dialog_count = 0;
}

/**
 * NPCs are stored in simple config files
 *
 * @param npc_id Config file loaded at npcs/[npc_id].txt
 */
void NPC::load(string npc_id) {

	ifstream infile;
	string line;
	string key;
	string val;
	string starts_with;
	string section = "";
	ItemStack stack;
	int event_count = 0;
	
	infile.open(("npcs/" + npc_id + ".txt").c_str(), ios::in);

	string filename_sprites = "";
	string filename_portrait = "";

	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);

			if (line.length() > 0) {
				starts_with = line.at(0);
				
				if (starts_with == "#") {
					// skip comments
				}
				else if (starts_with == "[") {
					section = parse_section_title(line);
					if (section == "dialog") {
						dialog_count++;
						event_count = 0;
					}
				}
				else { // this is data.  treatment depends on key
					parse_key_pair(line, key, val);          
					key = trim(key, ' ');
					val = trim(val, ' ');
				
					if (section == "dialog") {
					
						// here we use dialog_count-1 because we've already incremented the dialog count but the array is 0 based
					
						dialog[dialog_count-1][event_count].type = key;
						if (key == "requires_status")
							dialog[dialog_count-1][event_count].s = val;
						else if (key == "requires_not")
							dialog[dialog_count-1][event_count].s = val;
						else if (key == "requires_item")
							dialog[dialog_count-1][event_count].x = atoi(val.c_str());
						else if (key == "him" || key == "her")
							dialog[dialog_count-1][event_count].s = val;
						else if (key == "you")
							dialog[dialog_count-1][event_count].s = val;
						else if (key == "reward_item") {
							// id,count
							dialog[dialog_count-1][event_count].x = eatFirstInt(val, ',');
							dialog[dialog_count-1][event_count].y = atoi(val.c_str());							
						}
						else if (key == "reward_xp")
							dialog[dialog_count-1][event_count].x = atoi(val.c_str());
						else if (key == "reward_currency")
							dialog[dialog_count-1][event_count].x = atoi(val.c_str());
						else if (key == "remove_item")
							dialog[dialog_count-1][event_count].x = atoi(val.c_str());
						else if (key == "set_status")
							dialog[dialog_count-1][event_count].s = val;
						else if (key == "unset_status")
							dialog[dialog_count-1][event_count].s = val;						
						
						event_count++;
					}
					else {
						if (key == "name") {
							name = val;
						}
						else if (key == "level") {
							level = atoi(val.c_str());
						}
						else if (key == "gfx") {
							filename_sprites = val;
						}
						else if (key == "render_size") {
							val = val + ",";
							render_size.x = eatFirstInt(val, ',');
							render_size.y = eatFirstInt(val, ',');
						}
						else if (key == "render_offset") {
							val = val + ",";
							render_offset.x = eatFirstInt(val, ',');
							render_offset.y = eatFirstInt(val, ',');
						}
						else if (key == "anim_frames") {
							anim_frames = atoi(val.c_str());
						}
						else if (key == "anim_duration") {
							anim_duration = atoi(val.c_str());
						}
	
						// handle talkers
						else if (key == "talker") {
							if (val == "true") talker=true;
						}
						else if (key == "portrait") {
							filename_portrait = val;
						}
	
						// handle vendors
						else if (key == "vendor") {
							if (val == "true") vendor=true;
						}
						else if (key == "constant_stock") {
							val = val + ",";
							stack.quantity = 1;
							while (val != "") {
								stack.item = eatFirstInt(val, ',');
								stock.add(stack);
							}
						}
						else if (key == "random_stock") {
							random_stock = atoi(val.c_str());
						}
						
						// handle vocals
						else if (key == "vox_intro") {
						loadSound(val, NPC_VOX_INTRO);
						}
					}
				}
			}
		}
	}
	infile.close();
	loadGraphics(filename_sprites, filename_portrait);
}

void NPC::loadGraphics(string filename_sprites, string filename_portrait) {

	if (filename_sprites != "") {
		sprites = IMG_Load(("images/npcs/" + filename_sprites + ".png").c_str());
		if(!sprites) {
			fprintf(stderr, "Couldn't load NPC sprites: %s\n", IMG_GetError());
		}
	
		SDL_SetColorKey( sprites, SDL_SRCCOLORKEY, SDL_MapRGB(sprites->format, 255, 0, 255) );
	
		// optimize
		SDL_Surface *cleanup = sprites;
		sprites = SDL_DisplayFormatAlpha(sprites);
		SDL_FreeSurface(cleanup);
	}
	if (filename_portrait != "") {
		portrait = IMG_Load(("images/portraits/" + filename_portrait + ".png").c_str());
		if(!portrait) {
			fprintf(stderr, "Couldn't load NPC portrait: %s\n", IMG_GetError());
		}
	
		SDL_SetColorKey( portrait, SDL_SRCCOLORKEY, SDL_MapRGB(portrait->format, 255, 0, 255) );
	
		// optimize
		SDL_Surface *cleanup = portrait;
		portrait = SDL_DisplayFormatAlpha(portrait);
		SDL_FreeSurface(cleanup);
	}
	
}

/**
 * filename assumes the file is in soundfx/npcs/
 * type is a const int enum, see NPC.h
 */
void NPC::loadSound(string filename, int type) {

	if (type == NPC_VOX_INTRO) {
	
		// if too many already loaded, skip this one
		if (vox_intro_count == NPC_MAX_VOX) return;
		vox_intro[vox_intro_count] = Mix_LoadWAV(("soundfx/npcs/" + filename).c_str());
		
		if (vox_intro[vox_intro_count])
			vox_intro_count++;
	}

}

void NPC::logic() {

	// animate
	current_frame++;
	if (current_frame == anim_frames * anim_duration) {
		current_frame = 0;
	}

}

/**
 * type is a const int enum, see NPC.h
 */
bool NPC::playSound(int type) {
	int roll;
	if (type == NPC_VOX_INTRO) {
		if (vox_intro_count == 0) return false;
		roll = rand() % vox_intro_count;
		Mix_PlayChannel(-1, vox_intro[roll], 0);
		return true;
	}
	return false;
}

/**
 * NPCs have a list of dialog nodes
 * The player wants to begin dialog with this NPC
 * Determine the correct dialog node by the place in the story line
 */
int NPC::chooseDialogNode() {

	// NPC dialog nodes are listed in timeline order
	// So check from the bottom of the list up
	// First node we reach that meets requirements is the correct node
	
	for (int i=dialog_count-1; i>=0; i--) {
		for (int j=0; j<NPC_MAX_EVENTS; j++) {
			
			// check requirements
			// break (skip to next dialog node) if any requirement fails
			// if we reach an event that is not a requirement, succeed
			
			if (dialog[i][j].type == "requires_status") {
				if (!map->camp->checkStatus(dialog[i][j].s)) break;
			}
			else if (dialog[i][j].type == "requires_not") {
				if (map->camp->checkStatus(dialog[i][j].s)) break;
			}
			else if (dialog[i][j].type == "requires_item") {
				if (!map->camp->checkItem(dialog[i][j].x)) break;
			}
			else {
				return i;
			}
		}
	}
	return 0;
}


/**
 * Process the current dialog
 *
 * Return false if the dialog has ended
 */
bool NPC::processDialog(int dialog_node, int &event_cursor) {

	stringstream ss;
	ss.str("");
	
	while (event_cursor < NPC_MAX_EVENTS) {
	
		// we've already determined requirements are met, so skip these
		if (dialog[dialog_node][event_cursor].type == "requires_status") {
			// continue to next event component
		}
		else if (dialog[dialog_node][event_cursor].type == "requires_not") {
			// continue to next event component
		}
		else if (dialog[dialog_node][event_cursor].type == "requires_item") {
			// continue to next event component	
		}
		else if (dialog[dialog_node][event_cursor].type == "set_status") {
			map->camp->setStatus(dialog[dialog_node][event_cursor].s);
		}
		else if (dialog[dialog_node][event_cursor].type == "unset_status") {
			map->camp->unsetStatus(dialog[dialog_node][event_cursor].s);
		}
		else if (dialog[dialog_node][event_cursor].type == "him") {
			return true;
		}
		else if (dialog[dialog_node][event_cursor].type == "her") {
			return true;
		}
		else if (dialog[dialog_node][event_cursor].type == "you") {
			return true;
		}
		else if (dialog[dialog_node][event_cursor].type == "reward_xp") {
			map->camp->rewardXP(dialog[dialog_node][event_cursor].x);
		}
		else if (dialog[dialog_node][event_cursor].type == "reward_currency") {
			map->camp->rewardCurrency(dialog[dialog_node][event_cursor].x);
		}
		else if (dialog[dialog_node][event_cursor].type == "reward_item") {
			ItemStack istack;
			istack.item = dialog[dialog_node][event_cursor].x;
			istack.quantity = dialog[dialog_node][event_cursor].y;
			map->camp->rewardItem(istack);
		}
		else if (dialog[dialog_node][event_cursor].type == "remove_item") {
			map->camp->removeItem(dialog[dialog_node][event_cursor].x);
		}
		else if (dialog[dialog_node][event_cursor].type == "") {
			// conversation ends
			return false;
		}
		
		event_cursor++;
	}
	return false;
}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 */
Renderable NPC::getRender() {
	Renderable r;
	r.sprite = sprites;
	r.map_pos.x = pos.x;
	r.map_pos.y = pos.y;
	r.src.x = render_size.x * (current_frame / anim_duration);
	r.src.y = 0;
	r.src.w = render_size.x;
	r.src.h = render_size.y;
	r.offset.x = render_offset.x;
	r.offset.y = render_offset.y;
	r.object_layer = true;

	return r;
}


NPC::~NPC() {
	if (sprites != NULL) SDL_FreeSurface(sprites);
	if (portrait != NULL) SDL_FreeSurface(portrait);
	for (int i=0; i<NPC_MAX_VOX; i++) {
		Mix_FreeChunk(vox_intro[i]);
	}
}
