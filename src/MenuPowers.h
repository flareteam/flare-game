/**
 * class MenuPowers
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_POWERS_H
#define MENU_POWERS_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "MenuTooltip.h"
#include "PowerManager.h"
#include <string>
#include <sstream>

using namespace std;

class MenuPowers {
private:
	SDL_Surface *screen;
	FontEngine *font;
	StatBlock *stats;
	PowerManager *powers;
	
	SDL_Surface *background;
	SDL_Surface *powers_step;
	SDL_Surface *powers_unlock;
	void loadGraphics();
	void displayBuild(int value, int x);

public:
	MenuPowers(SDL_Surface *_screen, FontEngine *_font, StatBlock *_stats, PowerManager *_powers);
	~MenuPowers();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool requirementsMet(int power_index);
	int click(Point mouse);
	
	bool visible;
	SDL_Rect slots[20]; // the location of power slots

};

#endif
