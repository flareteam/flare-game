/*
Copyright © 2013 Henrik Andersson

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
 * class MenuNPCActions
 */


#pragma once
#ifndef MENU_NPC_ACTIONS_H
#define MENU_NPC_ACTIONS_H

#include <SDL.h>
#include <string>
#include <vector>

class Action;
class NPC;

class MenuNPCActions : public Menu {
private:
	typedef std::vector<Action> ActionsContainer;
	typedef ActionsContainer::iterator ActionsIterator;

	ActionsContainer npc_actions;
	NPC *npc;

	bool is_selected;
	int first_dialog_node;
	int topics;
	size_t current_action;

	SDL_Surface *action_menu;
	SDL_Color topic_normal_color;
	SDL_Color topic_hilight_color;
	SDL_Color vendor_normal_color;
	SDL_Color vendor_hilight_color;
	SDL_Color cancel_normal_color;
	SDL_Color cancel_hilight_color;

	SDL_Color background_color;
	int background_alpha;

public:
	MenuNPCActions();
	~MenuNPCActions();

	void setNPC(NPC *npc);

	void logic();
	void render();
	void update();

	bool selection();

	bool dialog_selected;
	bool vendor_selected;
	bool cancel_selected;
	int selected_dialog_node;

};


#endif
