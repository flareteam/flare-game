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
 * class MenuLog
 */

#include "MenuLog.h"
#include "SharedResources.h"
#include "WidgetLabel.h"

using namespace std;


MenuLog::MenuLog() {

	visible = false;
	
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		for (int j=0; j<MAX_LOG_MESSAGES; j++) {
			msg_buffer[i][j] = NULL;
		}
	}
	active_log = 0;

	tab_labels[LOG_TYPE_MESSAGES] = msg->get("Messages");
	tab_labels[LOG_TYPE_QUESTS] = msg->get("Quests");
	tab_labels[LOG_TYPE_STATISTICS] = msg->get("Statistics");

	// TODO: allow menu size to be configurable
	menu_area.x = 0;
	menu_area.y = (VIEW_H - 416)/2;
	menu_area.w = 320;
	menu_area.h = 416;
	
	list_area.x = menu_area.x + 40;
	list_area.y = menu_area.y + 56;
	list_area.w = 224;
	list_area.h = 328;
	
	tabs_area.x = menu_area.x + 32;
	tabs_area.y = menu_area.y + 30;
	tabs_area.w = 240;
	tabs_area.h = 20;

	tab_padding.y = 4;
	tab_padding.x = 6;
	paragraph_spacing = font->getLineHeight()/2;
	
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		tab_rect[i].y = tabs_area.y;
		tab_rect[i].h = tabs_area.h;
		
		if (i==0) tab_rect[i].x = tabs_area.x;
		else tab_rect[i].x = tab_rect[i-1].x + tab_rect[i-1].w;
		
		tab_rect[i].w = font->calc_width(tab_labels[i]) + tab_padding.x + tab_padding.x;
		
	}
	
	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));
	closeButton->pos.x = 294;
	closeButton->pos.y = (VIEW_H - 480)/2 + 34;
	
}

void MenuLog::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/log.png").c_str());
	tab_active = IMG_Load(mods->locate("images/menus/tab_active.png").c_str());
	tab_inactive = IMG_Load(mods->locate("images/menus/tab_inactive.png").c_str());
	
	
	if(!background || !tab_active || !tab_inactive) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	

	cleanup = tab_active;
	tab_active = SDL_DisplayFormatAlpha(tab_active);
	SDL_FreeSurface(cleanup);	

	cleanup = tab_inactive;
	tab_inactive = SDL_DisplayFormatAlpha(tab_inactive);
	SDL_FreeSurface(cleanup);	
}

/**
 * Perform one frame of logic
 */
void MenuLog::logic() {
	if (!visible) return;
	
	if (closeButton->checkClick()) {
		visible = false;
	}
}

/**
 * Render graphics for this frame when the menu is open
 */
void MenuLog::render() {

	if (!visible) return;
	
	SDL_Rect src;
	
	// background
	src.x = 0;
	src.y = 0;
	src.w = menu_area.w;
	src.h = menu_area.h;
	SDL_BlitSurface(background, &src, screen, &menu_area);
	
	// close button
	closeButton->render();
	
	// text overlay
	WidgetLabel label;
	label.set(menu_area.x+160, menu_area.y+8, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Log"), FONT_WHITE);
	label.render();

	// display tabs
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		renderTab(i);
	}
	
	// display latest log messages
	
	int display_number = 0;
	int total_size = 0;

	// first calculate how many entire messages can fit in the log view
	for (int i=log_count[active_log]-1; i>=0; i--) {
		total_size += msg_buffer[active_log][i]->h + paragraph_spacing;
		if (total_size < list_area.h) display_number++;
		else break;
	}
	
	// now display these messages
	SDL_Rect dest;
	dest.x = list_area.x;
	dest.y = list_area.y;
	
	for (int i=log_count[active_log]-display_number; i<log_count[active_log]; i++) {

		SDL_BlitSurface(msg_buffer[active_log][i], NULL, screen, &dest);
		dest.y += msg_buffer[active_log][i]->h + paragraph_spacing;
	}
}

/**
 * Display the specified tab
 * Render the font and tab background
 * The active tab will look different
 */
void MenuLog::renderTab(int log_type) {
	int i = log_type;
	
	// draw tab background
	SDL_Rect src;
	SDL_Rect dest;
	src.x = src.y = 0;
	dest.x = tab_rect[i].x;
	dest.y = tab_rect[i].y;
	src.w = tab_rect[i].w;
	src.h = tab_rect[i].h;
	
	if (i == active_log)
		SDL_BlitSurface(tab_active, &src, screen, &dest);	
	else
		SDL_BlitSurface(tab_inactive, &src, screen, &dest);	

	// draw tab right edge
	src.x = 128 - tab_padding.x;
	src.w = tab_padding.x;
	dest.x = tab_rect[i].x + tab_rect[i].w - tab_padding.x;
	
	if (i == active_log)
		SDL_BlitSurface(tab_active, &src, screen, &dest);	
	else
		SDL_BlitSurface(tab_inactive, &src, screen, &dest);	
	
	
	// set tab label text color
	int tab_label_color;
	if (i == active_log) tab_label_color = FONT_WHITE;
	else tab_label_color = FONT_GREY;
		
	WidgetLabel label;
	label.set(tab_rect[i].x + tab_padding.x, tab_rect[i].y + tab_padding.y, JUSTIFY_LEFT, VALIGN_TOP, tab_labels[i], tab_label_color);
	label.render();
}

/**
 * Add a new message to the log
 */
void MenuLog::add(const string& s, int log_type) {

	if (log_count[log_type] == MAX_LOG_MESSAGES) {
		remove(0, log_type);
	}
	
	// add new message
	log_msg[log_type][log_count[log_type]] = s;
	
	// render the log entry and store it in a buffer
	Point size = font->calc_size(s, list_area.w);
	msg_buffer[log_type][log_count[log_type]] = createSurface(size.x, size.y);
	font->renderShadowed(s, 0, 0, JUSTIFY_LEFT, msg_buffer[log_type][log_count[log_type]], list_area.w, FONT_WHITE);

	log_count[log_type]++;
}

/**
 * Remove log message with the given id
 */
void MenuLog::remove(int msg_index, int log_type) {

	SDL_FreeSurface(msg_buffer[log_type][msg_index]);
	msg_buffer[log_type][msg_index] = NULL;
		
	for (int i=msg_index; i<MAX_LOG_MESSAGES-1; i++) {
		log_msg[log_type][i] = log_msg[log_type][i+1];
		msg_buffer[log_type][i] = msg_buffer[log_type][i+1];
	}

	log_count[log_type]--;
}

/**
 * Called by MenuManager
 * The tab area was clicked. Change the active tab
 */
void MenuLog::clickTab(Point mouse) {
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		if(isWithin(tab_rect[i], mouse)) {
			active_log = i;
			return;
		}
	}
}

void MenuLog::clear(int log_type) {
	log_count[log_type] = 0;
	for (int i=0; i<MAX_LOG_MESSAGES; i++) {
		SDL_FreeSurface(msg_buffer[log_type][i]);
		msg_buffer[log_type][i] = NULL;
	}
}

void MenuLog::clear() {
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		clear(i);
	}
}

MenuLog::~MenuLog() {

	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		for (int j=0; j<MAX_LOG_MESSAGES; j++) {
			SDL_FreeSurface(msg_buffer[i][j]);
		}
	}

	SDL_FreeSurface(background);
	SDL_FreeSurface(tab_active);
	SDL_FreeSurface(tab_inactive);
	delete closeButton;
}
