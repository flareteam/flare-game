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
#include "InputState.h"
#include "WidgetButton.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>

class MenuCharacter {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;
	StatBlock *stats;
	MessageEngine *msg;

	SDL_Surface *background;
	SDL_Surface *proficiency;
	SDL_Surface *upgrade;
	WidgetButton *closeButton;

	void displayProficiencies(int value, int y);
	void loadGraphics();
	int bonusColor(int stat);
	
public:
	MenuCharacter(SDL_Surface *screen, InputState *inp, FontEngine *font, StatBlock *stats, MessageEngine *_msg);
	~MenuCharacter();
	void logic();
	void render();
	TooltipData checkTooltip();
	bool checkUpgrade();

	bool visible;

};

#endif
