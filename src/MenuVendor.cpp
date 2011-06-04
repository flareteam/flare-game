/**
 * class MenuVendor
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuVendor.h"

MenuVendor::MenuVendor(SDL_Surface *_screen, FontEngine *_font, ItemDatabase *_items, StatBlock *_stats) {
	screen = _screen;
	font = _font;
	items = _items;
	stats = _stats;
	
	int offset_y = (VIEW_H - 416)/2;
	
	slots_area.x = 32;
	slots_area.y = offset_y + 64;
	slots_area.w = 256;
	slots_area.h = 320;
	
	stock.init( VENDOR_SLOTS, items, screen, font, slots_area, ICON_SIZE_32, 8);

	visible = false;
	loadGraphics();
	loadMerchant("");
}

void MenuVendor::loadGraphics() {
	background = IMG_Load("images/menus/vendor.png");
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
}

void MenuVendor::loadMerchant(string filename) {
}

void MenuVendor::logic() {
}

void MenuVendor::render() {
	if (!visible) return;
	SDL_Rect src;
	SDL_Rect dest;
	
	int offset_y = (VIEW_H - 416)/2;
	
	// background
	src.x = 0;
	src.y = 0;
	dest.x = 0;
	dest.y = offset_y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);
		
	// text overlay
	// TODO: translate()
	font->render("Vendor", 160, offset_y+8, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(npc->name, 160, offset_y+24, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	// show stock
	stock.render();
}

/**
 * Start dragging a vendor item
 * Players can drag an item to their inventory to purchase.
 */
ItemStack MenuVendor::click(InputState * input) {
	ItemStack stack = stock.click(input);
	saveInventory();
	return stack;
}

/**
 * Cancel the dragging initiated by the clic()
 */
void MenuVendor::itemReturn(ItemStack stack) {
	stock.itemReturn(stack);
	saveInventory();
}

void MenuVendor::add(ItemStack stack) {
	stock.add(stack);
	saveInventory();
}

TooltipData MenuVendor::checkTooltip(Point mouse) {
	return stock.checkTooltip( mouse, stats, true);
}

bool MenuVendor::full() {
	return stock.full();
}

/**
 * Several NPCs vendors can share this menu.
 * When the player talks to a new NPC, apply that NPC's inventory
 */
void MenuVendor::setInventory() {	
	for (int i=0; i<VENDOR_SLOTS; i++) {
		stock[i] = npc->stock[i];
	}
}

/**
 * Save changes to the inventory back to the NPC
 * For persistent stock amounts and buyback (at least until
 * the player leaves this map)
 */
void MenuVendor::saveInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		npc->stock[i] = stock[i];
	}

}

MenuVendor::~MenuVendor() {
	SDL_FreeSurface(background);
}

