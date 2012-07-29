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
#include "UtilsParsing.h"
#include "WidgetLabel.h"


using namespace std;


GameStateLoad::GameStateLoad() : GameState() {
	items = new ItemManager();
	portrait = NULL;
	loading_requested = false;
	loading = false;
	loaded = false;

	label_loading = new WidgetLabel();
	label_slots = new WidgetLabel();

	// Confirmation box to confirm deleting
	confirm = new MenuConfirm(msg->get("Delete Save"), msg->get("Delete this save?"));
	button_exit = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_exit->label = msg->get("Exit to Title");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	button_exit->refresh();

	button_action = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_action->label = msg->get("Choose a Slot");
	button_action->enabled = false;

	button_alternate = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_alternate->label = msg->get("Delete Save");
	button_alternate->enabled = false;

	// Read positions from config file 
	FileParser infile;
	int counter = -1;
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
			name_pos.x = eatFirstInt(infile.val, ',');
			name_pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "level") {
			level_pos.x = eatFirstInt(infile.val, ',');
			level_pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "map") {
			map_pos.x = eatFirstInt(infile.val, ',');
			map_pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "sprite") {
			sprites_pos.x = eatFirstInt(infile.val, ',');
			sprites_pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "loading_label") {
			loading_pos.x = eatFirstInt(infile.val, ',');
			loading_pos.y = eatFirstInt(infile.val, ',');
		}
	  }
	  infile.close();
	} else fprintf(stderr, "Unable to open gameload.txt!\n");

	// Load the MenuConfirm positions and alignments from menus/menus.txt
	if (infile.open(mods->locate("menus/menus.txt"))) {
		while (infile.next()) {
			infile.val = infile.val + ',';

			if (infile.key == "confirm") {
				confirm->window_area.x = eatFirstInt(infile.val, ',');
				confirm->window_area.y = eatFirstInt(infile.val, ',');
				confirm->window_area.w = eatFirstInt(infile.val, ',');
				confirm->window_area.h = eatFirstInt(infile.val, ',');
				confirm->alignment = eatFirstString(infile.val, ',');
				confirm->align();
				confirm->update();
				break;
			}
		}
	} else {
		fprintf(stderr, "Unable to open menus.txt!\n");
	}
	infile.close();

	button_action->pos.x += (VIEW_W - FRAME_W)/2;
	button_action->pos.y += (VIEW_H - FRAME_H)/2;
	button_action->refresh();

	button_alternate->pos.x += (VIEW_W - FRAME_W)/2;
	button_alternate->pos.y += (VIEW_H - FRAME_H)/2;
	button_alternate->refresh();

	load_game = false;

	for (int i=0; i<GAME_SLOT_MAX; i++) {
		sprites[i] = NULL;
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
}

void GameStateLoad::loadGraphics() {
	background = NULL;
	selection = NULL;
	portrait_border = NULL;

	background = IMG_Load(mods->locate("images/menus/game_slots.png").c_str());
	selection = IMG_Load(mods->locate("images/menus/game_slot_select.png").c_str());
	portrait_border = IMG_Load(mods->locate("images/menus/portrait_border.png").c_str());
	if(!background || !selection || !portrait_border) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	SDL_SetColorKey( selection, SDL_SRCCOLORKEY, SDL_MapRGB(selection->format, 255, 0, 255) );
	SDL_SetColorKey( portrait_border, SDL_SRCCOLORKEY, SDL_MapRGB(portrait_border->format, 255, 0, 255) );

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = selection;
	selection = SDL_DisplayFormatAlpha(selection);
	SDL_FreeSurface(cleanup);

	cleanup = portrait_border;
	portrait_border = SDL_DisplayFormatAlpha(portrait_border);
	SDL_FreeSurface(cleanup);

}

void GameStateLoad::loadPortrait(int slot) {
	SDL_FreeSurface(portrait);
	portrait = NULL;

	if (stats[slot].name == "") return;

	portrait = IMG_Load(mods->locate("images/portraits/" + stats[slot].portrait + ".png").c_str());
	if (!portrait) return;

	// optimize
	SDL_Surface *cleanup = portrait;
	portrait = SDL_DisplayFormatAlpha(portrait);
	SDL_FreeSurface(cleanup);
}

void GameStateLoad::readGameSlots() {
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		readGameSlot(i);
	}
}

string GameStateLoad::getMapName(const string& map_filename) {
	FileParser infile;
	if (!infile.open(mods->locate("maps/" + map_filename))) return "";
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
	filename << "save" << (slot+1) << ".txt";

	if (!infile.open(PATH_USER + filename.str())) return;

	while (infile.next()) {

		// load (key=value) pairs
		if (infile.key == "name")
			stats[slot].name = infile.val;
		else if (infile.key == "xp")
			stats[slot].xp = atoi(infile.val.c_str());
		else if (infile.key == "build") {
			stats[slot].physical_character = atoi(infile.nextValue().c_str());
			stats[slot].mental_character = atoi(infile.nextValue().c_str());
			stats[slot].offense_character = atoi(infile.nextValue().c_str());
			stats[slot].defense_character = atoi(infile.nextValue().c_str());
		}
		else if (infile.key == "equipped") {
			equipped[slot][0] = atoi(infile.nextValue().c_str());
			equipped[slot][1] = atoi(infile.nextValue().c_str());
			equipped[slot][2] = atoi(infile.nextValue().c_str());
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

	string img_main;
	string img_body;
	string img_off;

	SDL_Surface *gfx_main = NULL;
	SDL_Surface *gfx_off = NULL;
	SDL_Surface *gfx_head = NULL;
	SDL_Rect dest;

	if (equipped[slot][0] != 0)	img_main = items->items[equipped[slot][0]].gfx;
	if (equipped[slot][1] != 0)	img_body = items->items[equipped[slot][1]].gfx;
	else img_body = "clothes";
	if (equipped[slot][2] != 0)	img_off = items->items[equipped[slot][2]].gfx;
	
	// load the body as the base image
	// we'll blit the other layers onto it
	sprites[slot] = IMG_Load(mods->locate("images/avatar/" + stats[slot].base + "/preview/" + img_body + ".png").c_str());

	// composite the hero graphic
	if (img_main != "") gfx_main = IMG_Load(mods->locate("images/avatar/" + stats[slot].base + "/preview/" + img_main + ".png").c_str());
	if (img_off != "") gfx_off = IMG_Load(mods->locate("images/avatar/" + stats[slot].base + "/preview/" + img_off + ".png").c_str());
	gfx_head = IMG_Load(mods->locate("images/avatar/" + stats[slot].base + "/preview/" + stats[slot].head + ".png").c_str());

	if (gfx_main) SDL_SetColorKey(gfx_main, SDL_SRCCOLORKEY, SDL_MapRGB(gfx_main->format, 255, 0, 255));
	if (gfx_off) SDL_SetColorKey(gfx_off, SDL_SRCCOLORKEY, SDL_MapRGB(gfx_off->format, 255, 0, 255));
	if (gfx_head) SDL_SetColorKey(gfx_head, SDL_SRCCOLORKEY, SDL_MapRGB(gfx_head->format, 255, 0, 255));

	dest.x = preview_pos.x;
	dest.y = preview_pos.y;
	dest.w = preview_pos.w;
	dest.h = preview_pos.h;

	if (gfx_main) SDL_gfxBlitRGBA(gfx_main, NULL, sprites[slot], &dest);
	if (gfx_off) SDL_gfxBlitRGBA(gfx_off, NULL, sprites[slot], &dest);
	if (gfx_head) SDL_gfxBlitRGBA(gfx_head, NULL, sprites[slot], &dest);

	if (gfx_main) SDL_FreeSurface(gfx_main);
	if (gfx_off) SDL_FreeSurface(gfx_off);
	if (gfx_head) SDL_FreeSurface(gfx_head);

	// optimize
	if (sprites[slot]) {
		SDL_SetColorKey(sprites[slot], SDL_SRCCOLORKEY, SDL_MapRGB(sprites[slot]->format, 255, 0, 255));
		SDL_Surface *cleanup = sprites[slot];
		sprites[slot] = SDL_DisplayFormatAlpha(sprites[slot]);
		SDL_FreeSurface(cleanup);
	}

}


void GameStateLoad::logic() {

	frame_ticker++;
	if (frame_ticker == 64) frame_ticker = 0;
	if (frame_ticker < 32)
		current_frame = frame_ticker / 8;
	else
		current_frame = (63 - frame_ticker) / 8;

	if (button_exit->checkClick()) {
		delete requestedGameState;
		requestedGameState = new GameStateTitle();
	}

	if(loading_requested) {
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
	if (button_alternate->checkClick())
	{
		// Display pop-up to make sure save should be deleted
		confirm->visible = true;
		confirm->render();
	}
	if (confirm->visible) {
		confirm->logic();
		if(confirm->confirmClicked) {
			stringstream filename;
			filename << PATH_USER << "save" << (selected_slot+1) << ".txt";
			if(remove(filename.str().c_str()) != 0)
				perror("Error deleting save from path");
			stats[selected_slot] = StatBlock();
			readGameSlot(selected_slot);
			loadPreview(selected_slot);
			loadPortrait(selected_slot);

			button_alternate->enabled = false;
			button_alternate->refresh();

			button_action->label = msg->get("New Game");
			button_action->refresh();

			confirm->visible = false;
			confirm->confirmClicked = false;
		}
	}
	// check clicking game slot
	if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) {
		for (int i=0; i<GAME_SLOT_MAX; i++) {
			if (isWithin(slot_pos[i], inpt->mouse)) {
				selected_slot = i;
				inpt->lock[MAIN1] = true;
				loadPortrait(selected_slot);

				button_action->enabled = true;
				if (stats[selected_slot].name == "") {
					button_action->label = msg->get("New Game");
					button_alternate->enabled = false;
				}
				else {
					button_action->label = msg->get("Load Game");
					button_alternate->enabled = true;
				}
				button_action->refresh();
				button_alternate->refresh();

			}
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

void GameStateLoad::render() {

	SDL_Rect src;
	SDL_Rect dest;

	// display buttons
	button_exit->render();
	button_action->render();
	button_alternate->render();

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

	if( loading_requested || loading || loaded ) {
		label.x = loading_pos.x + (VIEW_W - FRAME_W)/2;
		label.y = loading_pos.y + (VIEW_H - FRAME_H)/2;

		if ( loaded ) {
			label_loading->set(msg->get("Entering game world..."));
		} else {
			label_loading->set(msg->get("Loading saved game..."));
		}

		label_loading->set(label.x, label.y, JUSTIFY_CENTER, VALIGN_TOP, label_loading->get(), FONT_WHITE);
		label_loading->render();
	}

	// display text
	for (int slot=0; slot<GAME_SLOT_MAX; slot++) {
		if (stats[slot].name != "") {

			// name
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;
			label_slots->set(label.x, label.y, JUSTIFY_LEFT, VALIGN_TOP, stats[slot].name, FONT_WHITE);
			label_slots->render();

			// level
			ss.str("");
			label.x = slot_pos[slot].x + level_pos.x;
			label.y = slot_pos[slot].y + level_pos.y;
			ss << msg->get("Level %d %s", stats[slot].level, msg->get(stats[slot].character_class));
			label_slots->set(label.x, label.y, JUSTIFY_LEFT, VALIGN_TOP, ss.str(), FONT_WHITE);
			label_slots->render();

			// map
			label.x = slot_pos[slot].x + map_pos.x;
			label.y = slot_pos[slot].y + map_pos.y;
			label_slots->set(label.x, label.y, JUSTIFY_LEFT, VALIGN_TOP, current_map[slot], FONT_WHITE);
			label_slots->render();

			// render character preview
			dest.x = slot_pos[slot].x + sprites_pos.x;
			dest.y = slot_pos[slot].y + sprites_pos.y;
			src.x = current_frame * 128;
			src.y = 0;
			src.w = src.h = 128;

			SDL_BlitSurface(sprites[slot], &src, screen, &dest);
		}
		else {
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;
			label_slots->set(label.x, label.y, JUSTIFY_LEFT, VALIGN_TOP, msg->get("Empty Slot"), FONT_WHITE);
			label_slots->render();
		}
	}
	// display warnings
	if(confirm->visible) confirm->render();
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
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		SDL_FreeSurface(sprites[i]);
	}
	delete label_slots;
	delete label_loading;
	delete confirm;
}
