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
 * GameStateLoad
 */

#include "Avatar.h"
#include "SDL_gfxBlitFunc.h"
#include "FileParser.h"
#include "GameStateLoad.h"
#include "GameStateTitle.h"
#include "GameStatePlay.h"
#include "GameStateNew.h"
#include "ItemManager.h"
#include "MenuConfirm.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"

#include <algorithm>

using namespace std;


GameStateLoad::GameStateLoad() : GameState() {
	items = new ItemManager();
	portrait = NULL;
	loading_requested = false;
	loading = false;
	loaded = false;

	label_loading = new WidgetLabel();

	for (int i = 0; i < GAME_SLOT_MAX; i++) {
		label_name[i] = new WidgetLabel();
		label_level[i] = new WidgetLabel();
		label_map[i] = new WidgetLabel();
	}

	// Confirmation box to confirm deleting
	confirm = new MenuConfirm(msg->get("Delete Save"), msg->get("Delete this save?"));
	button_exit = new WidgetButton("images/menus/buttons/button_default.png");
	button_exit->label = msg->get("Exit to Title");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	button_exit->refresh();

	button_action = new WidgetButton("images/menus/buttons/button_default.png");
	button_action->label = msg->get("Choose a Slot");
	button_action->enabled = false;

	button_alternate = new WidgetButton("images/menus/buttons/button_default.png");
	button_alternate->label = msg->get("Delete Save");
	button_alternate->enabled = false;

	// Read positions from config file
	FileParser infile;

	if (infile.open(mods->locate("menus/gameload.txt"))) {
	  while (infile.next()) {
		infile.val = infile.val + ',';

		if (infile.key == "action_button") {
			button_action->pos.x = eatFirstInt(infile.val, ',');
			button_action->pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "atlernate_button") {
			button_alternate->pos.x = eatFirstInt(infile.val, ',');
			button_alternate->pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "portrait") {
			portrait_pos.x = eatFirstInt(infile.val, ',');
			portrait_pos.y = eatFirstInt(infile.val, ',');
			portrait_pos.w = eatFirstInt(infile.val, ',');
			portrait_pos.h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "gameslot") {
			gameslot_pos.x = eatFirstInt(infile.val, ',');
			gameslot_pos.y = eatFirstInt(infile.val, ',');
			gameslot_pos.w = eatFirstInt(infile.val, ',');
			gameslot_pos.h = eatFirstInt(infile.val, ',');
		} else if (infile.key == "preview") {
			preview_pos.x = eatFirstInt(infile.val, ',');
			preview_pos.y = eatFirstInt(infile.val, ',');
			preview_pos.w = eatFirstInt(infile.val, ',');
			preview_pos.h = eatFirstInt(infile.val, ',');
		// label positions within each slot
		} else if (infile.key == "name") {
			name_pos = eatLabelInfo(infile.val);
		} else if (infile.key == "level") {
			level_pos = eatLabelInfo(infile.val);
		} else if (infile.key == "map") {
			map_pos = eatLabelInfo(infile.val);
		} else if (infile.key == "loading_label") {
			loading_pos = eatLabelInfo(infile.val);
		// Position for the avatar preview image in each slot
		} else if (infile.key == "sprite") {
			sprites_pos.x = eatFirstInt(infile.val, ',');
			sprites_pos.y = eatFirstInt(infile.val, ',');
		}
	  }
	  infile.close();
	}

	// Load the MenuConfirm positions and alignments from menus/menus.txt
	if (infile.open(mods->locate("menus/menus.txt"))) {
		int menu_index = -1;
		while (infile.next()) {
			if (infile.key == "id") {
				if (infile.val == "confirm") menu_index = 0;
				else menu_index = -1;
			}

			if (menu_index == -1)
				continue;

			if (infile.key == "layout") {
				infile.val = infile.val + ',';
				confirm->window_area.x = eatFirstInt(infile.val, ',');
				confirm->window_area.y = eatFirstInt(infile.val, ',');
				confirm->window_area.w = eatFirstInt(infile.val, ',');
				confirm->window_area.h = eatFirstInt(infile.val, ',');
			}

			if (infile.key == "align") {
				confirm->alignment = infile.val;
			}
		}
		infile.close();
	}

	confirm->align();
	confirm->update();

	// get displayable types list
	bool found_layer = false;
	if (infile.open(mods->locate("engine/hero_options.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if (infile.key == "layer") {
				unsigned dir = eatFirstInt(infile.val,',');
				if (dir != 6) continue;
				else found_layer = true;

				string layer = eatFirstString(infile.val,',');
				while (layer != "") {
					preview_layer.push_back(layer);
					layer = eatFirstString(infile.val,',');
				}
			}
		}
		infile.close();
	}
	if (!found_layer) fprintf(stderr, "Warning: Could not find layers for direction 6\n");

	button_action->pos.x += (VIEW_W - FRAME_W)/2;
	button_action->pos.y += (VIEW_H - FRAME_H)/2;
	button_action->refresh();

	button_alternate->pos.x += (VIEW_W - FRAME_W)/2;
	button_alternate->pos.y += (VIEW_H - FRAME_H)/2;
	button_alternate->refresh();

	load_game = false;

	for (int i=0; i<GAME_SLOT_MAX; i++) {
		current_map[i] = "";
	}

	loadGraphics();
	readGameSlots();

	for (int i=0; i<GAME_SLOT_MAX; i++) {
		slot_pos[i].x = gameslot_pos.x + (VIEW_W - FRAME_W)/2;
		slot_pos[i].h = gameslot_pos.h;
		slot_pos[i].y = gameslot_pos.y + (VIEW_H - FRAME_H)/2 + (i * gameslot_pos.h);
		slot_pos[i].w = gameslot_pos.w;
	}

	selected_slot = -1;

	// temp
	current_frame = 0;
	frame_ticker = 0;

	color_normal = font->getColor("menu_normal");
}

void GameStateLoad::loadGraphics() {
	background = loadGraphicSurface("images/menus/game_slots.png");
	selection = loadGraphicSurface("images/menus/game_slot_select.png", "Couldn't load image", false, true);
	portrait_border = loadGraphicSurface("images/menus/portrait_border.png", "Couldn't load image", false, true);
}

void GameStateLoad::loadPortrait(int slot) {
	SDL_FreeSurface(portrait);
	portrait = NULL;

	if (slot < 0) return;

	if (stats[slot].name == "") return;

	portrait = loadGraphicSurface("images/portraits/" + stats[slot].portrait + ".png");
}

void GameStateLoad::readGameSlots() {
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		readGameSlot(i);
	}
}

string GameStateLoad::getMapName(const string& map_filename) {
	FileParser infile;
	if (!infile.open(mods->locate("maps/" + map_filename), "")) return "";
	string map_name = "";

	while (map_name == "" && infile.next()) {
		if (infile.key == "title")
			map_name = msg->get(infile.val);
	}

	infile.close();
	return map_name;
}

void GameStateLoad::readGameSlot(int slot) {

	stringstream filename;
	FileParser infile;

	// abort if not a valid slot number
	if (slot < 0 || slot >= GAME_SLOT_MAX) return;

	// save slots are named save#.txt
	filename << PATH_USER;
	if (GAME_PREFIX.length() > 0)
	  filename << GAME_PREFIX << "_";
	filename << "save" << (slot+1) << ".txt";

	if (!infile.open(filename.str(), "")) return;

	while (infile.next()) {

		// load (key=value) pairs
		if (infile.key == "name")
			stats[slot].name = infile.val;
		else if (infile.key == "class")
			stats[slot].character_class = infile.val;
		else if (infile.key == "xp")
			stats[slot].xp = atoi(infile.val.c_str());
		else if (infile.key == "build") {
			stats[slot].physical_character = atoi(infile.nextValue().c_str());
			stats[slot].mental_character = atoi(infile.nextValue().c_str());
			stats[slot].offense_character = atoi(infile.nextValue().c_str());
			stats[slot].defense_character = atoi(infile.nextValue().c_str());
		}
		else if (infile.key == "equipped") {
			string repeat_val = infile.nextValue();
			while (repeat_val != "") {
				equipped[slot].push_back(toInt(repeat_val));
				repeat_val = infile.nextValue();
			}
		}
		else if (infile.key == "option") {
			stats[slot].base = infile.nextValue();
			stats[slot].head = infile.nextValue();
			stats[slot].portrait = infile.nextValue();
		}
		else if (infile.key == "spawn") {
			current_map[slot] = getMapName(infile.nextValue());
		}
	}
	infile.close();

	stats[slot].recalc();
	loadPreview(slot);

}

void GameStateLoad::loadPreview(int slot) {

	vector<string> img_gfx;

	for (unsigned int i=0; i<sprites[slot].size(); i++) {
		if (sprites[slot][i]) SDL_FreeSurface(sprites[slot][i]);
	}
	sprites[slot].clear();

	// fall back to default if it exists
	for (unsigned int i=0; i<preview_layer.size(); i++) {
		bool exists = fileExists(mods->locate("animations/avatar/" + stats[slot].base + "/default_" + preview_layer[i] + ".txt"));
		if (exists) {
			img_gfx.push_back("default_" + preview_layer[i]);
		} else if (preview_layer[i] == "head") {
			img_gfx.push_back(stats[slot].head);
		} else {
			img_gfx.push_back("");
		}
	}

	for (unsigned int i=0; i<equipped[slot].size(); i++) {
		if ((unsigned)equipped[slot][i] > items->items.size()-1){
			fprintf(stderr, "Item with id=%d out of bounds 1-%d. Your savegame is broken or you might use incompatible savegame/mod\nQuitting to avoid savegame rewriting\n", equipped[slot][i], (int)items->items.size()-1);
			SDL_Quit();
			exit(1);
		}
		vector<string>::iterator found = find(preview_layer.begin(), preview_layer.end(), items->items[equipped[slot][i]].type);
		if (equipped[slot][i] > 0 && found != preview_layer.end()) {
			img_gfx[distance(preview_layer.begin(), found)] = items->items[equipped[slot][i]].gfx;
		}
	}

	// composite the hero graphic
	for (unsigned int i=0; i<img_gfx.size(); i++) {
		if (img_gfx[i] == "")
			continue;

		sprites[slot].push_back(NULL);

		if (!TEXTURE_QUALITY) {
			string fname = "images/avatar/" + stats[slot].base + "/preview/noalpha/" + img_gfx[i] + ".png";
			sprites[slot].back() = loadGraphicSurface(fname, "Falling back to alpha version", false, true);
		}
		if (!sprites[slot].back()) {
			sprites[slot].back() = loadGraphicSurface("images/avatar/" + stats[slot].base + "/preview/" + img_gfx[i] + ".png");
		}
	}

}


void GameStateLoad::logic() {

	frame_ticker++;
	if (frame_ticker == 64) frame_ticker = 0;
	if (frame_ticker < 32)
		current_frame = frame_ticker / 8;
	else
		current_frame = (63 - frame_ticker) / 8;

	if (!confirm->visible) {
		if (button_exit->checkClick()) {
			delete requestedGameState;
			requestedGameState = new GameStateTitle();
		}

		if (loading_requested) {
			loading = true;
			loading_requested = false;
			logicLoading();
		}

		if (button_action->checkClick()) {
			if (stats[selected_slot].name == "") {
				// create a new game
				GameStateNew* newgame = new GameStateNew();
				newgame->game_slot = selected_slot + 1;
				requestedGameState = newgame;
			}
			else {
				loading_requested = true;
			}
		}
		if (button_alternate->checkClick()) {
			// Display pop-up to make sure save should be deleted
			confirm->visible = true;
			confirm->render();
		}
		// check clicking game slot
		if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) {
			for (int i=0; i<GAME_SLOT_MAX; i++) {
				if (isWithin(slot_pos[i], inpt->mouse)) {
					inpt->lock[MAIN1] = true;
					selected_slot = i;
					updateButtons();
				}
			}
		}
	} else if (confirm->visible) {
		confirm->logic();
		if (confirm->confirmClicked) {
			stringstream filename;
			filename.str("");
			filename << PATH_USER;
			if (GAME_PREFIX.length() > 0)
			  filename << GAME_PREFIX << "_";
			filename << "save" << (selected_slot+1) << ".txt";

			if (remove(filename.str().c_str()) != 0)
				perror("Error deleting save from path");
			stats[selected_slot] = StatBlock();
			readGameSlot(selected_slot);
			loadPreview(selected_slot);
			loadPortrait(selected_slot);

			updateButtons();

			confirm->visible = false;
			confirm->confirmClicked = false;
		}
	}
}

void GameStateLoad::logicLoading() {
	// load an existing game
	GameStatePlay* play = new GameStatePlay();
	play->resetGame();
	play->game_slot = selected_slot + 1;
	play->loadGame();
	requestedGameState = play;
	loaded = true;
	loading = false;
}

void GameStateLoad::updateButtons() {
	loadPortrait(selected_slot);

	button_action->enabled = true;
	button_action->tooltip = "";
	if (stats[selected_slot].name == "") {
		button_action->label = msg->get("New Game");
		if (!fileExists(mods->locate("maps/spawn.txt"))) {
			button_action->enabled = false;
			button_action->tooltip = msg->get("Enable a story mod to continue");
		}
		button_alternate->enabled = false;
	}
	else {
		button_alternate->enabled = true;
		button_action->label = msg->get("Load Game");
		if (current_map[selected_slot] == "") {
			if (!fileExists(mods->locate("maps/spawn.txt"))) {
				button_action->enabled = false;
				button_action->tooltip = msg->get("Enable a story mod to continue");
			}
		}
	}
	button_action->refresh();
	button_alternate->refresh();
}

void GameStateLoad::render() {

	SDL_Rect src;
	SDL_Rect dest;

	// display background
	src.w = gameslot_pos.w;
	src.h = gameslot_pos.h * GAME_SLOT_MAX;
	src.x = src.y = 0;
	dest.x = slot_pos[0].x;
	dest.y = slot_pos[0].y;
	SDL_BlitSurface(background, &src, screen, &dest);

	// display selection
	if (selected_slot >= 0) {
		src.w = gameslot_pos.w;
		src.h = gameslot_pos.h;
		src.x = src.y = 0;
		SDL_BlitSurface(selection, &src, screen, &slot_pos[selected_slot]);
	}


	// portrait
	if (selected_slot >= 0 && portrait != NULL) {

		src.w = portrait_pos.w;
		src.h = portrait_pos.h;
		dest.x = portrait_pos.x + (VIEW_W - FRAME_W)/2;
		dest.y = portrait_pos.y + (VIEW_H - FRAME_H)/2;

		SDL_BlitSurface(portrait, &src, screen, &dest);
		SDL_BlitSurface(portrait_border, &src, screen, &dest);
	}

	Point label;
	stringstream ss;

	if (loading_requested || loading || loaded) {
		label.x = loading_pos.x + (VIEW_W - FRAME_W)/2;
		label.y = loading_pos.y + (VIEW_H - FRAME_H)/2;

		if ( loaded) {
			label_loading->set(msg->get("Entering game world..."));
		} else {
			label_loading->set(msg->get("Loading saved game..."));
		}

		label_loading->set(label.x, label.y, loading_pos.justify, loading_pos.valign, label_loading->get(), color_normal, loading_pos.font_style);
		label_loading->render();
	}

	// display text
	for (int slot=0; slot<GAME_SLOT_MAX; slot++) {
		if (stats[slot].name != "") {

			// name
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;
			label_name[slot]->set(label.x, label.y, name_pos.justify, name_pos.valign, stats[slot].name, color_normal, name_pos.font_style);
			label_name[slot]->render();

			// level
			ss.str("");
			label.x = slot_pos[slot].x + level_pos.x;
			label.y = slot_pos[slot].y + level_pos.y;
			ss << msg->get("Level %d %s", stats[slot].level, msg->get(stats[slot].character_class));
			label_level[slot]->set(label.x, label.y, level_pos.justify, level_pos.valign, ss.str(), color_normal, level_pos.font_style);
			label_level[slot]->render();

			// map
			label.x = slot_pos[slot].x + map_pos.x;
			label.y = slot_pos[slot].y + map_pos.y;
			label_map[slot]->set(label.x, label.y, map_pos.justify, map_pos.valign, current_map[slot], color_normal, map_pos.font_style);
			label_map[slot]->render();

			// render character preview
			dest.x = slot_pos[slot].x + sprites_pos.x;
			dest.y = slot_pos[slot].y + sprites_pos.y;
			src.x = current_frame * preview_pos.h;
			src.y = 0;
			src.w = src.h = preview_pos.h;

			for (unsigned int i=0; i<sprites[slot].size(); i++) {
				SDL_BlitSurface(sprites[slot][i], &src, screen, &dest);
			}
		}
		else {
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;
			label_name[slot]->set(label.x, label.y, name_pos.justify, name_pos.valign, msg->get("Empty Slot"), color_normal, name_pos.font_style);
			label_name[slot]->render();
		}
	}
	// display warnings
	if (confirm->visible) confirm->render();

	// display buttons
	button_exit->render();
	button_action->render();
	button_alternate->render();
}

GameStateLoad::~GameStateLoad() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(selection);
	SDL_FreeSurface(portrait_border);
	SDL_FreeSurface(portrait);
	delete button_exit;
	delete button_action;
	delete button_alternate;
	delete items;
	for (int slot=0; slot<GAME_SLOT_MAX; slot++) {
		for (unsigned int i=0; i<sprites[slot].size(); i++) {
			SDL_FreeSurface(sprites[slot][i]);
		}
		sprites[slot].clear();
	}
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		delete label_name[i];
		delete label_level[i];
		delete label_map[i];
	}
	delete label_loading;
	delete confirm;
}
