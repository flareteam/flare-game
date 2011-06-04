/**
 * class MenuHUDLog
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_HUD_LOG_H
#define MENU_HUD_LOG_H

#include "SDL.h"
#include "SDL_image.h"
#include "Settings.h"
#include "Utils.h"
#include "FontEngine.h"

const int MAX_HUD_MESSAGES = 32;

class MenuHUDLog{
private:

	int calcDuration(string s);

	SDL_Surface *screen;
	FontEngine *font;
	string log_msg[MAX_HUD_MESSAGES];
	int msg_age[MAX_HUD_MESSAGES];
	int log_count;
	int paragraph_spacing;
	
public:

	MenuHUDLog(SDL_Surface *screen, FontEngine *font);
	~MenuHUDLog();
	void logic();
	void render();
	void add(string s);
	void clear();
	
	Point list_area;

};

#endif
