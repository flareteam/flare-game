/**
 * class MenuItemStorage
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_ITEM_STORAGE_H
#define MENU_ITEM_STORAGE_H

#include "SDL.h"
#include "InputState.h"
#include "ItemStorage.h"

using namespace std;

class MenuItemStorage : public ItemStorage {
protected:
	SDL_Surface *screen;
	FontEngine *font;
	SDL_Rect area;
	int icon_size;
	int nb_cols;

public:
	void init(int _slot_number, ItemDatabase *_items, SDL_Surface *_screen, FontEngine *_font, SDL_Rect _area, int icon_size, int nb_cols);

	// rendering
	void render();
	int slotOver(Point mouse);
	TooltipData checkTooltip(Point mouse, StatBlock *stats, bool vendor_view);
	ItemStack click(InputState * input);
	void itemReturn(ItemStack stack);

	int drag_prev_slot;
};

#endif


