/*
Copyright © 2011-2012 Clint Bellanger
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
 * class MenuLog
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuLog.h"
#include "ModManager.h"
#include "Settings.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetScrollBox.h"
#include "WidgetTabControl.h"

using namespace std;


MenuLog::MenuLog() {

	visible = false;
	tab_content_indent = 4;

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/log.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "caption") {
				title = eatLabelInfo(infile.val);
			} else if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "tab_area") {
				tab_area.x = eatFirstInt(infile.val,',');
				tab_area.y = eatFirstInt(infile.val,',');
				tab_area.w = eatFirstInt(infile.val,',');
				tab_area.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "tab_bg_color") {
				tab_bg.r = eatFirstInt(infile.val,',');
				tab_bg.g = eatFirstInt(infile.val,',');
				tab_bg.b = eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/log.txt!\n");

	// Store the amount of displayed log messages on each log, and the maximum.
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		msg_buffer[i] = new WidgetScrollBox(tab_area.w,tab_area.h);
		msg_buffer[i]->bg.r = tab_bg.r;
		msg_buffer[i]->bg.g = tab_bg.g;
		msg_buffer[i]->bg.b = tab_bg.b;
	}

	// Initialize the tab control.
	tabControl = new WidgetTabControl(LOG_TYPE_COUNT);

	// Define the header.
	tabControl->setTabTitle(LOG_TYPE_MESSAGES, msg->get("Notes"));
	tabControl->setTabTitle(LOG_TYPE_QUESTS, msg->get("Quests"));

	font->setFont("font_regular");
	paragraph_spacing = font->getLineHeight()/2;

	background = loadGraphicSurface("images/menus/log.png");

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	color_normal = font->getColor("menu_normal");
}

void MenuLog::update() {
	tabControl->setMainArea(window_area.x + tab_area.x, window_area.y + tab_area.y, tab_area.w, tab_area.h);
	tabControl->updateHeader();
	closeButton->pos.x = window_area.x + close_pos.x;
	closeButton->pos.y = window_area.y + close_pos.y;

	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		msg_buffer[i]->pos.x = window_area.x+tab_area.x;
		msg_buffer[i]->pos.y = window_area.y+tab_area.y+tabControl->getTabHeight();
	}
}

/**
 * Perform one frame of logic.
 */
void MenuLog::logic() {
	if(!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
		snd->play(sfx_close);
	}

	tabControl->logic();
	int active_log = tabControl->getActiveTab();
	msg_buffer[active_log]->logic();
}

/**
 * Render graphics for this frame when the menu is open
 */
void MenuLog::render() {

	if (!visible) return;

	SDL_Rect src,dest;

	// Background.
	dest = window_area;
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &window_area);

	// Close button.
	closeButton->render();

	// Text overlay.
	if (!title.hidden) {
		WidgetLabel label;
		label.set(window_area.x+title.x, window_area.y+title.y, title.justify, title.valign, msg->get("Log"), color_normal, title.font_style);
		label.render();
	}

	// Tab control.
	tabControl->render();

	// Display latest log messages for the active tab.

	int total_size = tab_content_indent;
	int active_log = tabControl->getActiveTab();

	if (msg_buffer[active_log]->update) {
		msg_buffer[active_log]->refresh();
		font->setFont("font_regular");
		for (unsigned int i = log_msg[active_log].size(); i > 0; i--) {
			int widthLimit = tabControl->getContentArea().w;
			Point size = font->calc_size(log_msg[active_log][i-1], widthLimit);
			font->renderShadowed(log_msg[active_log][i-1], tab_content_indent, total_size, JUSTIFY_LEFT, msg_buffer[active_log]->contents, widthLimit, color_normal);
			total_size+=size.y+paragraph_spacing;
		}
	}
	msg_buffer[active_log]->update = false;

	msg_buffer[active_log]->render();
}

void MenuLog::refresh(int log_type) {
	int y = tab_content_indent;

	font->setFont("font_regular");
	for (unsigned int i=0; i<log_msg[log_type].size(); i++) {
		int widthLimit = tabControl->getContentArea().w;
		Point size = font->calc_size(log_msg[log_type][i], widthLimit);
		y+=size.y+paragraph_spacing;
	}
	y+=tab_content_indent;

	msg_buffer[log_type]->resize(y);
}

/**
 * Add a new message to the log.
 */
void MenuLog::add(const string& s, int log_type) {
	// If we have too many messages, remove the oldest ones
	while (log_msg[log_type].size() >= MAX_LOG_MESSAGES) {
		log_msg[log_type].erase(log_msg[log_type].begin());
	}

	// Add the new message.
	log_msg[log_type].push_back(s);
	msg_buffer[log_type]->update = true;
	refresh(log_type);

	log_count[log_type]++;
}

/**
 * Remove log message with the given identifier.
 */
void MenuLog::remove(int msg_index, int log_type) {

	log_msg[log_type][msg_index].erase();
	msg_buffer[log_type]->update = true;
	refresh(log_type);

	log_count[log_type]--;
}

void MenuLog::clear(int log_type) {
	log_count[log_type] = 0;
	log_msg[log_type].clear();
	msg_buffer[log_type]->update = true;
	refresh(log_type);
}

void MenuLog::clear() {
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		clear(i);
	}
}

MenuLog::~MenuLog() {

	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		delete msg_buffer[i];
	}

	SDL_FreeSurface(background);
	delete closeButton;
	delete tabControl;
}
