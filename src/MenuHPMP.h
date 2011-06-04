/**
 * MenuHPMP
 *
 * Handles the display of the HP and MP bars at the top/left of the screen
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_HP_MP_H
#define MENU_HP_MP_H

#include "SDL.h"
#include "SDL_image.h"
#include "StatBlock.h"
#include "Utils.h"
#include "FontEngine.h"
#include <string>
#include <sstream>

using namespace std;

class MenuHPMP {
private:
	SDL_Surface *screen;
	FontEngine *font;
	SDL_Surface *background;
	SDL_Surface *bar_hp;
	SDL_Surface *bar_mp;
public:
	MenuHPMP(SDL_Surface *_screen, FontEngine *_font);
	~MenuHPMP();
	void loadGraphics();
	void render(StatBlock *stats, Point mouse);
};

#endif
