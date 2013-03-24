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
#include <sstream>
#include "FileParser.h"
#include "Menu.h"
#include "MenuNPCActions.h"
#include "NPC.h"
#include "Settings.h"
#include "SharedResources.h"
#include "SDL_gfxBlitFunc.h"
#include "UtilsParsing.h"

#define SEPARATOR_HEIGHT 2
#define ITEM_SPACING 2
#define MENU_BORDER 8

using namespace std;

class Action {
public:
	Action(std::string _id = "", std::string _label="")
		: id(_id)
		, label(id != "" ? new WidgetLabel() : NULL)
	{
		if (label)
			label->set(_label);
	}

	Action(const Action &r) : label(NULL) {
		id = r.id;
		if (id != "") {
			label = new WidgetLabel();
			label->set(r.label->get());
		}
	}

	virtual ~Action() { delete label; }

	std::string id;
	WidgetLabel *label;
	SDL_Rect rect;
};

MenuNPCActions::MenuNPCActions()
	: Menu()
	, npc(NULL)
	, is_selected(false)
	, is_empty(true)
	, first_dialog_node(-1)
	, current_action(-1)
	, action_menu(NULL)
	, vendor_label(msg->get("Trade"))
	, cancel_label(msg->get("Cancel"))
	, dialog_selected(false)
	, vendor_selected(false)
	, cancel_selected(false)
	, selected_dialog_node(-1)
{
	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/npc.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "background_color") {
				background_color.r = eatFirstInt(infile.val,',');
				background_color.g = eatFirstInt(infile.val,',');
				background_color.b = eatFirstInt(infile.val,',');
				background_alpha = eatFirstInt(infile.val,',');
			}
			else if(infile.key == "topic_normal_color") {
				topic_normal_color.r = eatFirstInt(infile.val,',');
				topic_normal_color.g = eatFirstInt(infile.val,',');
				topic_normal_color.b = eatFirstInt(infile.val,',');
			}
			else if(infile.key == "topic_hilight_color") {
				topic_hilight_color.r = eatFirstInt(infile.val,',');
				topic_hilight_color.g = eatFirstInt(infile.val,',');
				topic_hilight_color.b = eatFirstInt(infile.val,',');
			}
			else if(infile.key == "vendor_normal_color") {
				vendor_normal_color.r = eatFirstInt(infile.val,',');
				vendor_normal_color.g = eatFirstInt(infile.val,',');
				vendor_normal_color.b = eatFirstInt(infile.val,',');
			}
			else if(infile.key == "vendor_hilight_color") {
				vendor_hilight_color.r = eatFirstInt(infile.val,',');
				vendor_hilight_color.g = eatFirstInt(infile.val,',');
				vendor_hilight_color.b = eatFirstInt(infile.val,',');
			}
			else if(infile.key == "cancel_normal_color") {
				cancel_normal_color.r = eatFirstInt(infile.val,',');
				cancel_normal_color.g = eatFirstInt(infile.val,',');
				cancel_normal_color.b = eatFirstInt(infile.val,',');
			}
			else if(infile.key == "cancel_hilight_color") {
				cancel_hilight_color.r = eatFirstInt(infile.val,',');
				cancel_hilight_color.g = eatFirstInt(infile.val,',');
				cancel_hilight_color.b = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/npc.txt!\n");
}

void MenuNPCActions::update() {
	if (action_menu)
		SDL_FreeSurface(action_menu);

	/* get max width and height of action menu */
	int w = 0, h = 0;
	for(size_t i=0; i<npc_actions.size(); i++) {
		h += ITEM_SPACING;
		if (npc_actions[i].label) {
			w = max((int)npc_actions[i].label->bounds.w, w);
			h += npc_actions[i].label->bounds.h;
		}
		else
			h += SEPARATOR_HEIGHT;

		h += ITEM_SPACING;
	}

	/* set action menu position */
	window_area.x = VIEW_W_HALF - (w / 2);
	window_area.y = max(40, VIEW_H_HALF - h - (int)(UNITS_PER_TILE*1.5));
	window_area.w = w;
	window_area.h = h;

	/* update all action menu items */
	int yoffs = MENU_BORDER;
	SDL_Color text_color;
	for(size_t i=0; i<npc_actions.size(); i++) {
		npc_actions[i].rect.x = window_area.x + MENU_BORDER;
		npc_actions[i].rect.y = window_area.y + yoffs;
		npc_actions[i].rect.w = w;

		if (npc_actions[i].label) {
			npc_actions[i].rect.h = npc_actions[i].label->bounds.h + (ITEM_SPACING*2);

			if (i == current_action) {
				if (npc_actions[i].id == "id_cancel")
					text_color = cancel_hilight_color;
				else if (npc_actions[i].id == "id_vendor")
					text_color = vendor_hilight_color;
				else
					text_color = topic_hilight_color;

				npc_actions[i].label->set(MENU_BORDER + (w/2),
							  yoffs + (npc_actions[i].rect.h/2) ,
							  JUSTIFY_CENTER, VALIGN_CENTER,
							  npc_actions[i].label->get(), text_color);
			} else {
				if (npc_actions[i].id == "id_cancel")
					text_color = cancel_normal_color;
				else if (npc_actions[i].id == "id_vendor")
					text_color = vendor_normal_color;
				else
					text_color = topic_normal_color;

				npc_actions[i].label->set(MENU_BORDER + (w/2),
							  yoffs + (npc_actions[i].rect.h/2),
							  JUSTIFY_CENTER, VALIGN_CENTER,
							  npc_actions[i].label->get(), text_color);
			}

		}
		else
			npc_actions[i].rect.h = SEPARATOR_HEIGHT + (ITEM_SPACING*2);

		yoffs += npc_actions[i].rect.h;
	}

	w += (MENU_BORDER*2);
	h += (MENU_BORDER*2);

	/* render action menu surface */
	action_menu = createAlphaSurface(w,h);
	Uint32 bg = SDL_MapRGBA(action_menu->format,
				background_color.r, background_color.g,
				background_color.b, background_alpha);
	SDL_FillRect(action_menu, NULL, bg);

	for(size_t i=0; i<npc_actions.size(); i++) {
	  if (npc_actions[i].label) {
		  npc_actions[i].label->render(action_menu);
	  }
	}

}

void MenuNPCActions::setNPC(NPC *pnpc) {

	// clear actions menu
	npc_actions.clear();

	// reset states
	is_empty = true;
	is_selected = false;
	int topics = 0;
	first_dialog_node = -1;

	npc = pnpc;

	if (npc == NULL)
		return;

	// reset selection
	dialog_selected = vendor_selected = cancel_selected = false;

	/* enumerate available dialog topics */
	std::vector<int> nodes;
	npc->getDialogNodes(nodes);
	for (int i = (int)nodes.size() - 1; i >= 0; i--) {

		if (first_dialog_node == -1 && topics == 0)
			first_dialog_node = nodes[i];

		std::string topic = npc->getDialogTopic(nodes[i]);
		if (topic.length() == 0)
			continue;

		stringstream ss;
		ss.str("");
		ss << "id_dialog_" << nodes[i];

		npc_actions.push_back(Action(ss.str(), topic));
		topics++;
		is_empty = false;
	}

	if (first_dialog_node != -1 && topics == 0)
		topics = 1;

	/* if npc is a vendor add entry */
	if (npc->vendor) {
		if (topics)
			npc_actions.push_back(Action());
		npc_actions.push_back(Action("id_vendor", vendor_label));
		is_empty = false;
	}

	npc_actions.push_back(Action());
	npc_actions.push_back(Action("id_cancel", cancel_label));

	/* if npc is not a vendor and only one topic is
	 available select the topic automatically */
	if (!npc->vendor && topics == 1) {
		dialog_selected = true;
		selected_dialog_node = first_dialog_node;
		is_selected = true;
		return;
	}

	/* if there is no dialogs and npc is a vendor set
	 vendor_selected automatically */
	if (npc->vendor && topics == 0) {
		vendor_selected = true;
		is_selected = true;
		return;
	}

	update();

}

bool MenuNPCActions::empty() {
	return is_empty;
}

bool MenuNPCActions::selection() {
	return is_selected;
}

void MenuNPCActions::logic() {
	if (!visible) return;

	if (inpt->lock[MAIN1])
		return;

	/* get action under mouse */
	bool got_action = false;
	for (size_t i=0; i<npc_actions.size(); i++) {

		if (!isWithin(npc_actions[i].rect, inpt->mouse))
			continue;

		got_action = true;

		if (current_action != i) {
			current_action = i;
			update();
		}

		break;
	}

	/* if we dont have an action under mouse skip main1 check */
	if (!got_action) {
		current_action = -1;
		return;
	}

	/* is main1 pressed */
	if (inpt->pressing[MAIN1]) {
		inpt->lock[MAIN1] = true;


		if (npc_actions[current_action].label == NULL)
			return;
		else if (npc_actions[current_action].id == "id_cancel")
			cancel_selected = true;

		else if (npc_actions[current_action].id == "id_vendor")
			vendor_selected = true;

		else if (npc_actions[current_action].id.compare("id_dialog_")) {
			dialog_selected = true;
			std::stringstream ss;
			std::string tmp(10,' ');
			ss.str(npc_actions[current_action].id);
			ss.read(&tmp[0], 10);
			ss >> selected_dialog_node;
		}

		is_selected = true;
		visible = false;
	}

}

void MenuNPCActions::render() {
	if (!visible) return;

	if (!action_menu) return;

	SDL_BlitSurface(action_menu, NULL, screen, &window_area);

}

MenuNPCActions::~MenuNPCActions() {
	SDL_FreeSurface(action_menu);
}

