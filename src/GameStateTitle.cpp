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

#include <iostream>

#include "GameStateLoad.h"
#include "GameStateTitle.h"
#include "GameStateConfig.h"
#include "SharedResources.h"
#include "Settings.h"
#include "WidgetButton.h"
#include "WidgetLabel.h"

GameStateTitle::GameStateTitle() : GameState() {

	exit_game = false;
	load_game = false;

	logo = loadGraphicSurface("images/menus/logo.png");

	// set up buttons
	button_play = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_exit = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	button_cfg = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	button_play->label = msg->get("Play Game");
	button_play->pos.x = VIEW_W_HALF - button_play->pos.w/2;
	button_play->pos.y = VIEW_H - (button_exit->pos.h*3);
	if (!ENABLE_PLAYGAME) {
		button_play->enabled = false;
		button_play->tooltip = msg->get("Enable a core mod to continue");
	}
	button_play->refresh();

	button_cfg->label = msg->get("Configuration");
	button_cfg->pos.x = VIEW_W_HALF - button_cfg->pos.w/2;
	button_cfg->pos.y = VIEW_H - (button_exit->pos.h*2);
	button_cfg->refresh();

	button_exit->label = msg->get("Exit Game");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	button_exit->refresh();

	// set up labels
	label_version = new WidgetLabel();
	label_version->set(VIEW_W, 0, JUSTIFY_RIGHT, VALIGN_TOP, msg->get("Flare Alpha v0.18"), font->getColor("menu_normal"));

	inpt->enableMouseEmulation();
}

void GameStateTitle::logic() {
	button_play->enabled = ENABLE_PLAYGAME;

	snd->logic(Point(0,0));

	if (button_play->checkClick()) {
		delete requestedGameState;
		requestedGameState = new GameStateLoad();
	} else if (button_cfg->checkClick()) {
		delete requestedGameState;
		requestedGameState = new GameStateConfig();
	} else if (button_exit->checkClick()) {
		exitRequested = true;
	}
}

void GameStateTitle::render() {

	SDL_Rect src;
	SDL_Rect dest;

	// display logo centered
	if (logo) {
		src.x = src.y = 0;
		src.w = dest.w = logo->w;
		src.h = dest.h = logo->h;
		dest.x = VIEW_W_HALF - (logo->w/2);
		dest.y = VIEW_H_HALF - (logo->h/2);
		SDL_BlitSurface(logo, &src, screen, &dest);
	}

	// display buttons
	button_play->render();
	button_cfg->render();
	button_exit->render();

	// version number
	label_version->render();
}

GameStateTitle::~GameStateTitle() {
	delete button_play;
	delete button_cfg;
	delete button_exit;
	delete label_version;
	SDL_FreeSurface(logo);
}
