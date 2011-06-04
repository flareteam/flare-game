/**
 * class MenuHUDLog
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuHUDLog.h"

MenuHUDLog::MenuHUDLog(SDL_Surface *_screen, FontEngine *_font) {
	screen = _screen;
	font = _font;
	
	log_count = 0;
	list_area.x = 224;
	list_area.y = 416;
	paragraph_spacing = 6;
}

/**
 * Calculate how long a given message should remain on the HUD
 * Formula: minimum time plus x frames per character
 */
int MenuHUDLog::calcDuration(string s) {
	// 5 seconds plus an extra second per 10 letters
	return FRAMES_PER_SEC * 5 + s.length() * (FRAMES_PER_SEC/10);
}

/**
 * Perform one frame of logic
 * Age messages
 */
void MenuHUDLog::logic() {
	for (int i=0; i<log_count; i++)
		if (msg_age[i] > 0) msg_age[i]--;
}


/**
 * New messages appear on the screen for a brief time
 */
void MenuHUDLog::render() {
	Point size;
	int cursor_y;
	
	cursor_y = VIEW_H - 40;
	
	// go through new messages
	for (int i=log_count-1; i>=0; i--) {
		if (msg_age[i] > 0 && cursor_y > 32) {
		
			size = font->calc_size(log_msg[i], list_area.x);
			cursor_y -= size.y + paragraph_spacing;
	
			font->render(log_msg[i], 32, cursor_y, JUSTIFY_LEFT, screen, list_area.x, FONT_WHITE);
			
		}
		else return; // no more new messages
	}
}


/**
 * Add a new message to the log
 */
void MenuHUDLog::add(string s) {

	if (log_count == MAX_HUD_MESSAGES) {

		// remove oldest message
		for (int i=0; i<MAX_HUD_MESSAGES-1; i++) {
			log_msg[i] = log_msg[i+1];
			msg_age[i] = msg_age[i+1];
		}

		log_count--;
	}
	
	// add new message
	log_msg[log_count] = s;
	msg_age[log_count] = calcDuration(s);
	
	// force HUD messages to vanish in order
	if (log_count > 0) {
		if (msg_age[log_count] < msg_age[log_count-1])
			msg_age[log_count] = msg_age[log_count-1];
	}
	
	log_count++;
}

void MenuHUDLog::clear() {
	log_count = 0;
}

MenuHUDLog::~MenuHUDLog() {

}
