/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2013 Henrik Andersson

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

#include "FileParser.h"
#include "ItemStorage.h"
#include "Menu.h"
#include "MenuVendor.h"
#include "NPC.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetTabControl.h"

using namespace std;


MenuVendor::MenuVendor(ItemManager *_items, StatBlock *_stats)
	: Menu()
	, items(_items)
	, stats(_stats)
	, closeButton(new WidgetButton(mods->locate("images/menus/buttons/button_x.png")))
	, tabControl(new WidgetTabControl(2))
	, activetab(VENDOR_BUY)
	, color_normal(font->getColor("menu_normal"))
	, npc(NULL)
	, buyback_stock()
	, talker_visible(false)
{
	background = loadGraphicSurface("images/menus/vendor.png");

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
			} else if (infile.key == "caption"){
				title =  eatLabelInfo(infile.val);
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/vendor.txt!\n");

	VENDOR_SLOTS = slots_cols * slots_rows;
}

void MenuVendor::update() {
	slots_area.x += window_area.x;
	slots_area.y += window_area.y;
	slots_area.w = slots_cols*ICON_SIZE;
	slots_area.h = slots_rows*ICON_SIZE;

	SDL_Rect tabs_area = slots_area;

	int tabheight = tabControl->getTabHeight();
	tabControl->setMainArea(tabs_area.x, tabs_area.y-tabheight, tabs_area.w, tabs_area.h+tabheight);
	tabControl->updateHeader();

	stock[VENDOR_BUY].init( VENDOR_SLOTS, items, slots_area, ICON_SIZE, slots_cols);
	stock[VENDOR_SELL].init( VENDOR_SLOTS, items, slots_area, ICON_SIZE, slots_cols);

	closeButton->pos.x = window_area.x+close_pos.x;
	closeButton->pos.y = window_area.y+close_pos.y;
}

void MenuVendor::loadMerchant(const std::string&) {
}

void MenuVendor::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
		snd->play(sfx_close);
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
	if (!title.hidden) {
		WidgetLabel label;
		label.set(window_area.x+title.x, window_area.y+title.y, title.justify, title.valign, msg->get("Vendor") + " - " + npc->name, color_normal, title.font_style);
		label.render();
	}

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
	items->playSound(stack.item);
	stock[activetab].itemReturn(stack);
	saveInventory();
}

void MenuVendor::add(ItemStack stack) {
	// Remove the first item stack to make room
	if (stock[VENDOR_SELL].full(stack.item)) {
		stock[VENDOR_SELL][0].item = 0;
		stock[VENDOR_SELL][0].quantity = 0;
		sort(VENDOR_SELL);
	}
	items->playSound(stack.item);
	stock[VENDOR_SELL].add(stack);
	saveInventory();
}

TooltipData MenuVendor::checkTooltip(Point mouse) {
	int vendor_view = (activetab == VENDOR_BUY) ? VENDOR_BUY : VENDOR_SELL;
	return stock[activetab].checkTooltip( mouse, stats, vendor_view);
}

/**
 * Several NPCs vendors can share this menu.
 * When the player talks to a new NPC, apply that NPC's inventory
 */
void MenuVendor::setInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		stock[VENDOR_BUY][i] = npc->stock[i];
		stock[VENDOR_SELL][i] = buyback_stock[i];
	}
	sort(VENDOR_BUY);
	sort(VENDOR_SELL);
}

/**
 * Save changes to the inventory back to the NPC
 * For persistent stock amounts and buyback (at least until
 * the player leaves this map)
 */
void MenuVendor::saveInventory() {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		if (npc) npc->stock[i] = stock[VENDOR_BUY][i];
		buyback_stock[i] = stock[VENDOR_SELL][i];
	}

}

void MenuVendor::sort(int type) {
	for (int i=0; i<VENDOR_SLOTS; i++) {
		if (stock[type][i].item == 0) {
			for (int j=i; j<VENDOR_SLOTS; j++) {
				if (stock[type][j].item != 0) {
					stock[type][i] = stock[type][j];
					stock[type][j].item = 0;
					stock[type][j].quantity = 0;
					break;
				}
			}
		}
	}
}

MenuVendor::~MenuVendor() {
	SDL_FreeSurface(background);
	delete closeButton;
	delete tabControl;
}

