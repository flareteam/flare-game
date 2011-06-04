/**
 * MenuExperience
 *
 * Handles the display of the Experience bar on the HUD
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_EXPERIENCE_H
#define MENU_EXPERIENCE_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "StatBlock.h"
#include "Utils.h"
#include "FontEngine.h"
#include <string>
#include <sstream>

using namespace std;

class MenuExperience {
private:
	SDL_Surface *screen;
	FontEngine *font;
	SDL_Surface *background;
	SDL_Surface *bar;
public:
	MenuExperience(SDL_Surface *_screen, FontEngine *_font);
	~MenuExperience();
	void loadGraphics();
	void render(StatBlock *stats, Point mouse);
	
	// variables to make the experience bar display configurable
	
	// hud_position is the bounding box for the entire menu.
	// This is useful for checking mouseover.
	// Also, changing hud_position (x,y) should move the frame contents properly
	SDL_Rect hud_position;
	
	Point background_offset; // offset from hud_position (x,y)
	Point background_size;
	Point bar_offset; // offset from hud_position (x,y)
	Point bar_size;
	Point text_offset;
	int text_justify;
	string text_label;
	
};

#endif
