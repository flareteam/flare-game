/**
 * class MenuCharacter
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_CHARACTER_H
#define MENU_CHARACTER_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "MenuTooltip.h"
#include <string>
#include <sstream>

class MenuCharacter {
private:
	SDL_Surface *screen;
	FontEngine *font;
	StatBlock *stats;

	
	SDL_Surface *background;
	SDL_Surface *proficiency;
	SDL_Surface *upgrade;

	void displayProficiencies(int value, int y);
	void loadGraphics();
	
public:
	MenuCharacter(SDL_Surface *screen, FontEngine *font, StatBlock *stats);
	~MenuCharacter();
	void logic();
	void render();
	TooltipData checkTooltip(Point mouse);
	bool checkUpgrade(Point mouse);

	bool visible;

};

#endif
