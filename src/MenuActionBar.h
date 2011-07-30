/**
 * class ActionBar
 *
 * Handles the config, display, and usage of the 0-9 hotkeys, mouse buttons, and menu calls
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#ifndef MENU_ACTION_BAR_H
#define MENU_ACTION_BAR_H

#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "InputState.h"
#include "MenuTooltip.h"
#include "PowerManager.h"
#include "FontEngine.h"
#include "StatBlock.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>

const int MENU_CHARACTER = 0;
const int MENU_INVENTORY = 1;
const int MENU_POWERS = 2;
const int MENU_LOG = 3;

class MenuActionBar {
private:
	SDL_Surface *screen;
	SDL_Surface *background;
	SDL_Surface *emptyslot;
	SDL_Surface *icons;
	SDL_Surface *disabled;
	
	StatBlock *hero;
	PowerManager *powers;
	InputState *inp;
	FontEngine *font;
	MessageEngine *msg;
	SDL_Rect src;
	SDL_Rect label_src;
	
	// for now the key mappings are static.  Just use an image for the labels
	SDL_Surface *labels;
	
public:

	MenuActionBar(SDL_Surface *_screen, FontEngine *_font, InputState *_inp, PowerManager *_powers, StatBlock *hero, SDL_Surface *icons, MessageEngine *_msg);
	~MenuActionBar();
	void loadGraphics();
	void renderIcon(int icon_id, int x, int y);
	void logic();
	void render();
	void renderItemCounts();
	int checkAction(Point mouse);
	int checkDrag(Point mouse);
	void checkMenu(Point mouse, bool &menu_c, bool &menu_i, bool &menu_p, bool &menu_l);
	void drop(Point mouse, int power_index, bool rearranging);
	void remove(Point mouse);
	void set(int power_id[12]);
	void clear();

	TooltipData checkTooltip(Point mouse);
	
	int hotkeys[12]; // refer to power_index in PowerManager
	SDL_Rect slots[12]; // the location of hotkey slots
	SDL_Rect menus[4]; // the location of the menu buttons
	int slot_item_count[12]; // -1 means this power isn't item based.  0 means out of items.  1+ means sufficient items.
	bool slot_enabled[12];
	
	// these store the area occupied by these hotslot sections.
	// useful for detecting mouse interactions on those locations
	SDL_Rect numberArea;
	SDL_Rect mouseArea;
	SDL_Rect menuArea;
	int drag_prev_slot;
	
};

#endif
