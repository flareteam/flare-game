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
 * class MenuHUDLog
 */

#include "MenuHUDLog.h"
#include "SharedResources.h"
#include "Settings.h"

using namespace std;

MenuHUDLog::MenuHUDLog() {

	log_count = 0;
	list_area.x = 224;
	list_area.y = 416;
	paragraph_spacing = font->getLineHeight()/2;

	for (int i=0; i<MAX_HUD_MESSAGES; i++) {
		msg_buffer[i] = NULL;
		log_msg[i] = "";
		msg_age[i] = 0;
	}
}

/**
 * Calculate how long a given message should remain on the HUD
 * Formula: minimum time plus x frames per character
 */
int MenuHUDLog::calcDuration(const string& s) {
	// 5 seconds plus an extra second per 10 letters
	return FRAMES_PER_SEC * 5 + s.length() * (FRAMES_PER_SEC/10);
}

/**
 * Perform one frame of logic
 * Age messages
 */
void MenuHUDLog::logic() {
	for (int i=0; i<log_count; i++) {
		if (msg_age[i] > 0) msg_age[i]--;
	}
}


/**
 * New messages appear on the screen for a brief time
 */
void MenuHUDLog::render() {

	SDL_Rect dest;
	dest.x = 32;
	dest.y = VIEW_H - 40;


	// go through new messages
	for (int i=log_count-1; i>=0; i--) {
		if (msg_age[i] > 0 && dest.y > 64) {

			dest.y -= msg_buffer[i]->h + paragraph_spacing;
			SDL_BlitSurface(msg_buffer[i], NULL, screen, &dest);
		}
		else return; // no more new messages
	}
}


/**
 * Add a new message to the log
 */
void MenuHUDLog::add(const string& s) {

	if (log_count == MAX_HUD_MESSAGES) {
		remove(0); // remove the oldest message
	}

	// add new message
	log_msg[log_count] = s;
	msg_age[log_count] = calcDuration(s);

	// force HUD messages to vanish in order
	if (log_count > 0) {
		if (msg_age[log_count] < msg_age[log_count-1])
			msg_age[log_count] = msg_age[log_count-1];
	}

	// render the log entry and store it in a buffer
	Point size = font->calc_size(s, list_area.x);
	msg_buffer[log_count] = createSurface(size.x, size.y);
	font->renderShadowed(s, 0, 0, JUSTIFY_LEFT, msg_buffer[log_count], list_area.x, FONT_WHITE);

	log_count++;
}

/**
 * Remove the given message from the list
 */
void MenuHUDLog::remove(int msg_index) {

	SDL_FreeSurface(msg_buffer[msg_index]);
	msg_buffer[msg_index] = NULL;

	for (int i=msg_index; i<MAX_HUD_MESSAGES-1; i++) {
		log_msg[i] = log_msg[i+1];
		msg_age[i] = msg_age[i+1];
		msg_buffer[i] = msg_buffer[i+1];
	}

	log_count--;
}

void MenuHUDLog::clear() {
	log_count = 0;
	for (int i=0; i<MAX_HUD_MESSAGES; i++) {
		SDL_FreeSurface(msg_buffer[i]);
		msg_buffer[i] = NULL;
	}

}

MenuHUDLog::~MenuHUDLog() {
	for (int i=0; i<MAX_HUD_MESSAGES; i++) {
		SDL_FreeSurface(msg_buffer[i]);
	}
}
