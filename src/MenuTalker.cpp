/*
Copyright 2011 Clint Bellanger and morris989

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
 * class MenuTalker
 */

#include "MenuTalker.h"
#include "SharedResources.h"

MenuTalker::MenuTalker(CampaignManager *_camp) {
	camp = _camp;
	npc = NULL;
	background = NULL;
	portrait = NULL;
	msg_buffer = NULL;

	advanceButton = new WidgetButton(mods->locate("images/menus/buttons/right.png"));
	advanceButton->pos.x = VIEW_W_HALF + 288;
	advanceButton->pos.y = VIEW_H_HALF + 112;

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));
	closeButton->pos.x = VIEW_W_HALF + 288;
	closeButton->pos.y = VIEW_H_HALF + 112;
	
	visible = false;

	// step through NPC dialog nodes
	dialog_node = 0;
	event_cursor = 0;
	accept_lock = false;

	loadGraphics();

}

void MenuTalker::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/dialog_box.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image dialog_box.png: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
	
}

void MenuTalker::chooseDialogNode() {
	event_cursor = 0;
	dialog_node = npc->chooseDialogNode();
	npc->processDialog(dialog_node, event_cursor);
	createBuffer();
}

/**
 * Menu interaction (enter/space/click to continue)
 */
void MenuTalker::logic() {

	if (!visible || npc==NULL) return;
	
	advanceButton->enabled = false;
	closeButton->enabled = false;
	
	// determine active button
	if (event_cursor < NPC_MAX_EVENTS-1) {
		if (npc->dialog[dialog_node][event_cursor+1].type != "") {
			advanceButton->enabled = true;
		}
		else {
			closeButton->enabled = true;
		}
	}
	else {
		closeButton->enabled = true;
	}
	
	bool more;

	if (advanceButton->checkClick() || closeButton->checkClick()) {
		// button was clicked
		event_cursor++;
		more = npc->processDialog(dialog_node, event_cursor);
	}
	else if	(inp->pressing[ACCEPT] && accept_lock) {
		return;
	}
	else if (!inp->pressing[ACCEPT]) {
		accept_lock = false;
		return;
	}
	else {
		accept_lock = true;
		// pressed next/more
		event_cursor++;
		more = npc->processDialog(dialog_node, event_cursor);
	}

	if (more) {
		createBuffer();
	}
	else {
		// end dialog
		npc = NULL;
		visible = false;
	}
}

void MenuTalker::createBuffer() {

	string line;
	
	// speaker name
	string etype = npc->dialog[dialog_node][event_cursor].type;
	if (etype == "him" || etype == "her") {
		line = npc->name + ": ";
	}
	else if (etype == "you") {
		line = hero_name + ": ";
	}
	
	line = line + npc->dialog[dialog_node][event_cursor].s;
	
	// render text to back buffer
	SDL_FreeSurface(msg_buffer);
	msg_buffer = createSurface(576,96);
	font->render(line, 16, 16, JUSTIFY_LEFT, msg_buffer, 544, FONT_WHITE);
	
}

void MenuTalker::render() {
	if (!visible) return;
	SDL_Rect src;
	SDL_Rect dest;
	
	int offset_x = (VIEW_W - 640)/2;
	int offset_y = (VIEW_H - 416)/2;
	
	// dialog box
	src.x = 0;
	src.y = 0;
	dest.x = offset_x;
	dest.y = offset_y + 320;
	src.w = dest.w = 640;
	src.h = dest.h = 96;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// show active portrait
	string etype = npc->dialog[dialog_node][event_cursor].type;
	if (etype == "him" || etype == "her") {
		if (npc->portrait != NULL) {
			src.w = dest.w = 320;
			src.h = dest.h = 320;
			dest.x = offset_x + 32;
			dest.y = offset_y;
			SDL_BlitSurface(npc->portrait, &src, screen, &dest);	
		}
	}
	else if (etype == "you") {
		if (portrait != NULL) {
			src.w = dest.w = 320;
			src.h = dest.h = 320;
			dest.x = offset_x + 288;
			dest.y = offset_y;
			SDL_BlitSurface(portrait, &src, screen, &dest);			
		}
	}
	
	// text overlay
	dest.x = offset_x+32;
	dest.y = offset_y+320;
	SDL_BlitSurface(msg_buffer, NULL, screen, &dest);

	// show advance button if there are more event components, or close button if not
	if (event_cursor < NPC_MAX_EVENTS-1) {
		if (npc->dialog[dialog_node][event_cursor+1].type != "") {
			advanceButton->render();
		}
		else {
			closeButton->render();
		}
	}
	else {
		closeButton->render();
	}
}

void MenuTalker::setHero(string name, string portrait_filename) {
	hero_name = name;
	
	portrait = IMG_Load(mods->locate("images/portraits/" + portrait_filename + ".png").c_str());
	if(!portrait) {
		fprintf(stderr, "Couldn't load portrait: %s\n", IMG_GetError());
		
		// keep playing, just don't show this portrait
	}
	else {
		// optimize
		SDL_Surface *cleanup = portrait;
		portrait = SDL_DisplayFormatAlpha(portrait);
		SDL_FreeSurface(cleanup);
	}
}

MenuTalker::~MenuTalker() {
	SDL_FreeSurface(msg_buffer);
	SDL_FreeSurface(background);
	SDL_FreeSurface(portrait);
	delete advanceButton;
	delete closeButton;
}
