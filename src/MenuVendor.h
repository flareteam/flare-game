/**
 * class MenuVendor
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_VENDOR_H
#define MENU_VENDOR_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "Utils.h"
#include "FontEngine.h"
#include "MenuItemStorage.h"
#include "MenuTooltip.h"
#include "StatBlock.h"
#include "WidgetButton.h"
#include "NPC.h"
#include "MessageEngine.h"
#include <string>
#include <sstream>

const int VENDOR_SLOTS = 80;

class MenuVendor {
private:
	SDL_Surface *screen;
	ItemDatabase *items;
	FontEngine *font;
	StatBlock *stats;
	InputState *inp;
	MessageEngine *msg;
	WidgetButton *closeButton;

	void loadGraphics();
	SDL_Surface *background;
	MenuItemStorage stock; // items the vendor currently has in stock

public:
	MenuVendor(SDL_Surface *screen, InputState *_inp, FontEngine *font, ItemDatabase *items, StatBlock *stats, MessageEngine *msg);
	~MenuVendor();

	NPC *npc;

	void loadMerchant(string filename);
	void logic();
	void render();
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);
	void add(ItemStack stack);
	TooltipData checkTooltip(Point mouse);
	bool full();
	void setInventory();
	void saveInventory();

	bool visible;
	SDL_Rect slots_area;
};


#endif
