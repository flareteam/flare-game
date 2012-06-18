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
 * class MenuManager
 */

#include "MenuManager.h"
#include "MenuActionBar.h"
#include "MenuCharacter.h"
#include "MenuExperience.h"
#include "MenuHPMP.h"
#include "MenuHUDLog.h"
#include "MenuInventory.h"
#include "MenuMiniMap.h"
#include "MenuPowers.h"
#include "MenuEnemy.h"
#include "MenuVendor.h"
#include "MenuTalker.h"
#include "MenuExit.h"
#include "MenuLog.h"
#include "ModManager.h"
#include "PowerManager.h"
#include "SharedResources.h"

MenuManager::MenuManager(PowerManager *_powers, StatBlock *_stats, CampaignManager *_camp, ItemManager *_items) {
	powers = _powers;
	stats = _stats;
	powers = _powers;
	camp = _camp;
	items = _items;

	loadIcons();

	chr = new MenuCharacter(stats);
	inv = new MenuInventory(items, stats, powers);
	pow = new MenuPowers(stats, powers);
	log = new MenuLog();
	hudlog = new MenuHUDLog();
	act = new MenuActionBar(powers, stats, icons);
	hpmp = new MenuHPMP();
	tip = new WidgetTooltip();
	mini = new MenuMiniMap();
	xp = new MenuExperience();
	enemy = new MenuEnemy();
	vendor = new MenuVendor(items, stats);
	talker = new MenuTalker(camp);
	exit = new MenuExit();

	pause = false;
	dragging = false;
	drag_stack.item = 0;
	drag_stack.quantity = 0;
	drag_power = -1;
	drag_src = 0;
	drop_stack.item = 0;
	drop_stack.quantity = 0;

	loadSounds();

	done = false;
}

/**
 * Icon set shared by all menus
 */
void MenuManager::loadIcons() {

	icons = IMG_Load(mods->locate("images/icons/icons32.png").c_str());
	if(!icons) {
		fprintf(stderr, "Couldn't load icons: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = icons;
	icons = SDL_DisplayFormatAlpha(icons);
	SDL_FreeSurface(cleanup);
}

void MenuManager::loadSounds() {
    if (audio == true) {
        sfx_open = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_page.ogg").c_str());
        sfx_close = Mix_LoadWAV(mods->locate("soundfx/inventory/inventory_book.ogg").c_str());

        if (!sfx_open || !sfx_close)
            fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
    } else {
        sfx_open = NULL;
        sfx_close = NULL;
    }
}


void MenuManager::renderIcon(int icon_id, int x, int y) {
	SDL_Rect src;
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	src.w = src.h = dest.w = dest.h = 32;
	src.x = (icon_id % 16) * 32;
	src.y = (icon_id / 16) * 32;
	SDL_BlitSurface(icons, &src, screen, &dest);
}

void MenuManager::logic() {

	bool clicking_character = false;
	bool clicking_inventory = false;
	bool clicking_powers = false;
	bool clicking_log = false;
	ItemStack stack;

	hudlog->logic();
	enemy->logic();
	chr->logic();
	inv->logic();
	vendor->logic();
	pow->logic();
	log->logic();
	talker->logic();

	// only allow the vendor window to be open if the inventory is open
	if (vendor->visible && !(inv->visible)) {
	  closeLeft(true);
	}

	if (!inpt->pressing[INVENTORY] && !inpt->pressing[POWERS] && !inpt->pressing[CHARACTER] && !inpt->pressing[LOG])
		key_lock = false;

	// check if mouse-clicking a menu button
	act->checkMenu(inpt->mouse, clicking_character, clicking_inventory, clicking_powers, clicking_log);

	if (exit->visible) {
		exit->logic();
		if (exit->isExitRequested()) {
			done = true;
		}
	}

	// exit menu toggle
	if ((inpt->pressing[CANCEL] && !inpt->lock[CANCEL] && !key_lock && !dragging) && !(stats->corpse && stats->permadeath)) {
		inpt->lock[CANCEL] = true;
		key_lock = true;
		if (menus_open) {
			closeAll(true);
		}
		else {
			exit->visible = !exit->visible;
		}
	}

	// inventory menu toggle
	if (((inpt->pressing[INVENTORY] && !key_lock && !dragging) || clicking_inventory) && !stats->transformed) {
		key_lock = true;
		if (inv->visible) {
			closeRight(true);
		}
		else {
			closeRight(false);
            act->requires_attention[MENU_INVENTORY] = false;
			inv->visible = true;
            if (sfx_open)
                Mix_PlayChannel(-1, sfx_open, 0);
		}

	}

	// powers menu toggle
	if (((inpt->pressing[POWERS] && !key_lock && !dragging) || clicking_powers) && !stats->transformed) {
		key_lock = true;
		if (pow->visible) {
			closeRight(true);
		}
		else {
			closeRight(false);
            act->requires_attention[MENU_POWERS] = false;
			pow->visible = true;
            if (sfx_open)
                Mix_PlayChannel(-1, sfx_open, 0);
		}
	}

	// character menu toggleggle
	if (((inpt->pressing[CHARACTER] && !key_lock && !dragging) || clicking_character) && !stats->transformed) {
		key_lock = true;
		if (chr->visible) {
			closeLeft(true);
		}
		else {
			closeLeft(false);
            act->requires_attention[MENU_CHARACTER] = false;
			chr->visible = true;
            if (sfx_open)
                Mix_PlayChannel(-1, sfx_open, 0);
		}
	}

	// log menu toggle
	if (((inpt->pressing[LOG] && !key_lock && !dragging) || clicking_log) && !stats->transformed) {
		key_lock = true;
		if (log->visible) {
			closeLeft(true);
		}
		else {
			closeLeft(false);
            act->requires_attention[MENU_LOG] = false;
			log->visible = true;
            if (sfx_open)
                Mix_PlayChannel(-1, sfx_open, 0);
		}
	}

	if (MENUS_PAUSE) {
		pause = (inv->visible || pow->visible || chr->visible || log->visible || vendor->visible || talker->visible);
	}
	menus_open = (inv->visible || pow->visible || chr->visible || log->visible || vendor->visible || talker->visible);

	if (stats->alive) {
		int offset_x = (VIEW_W - 320);
		int offset_y = (VIEW_H - 416)/2;

		// handle right-click
		if (!dragging && inpt->pressing[MAIN2] && !inpt->lock[MAIN2]) {
			// exit menu
			if (exit->visible && isWithin(exit->window_area, inpt->mouse)) {
				inpt->lock[MAIN2] = true;
			}

			// activate inventory item
			else if (inv->visible && isWithin(inv->window_area, inpt->mouse)) {
				inpt->lock[MAIN2] = true;
				if (isWithin(inv->carried_area, inpt->mouse)) {
					inv->activate(inpt);
				}
			}
		}

		// handle left-click
		if (!dragging && inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) {
			// exit menu
			if (exit->visible && isWithin(exit->window_area, inpt->mouse)) {
				inpt->lock[MAIN1] = true;
			}

			// left side menu
			else if (inpt->mouse.x <= 320 && inpt->mouse.y >= offset_y && inpt->mouse.y <= offset_y+416) {
				if (chr->visible) {
					inpt->lock[MAIN1] = true;

					// applied a level-up
					if (chr->checkUpgrade()) {

						// apply equipment and max hp/mp
						inv->applyEquipment(inv->inventory[EQUIPMENT].storage);
						stats->hp = stats->maxhp;
						stats->mp = stats->maxmp;
					}
				}
				else if (vendor->visible) {

					inpt->lock[MAIN1] = true;
					if (inpt->pressing[CTRL]) {

						// buy item from a vendor
						if (!inv->full()) {
							stack = vendor->click(inpt);
							if (stack.item > 0) {
								if( inv->full()) {
									// Can we say "Not enough place" ?
									vendor->itemReturn( stack);
								}
								else if( ! inv->buy( stack)) {
									// Can we say "Not enough money" ? (here or in MenuInventory::buy())
									vendor->itemReturn( stack);
								}
							}
						}
					}
					else {

						// start dragging a vendor item
						drag_stack = vendor->click(inpt);
						if (drag_stack.item > 0) {
							dragging = true;
							drag_src = DRAG_SRC_VENDOR;
						}
					}
				} else if(log->visible) {
          inpt->lock[MAIN1] = true;
          log->tabsLogic();
        }
			}

			// right side menu
			else if (inpt->mouse.x >= offset_x && inpt->mouse.y >= offset_y && inpt->mouse.y <= offset_y+416) {

				// pick up an inventory item
				if (inv->visible) {

					if (inpt->pressing[CTRL]) {
						inpt->lock[MAIN1] = true;
						stack = inv->click(inpt);
						if( stack.item > 0) {
							if (vendor->visible) {
								if( vendor->full()) {
									// Can we say "Not enough place" ?
									inv->itemReturn( stack);
								}
								else {
									// The vendor could have a limited amount of money in the future. It will be tested here.
									if (inv->sell(stack)) {
										vendor->add(stack);
									}
									else {
										inv->itemReturn(stack);
									}
								}
							}
							else {
								if (!inv->sell(stack)) {
									inv->itemReturn(stack);
								}
							}
						}
					}
					else {
						inpt->lock[MAIN1] = true;
						drag_stack = inv->click(inpt);
						if (drag_stack.item > 0) {
							dragging = true;
							drag_src = DRAG_SRC_INVENTORY;
						}
					}
				}
				// pick up a power
				else if (pow->visible) {
					inpt->lock[MAIN1] = true;
					drag_power = pow->click(inpt->mouse);
					if (drag_power > -1) {
						dragging = true;
						drag_src = DRAG_SRC_POWERS;
					}
				}
			}
			// action bar
			else if (isWithin(act->numberArea,inpt->mouse) || isWithin(act->mouseArea,inpt->mouse) || isWithin(act->menuArea, inpt->mouse)) {
				inpt->lock[MAIN1] = true;

				// ctrl-click action bar to clear that slot
				if (inpt->pressing[CTRL] && !stats->transformed) {
					act->remove(inpt->mouse);
				}
				// allow drag-to-rearrange action bar
				else if (!isWithin(act->menuArea, inpt->mouse) && !stats->transformed) {
					drag_power = act->checkDrag(inpt->mouse);
					if (drag_power > -1) {
						dragging = true;
						drag_src = DRAG_SRC_ACTIONBAR;
					}
				}

				// else, clicking action bar to use a power?
				// this check is done by GameEngine when calling Avatar::logic()


			}
		}
		// handle dropping
		if (dragging && !inpt->pressing[MAIN1]) {

			// putting a power on the Action Bar
			if (drag_src == DRAG_SRC_POWERS) {
				if (isWithin(act->numberArea,inpt->mouse) || isWithin(act->mouseArea,inpt->mouse)) {
					act->drop(inpt->mouse, drag_power, 0);
				}
			}

			// rearranging the action bar
			else if (drag_src == DRAG_SRC_ACTIONBAR) {
				if (isWithin(act->numberArea,inpt->mouse) || isWithin(act->mouseArea,inpt->mouse)) {
					act->drop(inpt->mouse, drag_power, 1);
				}
			}

			// rearranging inventory or dropping items
			else if (drag_src == DRAG_SRC_INVENTORY) {

				if (inv->visible && inpt->mouse.x >= offset_x && inpt->mouse.y >= offset_y && inpt->mouse.y <= offset_y+416) {
					inv->drop(inpt->mouse, drag_stack);
					drag_stack.item = 0;
				}
				else if (isWithin(act->numberArea,inpt->mouse) || isWithin(act->mouseArea,inpt->mouse)) {
					// The action bar is not storage!
					inv->itemReturn(drag_stack);

					// put an item with a power on the action bar
					if (items->items[drag_stack.item].power != -1) {
						act->drop(inpt->mouse, items->items[drag_stack.item].power, false);
					}
				}
				else if (vendor->visible && isWithin(vendor->slots_area, inpt->mouse)) {
					// vendor sell item
					if( vendor->full()) {
						// Can we say "Not enough place" ?
						inv->itemReturn( drag_stack);
					}
					else {
						if (inv->sell( drag_stack)) {
							vendor->add( drag_stack);
						}
						else {
							inv->itemReturn(drag_stack);
						}
					}
					drag_stack.item = 0;
				}
				else {
					// if dragging and the source was inventory, drop item to the floor

					// quest items cannot be dropped
					if (items->items[drag_stack.item].type != ITEM_TYPE_QUEST) {
						drop_stack = drag_stack;
						drag_stack.item = 0;
						drag_stack.quantity = 0;
					}
					else {
						inv->itemReturn(drag_stack);
					}
				}
			}

			else if (drag_src == DRAG_SRC_VENDOR) {

				// dropping an item from vendor (we only allow to drop into the carried area)
				if (inv->visible && isWithin( inv->carried_area, inpt->mouse)) {
					if( inv->full()) {
						// Can we say "Not enough place" ?
						vendor->itemReturn( drag_stack);
					}
					else if( ! inv->buy( drag_stack, inpt->mouse)) {
						// Can we say "Not enough money" ? (here or in MenuInventory::buy())
						vendor->itemReturn( drag_stack);
					}
					drag_stack.item = 0;
				}
				else {
					vendor->itemReturn(drag_stack);
				}
			}

			dragging = false;
		}

	}

	// handle equipment changes affecting hero stats
	if (inv->changed_equipment || inv->changed_artifact) {
		inv->applyEquipment(inv->inventory[EQUIPMENT].storage);
		inv->changed_artifact = false;
		// the equipment flag is reset after the new sprites are loaded
	}

	// for action-bar powers that represent items, lookup the current item count
	for (int i=0; i<12; i++) {
		act->slot_enabled[i] = true;
		act->slot_item_count[i] = -1;

		if (act->hotkeys[i] != -1) {
			int item_id = powers->powers[act->hotkeys[i]].requires_item;
			if (item_id != -1 && items->items[item_id].type == ITEM_TYPE_CONSUMABLE) {
				act->slot_item_count[i] = inv->getItemCountCarried(item_id);
				if (act->slot_item_count[i] == 0) {
					act->slot_enabled[i] = false;
				}
			}
			else if (item_id != -1) {

				// if a non-consumable item power is unequipped, disable that slot
				if (!inv->isItemEquipped(item_id)) {
					act->slot_enabled[i] = false;
				}
			}
		}
	}

}

void MenuManager::render() {
	hpmp->render(stats, inpt->mouse);
	xp->render(stats, inpt->mouse);
	act->render();
	inv->render();
	pow->render();
	chr->render();
	log->render();
	vendor->render();
	talker->render();
	talker->render();
	enemy->render();
	if (exit->visible) exit->render();

	TooltipData tip_new;
	int offset_x = (VIEW_W - 320);
	int offset_y = (VIEW_H - 416)/2;

	// Find tooltips depending on mouse position
	if (inpt->mouse.x < 320 && inpt->mouse.y >= offset_y && inpt->mouse.y <= offset_y+416) {
		if (chr->visible) {
			tip_new = chr->checkTooltip();
		}
		else if (vendor->visible) {
			tip_new = vendor->checkTooltip(inpt->mouse);
		}
	}
	else if (inpt->mouse.x >= offset_x && inpt->mouse.y >= offset_y && inpt->mouse.y <= offset_y+416) {
		if (pow->visible) {
			tip_new = pow->checkTooltip(inpt->mouse);
		}
		else if (inv->visible && !dragging) {
			tip_new = inv->checkTooltip(inpt->mouse);
		}
	}
	else if (inpt->mouse.y >= VIEW_H-32) {
		tip_new = act->checkTooltip(inpt->mouse);
	}

	if (tip_new.num_lines > 0) {

		// when we render a tooltip it buffers the rasterized text for performance.
		// If this new tooltip is the same as the existing one, reuse.

		// TODO: comparing the first line of a tooltip works in all existing cases,
		// but may not hold true in the future.
		if (tip_new.lines[0] != tip_buf.lines[0]) {
			tip_buf.clear();
			tip_buf = tip_new;
		}
		tip->render(tip_buf, inpt->mouse, STYLE_FLOAT);
	}

	// draw icon under cursor if dragging
	if (dragging) {
		if (drag_src == DRAG_SRC_INVENTORY || drag_src == DRAG_SRC_VENDOR)
			items->renderIcon(drag_stack, inpt->mouse.x - 16, inpt->mouse.y - 16, ICON_SIZE_32);
		else if (drag_src == DRAG_SRC_POWERS || drag_src == DRAG_SRC_ACTIONBAR)
			renderIcon(powers->powers[drag_power].icon, inpt->mouse.x-16, inpt->mouse.y-16);
	}

}

void MenuManager::closeAll(bool play_sound) {
	if (!dragging) {
		closeLeft(play_sound);
		closeRight(false);
	}
}

void MenuManager::closeLeft(bool play_sound) {
	if (!dragging) {
		chr->visible = false;
		log->visible = false;
		vendor->visible = false;
		talker->visible = false;
		exit->visible = false;

        if (sfx_close)
            if (play_sound) Mix_PlayChannel(-1, sfx_close, 0);

	}
}

void MenuManager::closeRight(bool play_sound) {
	if (!dragging) {
		inv->visible = false;
		pow->visible = false;
		talker->visible = false;
		exit->visible = false;

        if (sfx_close)
            if (play_sound) Mix_PlayChannel(-1, sfx_close, 0);
	}
}

MenuManager::~MenuManager() {

	tip_buf.clear();

	delete xp;
	delete mini;
	delete inv;
	delete pow;
	delete chr;
	delete hudlog;
	delete log;
	delete act;
	delete tip;
	delete vendor;
	delete talker;
	delete exit;
	delete enemy;
	delete hpmp;

    if (sfx_open != NULL)
        Mix_FreeChunk(sfx_open);
    if (sfx_close != NULL)
        Mix_FreeChunk(sfx_close);
}
