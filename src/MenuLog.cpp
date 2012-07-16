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

#include "Menu.h"
#include "MenuLog.h"
#include "ModManager.h"
#include "Settings.h"
#include "WidgetButton.h"
#include "WidgetTabControl.h"

using namespace std;


MenuLog::MenuLog() {

	visible = false;

	// Store the amount of displayed log messages on each log, and the maximum.
	for (int i=0; i<LOG_TYPE_COUNT; i++) {
		log_count[i] = 0;
		for (int j=0; j<MAX_LOG_MESSAGES; j++) {
			msg_buffer[i][j] = NULL;
		}
	}

	// Initialize the tab control.
	tabControl = new WidgetTabControl(LOG_TYPE_COUNT);

	// Define the header.
	tabControl->setTabTitle(LOG_TYPE_MESSAGES, msg->get("Messages"));
	tabControl->setTabTitle(LOG_TYPE_QUESTS, msg->get("Quests"));
	tabControl->setTabTitle(LOG_TYPE_STATISTICS, msg->get("Statistics"));

	paragraph_spacing = font->getLineHeight()/2;

	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));
}

void MenuLog::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/log.png").c_str());

	if(!background) {
		fprintf(stderr, "Could not load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

}

void MenuLog::update() {
	tabControl->setMainArea(window_area.x + 32, window_area.y + 30, 240, 348);
	tabControl->updateHeader();
	closeButton->pos.x = window_area.x + 294;
	closeButton->pos.y = window_area.y + 2;
}

/**
 * Perform one frame of logic.
 */
void MenuLog::logic() {
	if(!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}
}

/**
 * Run the logic for the tabs control.
 */
void MenuLog::tabsLogic()
{
	tabControl->logic();
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
	WidgetLabel label;
	label.set(window_area.x+160, window_area.y+8, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Log"), FONT_WHITE);
	label.render();

	// Tab control.
	tabControl->render();

	// Display latest log messages for the active tab.

	int display_number = 0;
	int total_size = 0;
	int active_log = tabControl->getActiveTab();
	SDL_Rect contentArea = tabControl->getContentArea();

	// first calculate how many entire messages can fit in the log view
	for (int i=log_count[active_log]-1; i>=0; i--) {
		total_size += msg_buffer[active_log][i]->h + paragraph_spacing;
		if (total_size < contentArea.h) display_number++;
		else break;
	}

	// Now display these messages.
	for (int i=log_count[active_log]-display_number; i<log_count[active_log]; i++) {
		SDL_BlitSurface(msg_buffer[active_log][i], NULL, screen, &contentArea);
		contentArea.y += msg_buffer[active_log][i]->h + paragraph_spacing;
	}
}

/**
 * Add a new message to the log.
 */
void MenuLog::add(const string& s, int log_type) {

	// Make space if needed.
	if (log_count[log_type] == MAX_LOG_MESSAGES) {
		remove(0, log_type);
	}

	// Add the new message.
	log_msg[log_type][log_count[log_type]] = s;

	// Render the log entry and store it in a buffer.
	int widthLimit = tabControl->getContentArea().w;
	Point size = font->calc_size(s, widthLimit);
	msg_buffer[log_type][log_count[log_type]] = createAlphaSurface(size.x, size.y);
	font->renderShadowed(s, 0, 0, JUSTIFY_LEFT, msg_buffer[log_type][log_count[log_type]], widthLimit, FONT_WHITE);

	log_count[log_type]++;
}

/**
 * Remove log message with the given identifier.
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
	delete closeButton;
	delete tabControl;
}
