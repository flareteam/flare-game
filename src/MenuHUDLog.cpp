/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller

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

#include "Menu.h"
#include "MenuHUDLog.h"
#include "SharedResources.h"
#include "Settings.h"

using namespace std;

MenuHUDLog::MenuHUDLog() {

	list_area.x = 224;
	list_area.y = 416;
	font->setFont("font_regular");
	paragraph_spacing = font->getLineHeight()/2;

	color_normal = font->getColor("menu_normal");
}

/**
 * Calculate how long a given message should remain on the HUD
 * Formula: minimum time plus x frames per character
 */
int MenuHUDLog::calcDuration(const string& s) {
	// 5 seconds plus an extra second per 10 letters
	return MAX_FRAMES_PER_SEC * 5 + s.length() * (MAX_FRAMES_PER_SEC/10);
}

/**
 * Perform one frame of logic
 * Age messages
 */
void MenuHUDLog::logic() {
	for (unsigned i=0; i<msg_age.size(); i++) {
		if (msg_age[i] > 0)
			msg_age[i]--;
		else
			remove(i);
	}
}


/**
 * New messages appear on the screen for a brief time
 */
void MenuHUDLog::render() {

	SDL_Rect dest;
	dest.x = window_area.x;
	dest.y = window_area.y;


	// go through new messages
	for (int i=msg_age.size()-1; i>=0; i--) {
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

	// add new message
	log_msg.push_back(s);
	msg_age.push_back(calcDuration(s));

	// force HUD messages to vanish in order
	if (msg_age.size() > 1) {
		const int last = msg_age.size();
		if (msg_age[last] < msg_age[last-1])
			msg_age[last] = msg_age[last-1];
	}

	// render the log entry and store it in a buffer
	font->setFont("font_regular");
	Point size = font->calc_size(s, window_area.w);
	msg_buffer.push_back(createAlphaSurface(size.x, size.y));
	font->renderShadowed(s, 0, 0, JUSTIFY_LEFT, msg_buffer.back(), window_area.w, color_normal);
}

/**
 * Remove the given message from the list
 */
void MenuHUDLog::remove(int msg_index) {
	SDL_FreeSurface(msg_buffer.at(msg_index));
	msg_buffer.erase(msg_buffer.begin()+msg_index);
	msg_age.erase(msg_age.begin()+msg_index);
	log_msg.erase(log_msg.begin()+msg_index);
}

void MenuHUDLog::clear() {
	for (unsigned i=0; i<msg_buffer.size(); i++) {
		SDL_FreeSurface(msg_buffer[i]);
	}
	msg_buffer.clear();
	msg_age.clear();
	log_msg.clear();
}

MenuHUDLog::~MenuHUDLog() {
	for (unsigned i=0; i<msg_buffer.size(); i++) {
		SDL_FreeSurface(msg_buffer[i]);
	}
}
