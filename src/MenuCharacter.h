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
#include "WidgetLabel.h"
#include <string>
#include <sstream>

const int CSTAT_NAME = 0;
const int CSTAT_LEVEL = 1;
const int CSTAT_PHYSICAL = 2;
const int CSTAT_HP = 3;
const int CSTAT_HPREGEN = 4;
const int CSTAT_MENTAL = 5;
const int CSTAT_MP = 6;
const int CSTAT_MPREGEN = 7;
const int CSTAT_OFFENSE = 8;
const int CSTAT_ACCURACYV1 = 9;
const int CSTAT_ACCURACYV5 = 10;
const int CSTAT_DEFENSE = 11;
const int CSTAT_AVOIDANCEV1 = 12;
const int CSTAT_AVOIDANCEV5 = 13;
const int CSTAT_DMGMAIN = 14;
const int CSTAT_DMGRANGED = 15;
const int CSTAT_CRIT = 16;
const int CSTAT_ABSORB = 17;
const int CSTAT_FIRERESIST = 18;
const int CSTAT_ICERESIST = 19;
const int CSTAT_COUNT = 20;

struct CharStat {
	WidgetLabel *label;
	WidgetLabel *value;
	SDL_Rect hover;
	TooltipData tip;
	
	void setHover(int x, int y, int w, int h) {
		hover.x=x;
		hover.y=y;
		hover.w=w;
		hover.h=h;
	}
};

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
	WidgetLabel *labelCharacter;
	CharStat cstat[CSTAT_COUNT];
	

	void displayProficiencies(int value, int y);
	void loadGraphics();
	int bonusColor(int stat);
	
public:
	MenuCharacter(SDL_Surface *screen, InputState *inp, FontEngine *font, StatBlock *stats, MessageEngine *_msg);
	~MenuCharacter();
	void logic();
	void render();
	void refreshStats();
	TooltipData checkTooltip();
	bool checkUpgrade();

	bool visible;

};

#endif
