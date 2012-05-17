/*
Copyright © 2012 Igor Paliychuk

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
 * GameStateConfig
 *
 * Handle game Settings Menu
 */
//#include "Network.h"
#include "GameStateMultiPlayer.h"
#include "GameStateTitle.h"
#include "GameStateLoad.h"
#include "SharedResources.h"
#include "Settings.h"
#include <sstream>

GameStateMultiPlayer::GameStateMultiPlayer ()
		: GameState(),
		  host_button(NULL),
		  join_button(NULL),
		  cancel_button(NULL)

{
	// Initialize Widgets
	host_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	join_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	ip_setting = new WidgetInput();
	port_setting = new WidgetInput();

	// Define widgets
	host_button->label = msg->get("Host");
	host_button->pos.x = VIEW_W_HALF - host_button->pos.w/2;
	host_button->pos.y = VIEW_H - (cancel_button->pos.h*3);
	host_button->refresh();

    join_button->label = msg->get("Join");
	join_button->pos.x = VIEW_W_HALF - join_button->pos.w/2;
	join_button->pos.y = VIEW_H - (cancel_button->pos.h*2);
	join_button->refresh();

	cancel_button->label = msg->get("Back");
	cancel_button->pos.x = VIEW_W_HALF - cancel_button->pos.w/2;
	cancel_button->pos.y = VIEW_H - (cancel_button->pos.h);
	cancel_button->refresh();

	ip_setting->setPosition((VIEW_W - 640)/2 + 255, (VIEW_H - 640)/2 + 200);
	port_setting->setPosition((VIEW_W - 640)/2 + 255, (VIEW_H - 640)/2 + 400);

	update();
}


GameStateMultiPlayer::~GameStateMultiPlayer()
{
	delete host_button;
	delete join_button;
	delete cancel_button;

	delete ip_setting;
	delete port_setting;
}

void GameStateMultiPlayer::update () {

	//ip_setting->set
	//port_setting->set
}

void GameStateMultiPlayer::logic ()
{
	if (host_button->checkClick()) {
		multiplayer = true;
		isHost = true;
		delete requestedGameState;
		requestedGameState = new GameStateLoad();
	} else if (join_button->checkClick()) {
		multiplayer = true;
		isHost = false;
		delete requestedGameState;
		requestedGameState = new GameStateLoad();
	} else if (cancel_button->checkClick()) {
		delete requestedGameState;
		requestedGameState = new GameStateTitle();
	}
}

void GameStateMultiPlayer::render ()
{
	host_button->render();
	join_button->render();
	cancel_button->render();

	ip_setting->render();
	port_setting->render();
}
