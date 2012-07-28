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
 * GameStateNew
 *
 * Handle player choices when starting a new game
 * (e.g. character appearance)
 */

#include "Avatar.h"
#include "FileParser.h"
#include "GameStateConfig.h"
#include "GameStateNew.h"
#include "GameStateLoad.h"
#include "GameStatePlay.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetInput.h"
#include "WidgetLabel.h"

using namespace std;


GameStateNew::GameStateNew() : GameState() {
	game_slot = 0;
	option_count = 0;
	current_option = 0;
	portrait_image = NULL;

	// set up buttons
	button_exit = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_exit->label = msg->get("Cancel");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	button_exit->refresh();

	button_create = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_create->label = msg->get("Create Character");
	button_create->pos.x = VIEW_W_HALF + button_create->pos.w/2;
	button_create->pos.y = VIEW_H - button_create->pos.h;
	button_create->enabled = false;
	button_create->refresh();

	button_prev = new WidgetButton(mods->locate("images/menus/buttons/left.png"));
	button_next = new WidgetButton(mods->locate("images/menus/buttons/right.png"));
	input_name = new WidgetInput();
	button_permadeath = new WidgetCheckBox(mods->locate(
												"images/menus/buttons/checkbox_default.png"));

	// Read positions from config file 
	FileParser infile;
	int counter = -1;
	if (infile.open(mods->locate("menus/gamenew.txt"))) {
	  while (infile.next()) {
		infile.val = infile.val + ',';

		if (infile.key == "button_prev") {
			button_prev->pos.x = eatFirstInt(infile.val, ',');
			button_prev->pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "button_next") {
			button_next->pos.x = eatFirstInt(infile.val, ',');
			button_next->pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "button_permadeath") {
			button_permadeath->pos.x = eatFirstInt(infile.val, ',');
			button_permadeath->pos.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "name_input") {
			name.x = eatFirstInt(infile.val, ',');
			name.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "portrait_label") {
			portrait_label.x = eatFirstInt(infile.val, ',');
			portrait_label.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "name_label") {
			name_label.x = eatFirstInt(infile.val, ',');
			name_label.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "permadeath_label") {
			permadeath_label.x = eatFirstInt(infile.val, ',');
			permadeath_label.y = eatFirstInt(infile.val, ',');
		} else if (infile.key == "portrait") {
			portrait_pos.x = eatFirstInt(infile.val, ',');
			portrait_pos.y = eatFirstInt(infile.val, ',');
			portrait_pos.w = eatFirstInt(infile.val, ',');
			portrait_pos.h = eatFirstInt(infile.val, ',');
		}
	  }
	  infile.close();
	} else fprintf(stderr, "Unable to open gamenew.txt!\n");

	button_prev->pos.x += (VIEW_W - 640)/2;
	button_prev->pos.y += (VIEW_H - 480)/2;

	button_next->pos.x += (VIEW_W - 640)/2;
	button_next->pos.y += (VIEW_H - 480)/2;

	name.x += (VIEW_W - 640)/2;
	name.y += (VIEW_H - 480)/2;

	input_name->setPosition(name.x, name.y);

	if (DEFAULT_NAME != "") input_name->setText(DEFAULT_NAME);

	button_permadeath->pos.x += (VIEW_W - 640)/2;
	button_permadeath->pos.y += (VIEW_H - 480)/2;

	portrait_label.x += (VIEW_W - 640)/2;
	portrait_label.y += (VIEW_H - 480)/2;

	name_label.x += (VIEW_W - 640)/2;
	name_label.y += (VIEW_H - 480)/2;

	permadeath_label.x += (VIEW_W - 640)/2;
	permadeath_label.y += (VIEW_H - 480)/2;

	// set up labels
	label_portrait = new WidgetLabel();
	label_portrait->set(portrait_label.x, portrait_label.y, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Choose a Portrait"), FONT_GREY);
	label_name = new WidgetLabel();
	label_name->set(name_label.x, name_label.y, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Choose a Name"), FONT_GREY);
	label_permadeath = new WidgetLabel();
	label_permadeath->set(permadeath_label.x, permadeath_label.y, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Permadeath?"), FONT_GREY);

	loadGraphics();
	loadOptions("hero_options.txt");
	loadPortrait(portrait[0]);
}

void GameStateNew::loadGraphics() {
	portrait_border = NULL;

	portrait_border = IMG_Load(mods->locate("images/menus/portrait_border.png").c_str());
	if(!portrait_border) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	SDL_SetColorKey( portrait_border, SDL_SRCCOLORKEY, SDL_MapRGB(portrait_border->format, 255, 0, 255) );

	// optimize
	SDL_Surface *cleanup = portrait_border;
	portrait_border = SDL_DisplayFormatAlpha(portrait_border);
	SDL_FreeSurface(cleanup);
}

void GameStateNew::loadPortrait(const string& portrait_filename) {
	SDL_FreeSurface(portrait_image);
	portrait_image = NULL;

	portrait_image = IMG_Load(mods->locate("images/portraits/" + portrait_filename + ".png").c_str());
	if (!portrait_image) return;

	// optimize
	SDL_Surface *cleanup = portrait_image;
	portrait_image = SDL_DisplayFormatAlpha(portrait_image);
	SDL_FreeSurface(cleanup);
}

/**
 * Load body type "base" and portrait/head "portrait" options from a config file
 *
 * @param filename File containing entries for option=base,look
 */
void GameStateNew::loadOptions(const string& filename) {
	FileParser fin;
	if (!fin.open(mods->locate("engine/" + filename))) return;

	while (fin.next()) {

		// if at the max allowed base+look options, skip the rest of the file
		// TODO: remove static array size limit
		if (option_count == PLAYER_OPTION_MAX-1) break;

		if (fin.key == "option") {
			base[option_count] = fin.nextValue();
			head[option_count] = fin.nextValue();
			portrait[option_count] = fin.nextValue();
			option_count++;
		}
	}
	fin.close();
}

void GameStateNew::logic() {
	button_permadeath->checkClick();

	// require character name
	if (input_name->getText() == "" && DEFAULT_NAME == "") {
		if (button_create->enabled) {
			button_create->enabled = false;
			button_create->refresh();
		}
	}
	else {
		if (!button_create->enabled) {
			button_create->enabled = true;
			button_create->refresh();
		}
	}

	if (button_exit->checkClick()) {
		delete requestedGameState;
		requestedGameState = new GameStateLoad();
	}

	if (button_create->checkClick()) {
		// start the new game
		GameStatePlay* play = new GameStatePlay();
		play->pc->stats.base = base[current_option];
		play->pc->stats.head = head[current_option];
		play->pc->stats.portrait = portrait[current_option];
		play->pc->stats.name = input_name->getText();
		play->pc->stats.permadeath = button_permadeath->isChecked();
		play->game_slot = game_slot;
		play->resetGame();
		requestedGameState = play;
	}

	// scroll through portrait options
	if (button_next->checkClick()) {
		current_option++;
		if (current_option == option_count) current_option = 0;
		loadPortrait(portrait[current_option]);
	}
	else if (button_prev->checkClick()) {
		current_option--;
		if (current_option == -1) current_option = option_count-1;
		loadPortrait(portrait[current_option]);
	}

	if (DEFAULT_NAME == "") input_name->logic();

}

void GameStateNew::render() {

	// display buttons
	button_exit->render();
	button_create->render();
	button_prev->render();
	button_next->render();
	input_name->render();
	button_permadeath->render();

	// display portrait option
	SDL_Rect src;
	SDL_Rect dest;

	src.w = dest.w = portrait_pos.w;
	src.h = dest.h = portrait_pos.h;
	src.x = src.y = 0;
	dest.x = portrait_pos.x + (VIEW_W - 640)/2;
	dest.y = portrait_pos.y + (VIEW_H - 480)/2;

	if (portrait != NULL) {
		SDL_BlitSurface(portrait_image, &src, screen, &dest);
	}
	if (portrait_border != NULL) {
		SDL_BlitSurface(portrait_border, &src, screen, &dest);
	}

	// display labels
	label_portrait->render();
	if (DEFAULT_NAME == "") label_name->render();
	label_permadeath->render();
}

GameStateNew::~GameStateNew() {
	SDL_FreeSurface(portrait_image);
	SDL_FreeSurface(portrait_border);
	delete button_exit;
	delete button_create;
	delete button_next;
	delete button_prev;
	delete label_portrait;
	delete label_name;
	delete input_name;
	delete button_permadeath;
	delete label_permadeath;
}
