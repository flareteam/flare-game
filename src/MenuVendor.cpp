/*
Copyright © 2011-2012 Clint Bellanger

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class MenuVendor
 */

#include "CampaignManager.h"
#include "FileParser.h"
#include "Menu.h"
#include "MenuVendor.h"
#include "NPC.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetTabControl.h"

using namespace std;


MenuVendor::MenuVendor(ItemManager *_items, StatBlock *_stats, CampaignManager *_camp) {
	items = _items;
	stats = _stats;
	camp = _camp;

	visible = false;
	talker_visible = false;
	activetab = VENDOR_BUY;
	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	tabControl = new WidgetTabControl(2);
	tabControl->setTabTitle(VENDOR_BUY,msg->get("Inventory"));
	tabControl->setTabTitle(VENDOR_SELL,msg->get("Buyback"));

	loadMerchant("");


	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/vendor.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "slots_area") {
				slots_area.x = eatFirstInt(infile.val,',');
				slots_area.y = eatFirstInt(infile.val,',');
			} else if (infile.key == "vendor_cols"){
				slots_cols = eatFirstInt(infile.val,',');
			} else if (infile.key == "vendor_rows"){
				slots_rows = eatFirstInt(infile.val,',');
			} else if (infile.key == "title"){
				title_pos.x =  eatFirstInt(infile.val,',');
				title_pos.y =  eatFirstInt(infile.val,',');
			} else if (infile.key == "name"){
				name_pos.x =  eatFirstInt(infile.val,',');
				name_pos.y =  eatFirstInt(infile.val,',');
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open vendor.txt!\n");

	VENDOR_SLOTS = slots_cols * slots_rows;
}

void MenuVendor::loadGraphics() {
	background = IMG_Load(mods->locate("images/menus/vendor.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);
}

void MenuVendor::update() {
	slots_area.x += window_area.x;
	slots_area.y += window_area.y;
	slots_area.w = slots_cols*32;
	slots_area.h = slots_rows*32;

	SDL_Rect tabs_area = slots_area;

	//TODO: Put tabcontrol posistion in vendor.txt
	tabControl->setMainArea(tabs_area.x, tabs_area.y-18, tabs_area.w, tabs_area.h+18);
	tabControl->updateHeader();

	stock[VENDOR_BUY].init( VENDOR_SLOTS, items, slots_area, ICON_SIZE_32, slots_cols);
	stock[VENDOR_SELL].init( VENDOR_SLOTS, items, slots_area, ICON_SIZE_32, slots_cols);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;
}

void MenuVendor::loadMerchant(const std::string&) {
}

void MenuVendor::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}
}

void MenuVendor::tabsLogic() {
	tabControl->logic();
	activetab = tabControl->getActiveTab();
}

void MenuVendor::setTab(int tab) {
	tabControl->setActiveTab(tab);
	activetab = tab;
}

void MenuVendor::render() {
	if (!visible) return;
	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = window_area.w;
	src.h = dest.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &dest);

	// close button
	closeButton->render();

	// text overlay
	WidgetLabel label;
	label.set(window_area.x+title_pos.x, window_area.y+title_pos.y, JUSTIFY_CENTER, VALIGN_TOP, msg->get("Vendor"), FONT_WHITE);
	label.render();
	label.set(window_area.x+name_pos.x, window_area.y+name_pos.y, JUSTIFY_CENTER, VALIGN_TOP, npc->name, FONT_WHITE);
	label.render();

	// render tabs
	tabControl->render();

	// show stock
	stock[activetab].render();
}

/**
 * Start dragging a vendor item
 * Players can drag an item to their inventory to purchase.
 */
ItemStack MenuVendor::click(InputState * input) {
	ItemStack stack = stock[activetab].click(input);
	saveInventory();
	return stack;
}

/**
 * Cancel the dragging initiated by the clic()
 */
void MenuVendor::itemReturn(ItemStack stack) {
	stock[activetab].itemReturn(stack);
	saveInventory();
}

void MenuVendor::add(ItemStack stack) {
	stock[activetab].add(stack);
	saveInventory();
}

TooltipData MenuVendor::checkTooltip(Point mouse) {
	return stock[activetab].checkTooltip( mouse, stats, true);
}

bool MenuVendor::full() {
	return stock[activetab].full();
}

/**
 * Several NPCs vendors can share this menu.
 * When the player talks to a new NPC, apply that NPC's inventory
 */
void MenuVendor::setInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		stock[VENDOR_BUY][i] = npc->stock[i];
		stock[VENDOR_SELL][i] = camp->buyback_stock[i];
	}
}

/**
 * Save changes to the inventory back to the NPC
 * For persistent stock amounts and buyback (at least until
 * the player leaves this map)
 */
void MenuVendor::saveInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		npc->stock[i] = stock[VENDOR_BUY][i];
		camp->buyback_stock[i] = stock[VENDOR_SELL][i];
	}

}

MenuVendor::~MenuVendor() {
	SDL_FreeSurface(background);
	delete closeButton;
	delete tabControl;
}

