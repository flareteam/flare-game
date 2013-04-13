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
 * class MenuManager
 */

#include "FileParser.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"
#include "Menu.h"
#include "MenuManager.h"
#include "MenuActionBar.h"
#include "MenuCharacter.h"
#include "MenuStatBar.h"
#include "MenuHUDLog.h"
#include "MenuInventory.h"
#include "MenuMiniMap.h"
#include "MenuNPCActions.h"
#include "MenuPowers.h"
#include "MenuEnemy.h"
#include "MenuVendor.h"
#include "MenuTalker.h"
#include "MenuExit.h"
#include "MenuActiveEffects.h"
#include "MenuStash.h"
#include "MenuLog.h"
#include "ModManager.h"
#include "NPC.h"
#include "PowerManager.h"
#include "SharedResources.h"

MenuManager::MenuManager(PowerManager *_powers, StatBlock *_stats, CampaignManager *_camp, ItemManager *_items)
	: icons(NULL)
	, powers(_powers)
	, stats(_stats)
	, camp(_camp)
	, tip_buf()
	, key_lock(false)
	, dragging(0)
	, drag_stack()
	, drag_power(0)
	, drag_src(0)
	, done(false)
	/*std::vector<Menu*> menus;*/
	, items(_items)
	, inv(NULL)
	, pow(NULL)
	, chr(NULL)
	, log(NULL)
	, hudlog(NULL)
	, act(NULL)
	, hp(NULL)
	, mp(NULL)
	, xp(NULL)
	, tip(NULL)
	, mini(NULL)
	, npc(NULL)
	, enemy(NULL)
	, vendor(NULL)
	, talker(NULL)
	, exit(NULL)
	, effects(NULL)
	, stash(NULL)
	, pause(false)
	, menus_open(false)
	, drop_stack()
{
	loadIcons();

	hp = new MenuStatBar("hp");
	menus.push_back(hp); // menus[0]
	mp = new MenuStatBar("mp");
	menus.push_back(mp); // menus[1]
	xp = new MenuStatBar("xp");
	menus.push_back(xp); // menus[2]
	effects = new MenuActiveEffects(icons);
	menus.push_back(effects); // menus[3]
	hudlog = new MenuHUDLog();
	menus.push_back(hudlog); // menus[4]
	act = new MenuActionBar(powers, stats, icons);
	menus.push_back(act); // menus[5]
	enemy = new MenuEnemy();
	menus.push_back(enemy); // menus[6]
	vendor = new MenuVendor(items, stats);
	menus.push_back(vendor); // menus[7]
	talker = new MenuTalker(this, camp);
	menus.push_back(talker); // menus[8]
	exit = new MenuExit();
	menus.push_back(exit); // menus[9]
	mini = new MenuMiniMap();
	menus.push_back(mini); // menus[10]
	chr = new MenuCharacter(stats);
	menus.push_back(chr); // menus[11]
	inv = new MenuInventory(items, stats, powers);
	menus.push_back(inv); // menus[12]
	pow = new MenuPowers(stats, powers, icons);
	menus.push_back(pow); // menus[13]
	log = new MenuLog();
	menus.push_back(log); // menus[14]
	stash = new MenuStash(items, stats);
	menus.push_back(stash); // menus[15]
	npc = new MenuNPCActions();
	menus.push_back(npc); // menus[16]

	tip = new WidgetTooltip();

	// Load the menu layout and sound effects from menus/menus.txt
	FileParser infile;
	if (infile.open(mods->locate("menus/menus.txt"))) {

		int menu_index = -1;

		while (infile.next()) {

			if (infile.key == "id") {

				/* finalize previously parsed menu */
				if (menu_index != -1)
					menus[menu_index]->align();

				if (infile.val == "hp") menu_index = 0;
				else if (infile.val == "mp") menu_index = 1;
				else if (infile.val == "xp") menu_index = 2;
				else if (infile.val == "effects") menu_index = 3;
				else if (infile.val == "hudlog") menu_index = 4;
				else if (infile.val == "actionbar") menu_index = 5;
				else if (infile.val == "enemy") menu_index = 6;
				else if (infile.val == "vendor") menu_index = 7;
				else if (infile.val == "talker") menu_index = 8;
				else if (infile.val == "exit") menu_index = 9;
				else if (infile.val == "minimap") menu_index = 10;
				else if (infile.val == "character") menu_index = 11;
				else if (infile.val == "inventory") menu_index = 12;
				else if (infile.val == "powers") menu_index = 13;
				else if (infile.val == "log") menu_index = 14;
				else if (infile.val == "stash") menu_index = 15;
				else if (infile.val == "npc") menu_index = 16;
				else menu_index = -1;

			}

			if (menu_index == -1)
				continue;

			if (infile.key == "layout") {

				infile.val = infile.val + ',';
				int x = eatFirstInt(infile.val, ',');
				int y = eatFirstInt(infile.val, ',');
				int w = eatFirstInt(infile.val, ',');
				int h = eatFirstInt(infile.val, ',');

				menus[menu_index]->window_area.x = x;
				menus[menu_index]->window_area.y = y;
				menus[menu_index]->window_area.w = w;
				menus[menu_index]->window_area.h = h;

			} else if (infile.key == "align") {
				menus[menu_index]->alignment = infile.val;
			} else if (infile.key == "soundfx_open") {
				menus[menu_index]->sfx_open = snd->load(infile.val, "MenuManager open tab");
			} else if (infile.key == "soundfx_close") {
				menus[menu_index]->sfx_close = snd->load(infile.val, "MenuManager close tab");
			}

		}

		infile.close();
	} else fprintf(stderr, "Unable to open menus/menus.txt!\n");

	// Some menus need to be updated to apply their new dimensions
	act->update();
	vendor->update();
	vendor->buyback_stock.init(NPC_VENDOR_MAX_STOCK, items);
	talker->update();
	exit->update();
	chr->update();
	inv->update();
	pow->update();
	log->update();
	stash->update();

	pause = false;
	dragging = false;
	drag_stack.item = 0;
	drag_stack.quantity = 0;
	drag_power = 0;
	drag_src = 0;
	drop_stack.item = 0;
	drop_stack.quantity = 0;

	done = false;

	closeAll(); // make sure all togglable menus start closed
}

/**
 * Icon set shared by all menus
 */
void MenuManager::loadIcons() {

	icons = IMG_Load(mods->locate("images/icons/icons.png").c_str());
	if (!icons) {
		fprintf(stderr, "Couldn't load icons: %s\n", IMG_GetError());
	} else {
		// optimize
		SDL_Surface *cleanup = icons;
		icons = SDL_DisplayFormatAlpha(icons);
		SDL_FreeSurface(cleanup);
	}
}

void MenuManager::renderIcon(int icon_id, int x, int y) {
	SDL_Rect src;
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	src.w = src.h = dest.w = dest.h = ICON_SIZE;

	int columns = icons->w / ICON_SIZE;
	src.x = (icon_id % columns) * ICON_SIZE;
	src.y = (icon_id / columns) * ICON_SIZE;

	SDL_BlitSurface(icons, &src, screen, &dest);
}

void MenuManager::logic() {

	bool clicking_character = false;
	bool clicking_inventory = false;
	bool clicking_powers = false;
	bool clicking_log = false;
	ItemStack stack;

	hp->update(stats->hp,stats->maxhp,inpt->mouse,"");
	mp->update(stats->mp,stats->maxmp,inpt->mouse,"");
	xp->update((stats->xp - stats->xp_table[stats->level-1]),(stats->xp_table[stats->level] - stats->xp_table[stats->level-1]),inpt->mouse,msg->get("XP: %d/%d", stats->xp, stats->xp_table[stats->level]));
	effects->update(stats);

	hudlog->logic();
	enemy->logic();
	chr->logic();
	inv->logic();
	vendor->logic();
	pow->logic();
	log->logic();
	talker->logic();
	stash->logic();

	if (chr->checkUpgrade() || stats->level_up) {
		// apply equipment and max hp/mp
		inv->applyEquipment(inv->inventory[EQUIPMENT].storage);
		stats->hp = stats->maxhp;
		stats->mp = stats->maxmp;
		stats->level_up = false;
	}

	// only allow the vendor window to be open if the inventory is open
	if (vendor->visible && !(inv->visible)) {
		snd->play(vendor->sfx_close);
		closeLeft();
		if (vendor->talker_visible && !(inv->visible))
			closeRight();
	}

	if (!inpt->pressing[INVENTORY] && !inpt->pressing[POWERS] && !inpt->pressing[CHARACTER] && !inpt->pressing[LOG])
		key_lock = false;

	// handle npc action menu
	if (npc->visible) {
	  npc->logic();
	}

	// check if mouse-clicking a menu button
	act->checkMenu(inpt->mouse, clicking_character, clicking_inventory, clicking_powers, clicking_log);

	if (exit->visible) {
		exit->logic();
		if (exit->isExitRequested()) {
			done = true;
		}
	}

	// exit menu toggle
	if ((!key_lock && !dragging) && !(stats->corpse && stats->permadeath)) {
		if (inpt->pressing[CANCEL] && !inpt->lock[CANCEL]) {
			inpt->lock[CANCEL] = true;
			key_lock = true;
			if (menus_open) {
				closeAll();
			}
			else {
				exit->visible = !exit->visible;
			}
		}
	}

	// inventory menu toggle
	if ((inpt->pressing[INVENTORY] && !key_lock && !dragging) || clicking_inventory) {
		key_lock = true;
		if (inv->visible) {
			snd->play(inv->sfx_close);
			closeRight();
		}
		else {
			closeRight();
			act->requires_attention[MENU_INVENTORY] = false;
			inv->visible = true;
			snd->play(inv->sfx_open);
		}

	}

	// powers menu toggle
	if (((inpt->pressing[POWERS] && !key_lock && !dragging) || clicking_powers) && stats->humanoid) {
		key_lock = true;
		if (pow->visible) {
			snd->play(pow->sfx_close);
			closeRight();
		}
		else {
			closeRight();
			act->requires_attention[MENU_POWERS] = false;
			pow->visible = true;
			snd->play(pow->sfx_open);
		}
	}
	act->requires_attention[MENU_POWERS] = pow->getUnspent() > 0;

	// character menu toggleggle
	if (((inpt->pressing[CHARACTER] && !key_lock && !dragging) || clicking_character)) {
		key_lock = true;
		if (chr->visible) {
			snd->play(chr->sfx_close);
			closeLeft();
		}
		else {
			closeLeft();
			act->requires_attention[MENU_CHARACTER] = false;
			chr->visible = true;
			snd->play(chr->sfx_open);
			// Make sure the stat list isn't scrolled when we open the character menu
			inpt->resetScroll();
		}
	}
	act->requires_attention[MENU_CHARACTER] = chr->getUnspent() > 0;

	// log menu toggle
	if ((inpt->pressing[LOG] && !key_lock && !dragging) || clicking_log) {
		key_lock = true;
		if (log->visible) {
			snd->play(log->sfx_close);
			closeLeft();
		}
		else {
			closeLeft();
			act->requires_attention[MENU_LOG] = false;
			log->visible = true;
			snd->play(log->sfx_open);
			// Make sure the log isn't scrolled when we open the log menu
			inpt->resetScroll();
		}
	}

	if (MENUS_PAUSE) {
		pause = (inv->visible || pow->visible || chr->visible || log->visible || vendor->visible || talker->visible || npc->visible);
	}
	menus_open = (inv->visible || pow->visible || chr->visible || log->visible || vendor->visible || talker->visible || npc->visible);

	if (ENABLE_JOYSTICK && (menus_open || exit->visible)) inpt->enableMouseEmulation();
	else inpt->disableMouseEmulation();

	if (stats->alive) {

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

			if (chr->visible && isWithin(chr->window_area, inpt->mouse)) {
				inpt->lock[MAIN1] = true;
			}

			if (vendor->visible && isWithin(vendor->window_area,inpt->mouse)) {
				inpt->lock[MAIN1] = true;
				vendor->tabsLogic();
				if (inpt->pressing[CTRL]) {
					// buy item from a vendor
					stack = vendor->click(inpt);
					if (stack.item > 0) {
						if (!inv->buy(stack,vendor->getTab())) {
							log->add(msg->get("Not enough money."), LOG_TYPE_MESSAGES);
							hudlog->add(msg->get("Not enough money."));
							vendor->itemReturn( stack);
						} else {
							if (inv->full(stack.item)) {
								log->add(msg->get("Inventory is full."), LOG_TYPE_MESSAGES);
								hudlog->add(msg->get("Inventory is full."));
								drop_stack = stack;
							} else {
								inv->add(stack);
							}
						}
					}
				} else {
					// start dragging a vendor item
					drag_stack = vendor->click(inpt);
					if (drag_stack.item > 0) {
						dragging = true;
						drag_src = DRAG_SRC_VENDOR;
					}
				}
			}

			if (stash->visible && isWithin(stash->window_area,inpt->mouse)) {
				inpt->lock[MAIN1] = true;
				if (inpt->pressing[CTRL]) {
					// take an item from the stash
					stack = stash->click(inpt);
					if (stack.item > 0) {
						if (inv->full(stack.item)) {
							log->add(msg->get("Inventory is full."), LOG_TYPE_MESSAGES);
							hudlog->add(msg->get("Inventory is full."));
							drop_stack = stack;
						} else {
							inv->add(stack);
						}
						stash->updated = true;
					}
				} else {
					// start dragging a stash item
					drag_stack = stash->click(inpt);
					if (drag_stack.item > 0) {
						dragging = true;
						drag_src = DRAG_SRC_STASH;
					}
				}
			}

			if (log->visible && isWithin(log->window_area,inpt->mouse)) {
				inpt->lock[MAIN1] = true;
			}

			// pick up an inventory item
			if (inv->visible && isWithin(inv->window_area,inpt->mouse)) {
				if (inpt->pressing[CTRL]) {
					inpt->lock[MAIN1] = true;
					stack = inv->click(inpt);
					if (stack.item > 0) {
						if (stash->visible) {
							if (inv->stashAdd(stack) && !stash->full(stack.item)) {
								stash->add(stack);
								stash->updated = true;
							}
							else {
								inv->itemReturn(stack);
							}
						}
						else {
							// The vendor could have a limited amount of money in the future. It will be tested here.
							if ((SELL_WITHOUT_VENDOR || vendor->visible) && inv->sell(stack)) {
								vendor->setTab(VENDOR_SELL);
								vendor->add(stack);
							}
							else {
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
			if (pow->visible && isWithin(pow->window_area,inpt->mouse)) {
				inpt->lock[MAIN1] = true;

				// check for unlock first
				if (!pow->unlockClick(inpt->mouse)) {

					// otherwise, check for dragging
					drag_power = pow->click(inpt->mouse);
					if (drag_power > 0) {
						dragging = true;
						drag_src = DRAG_SRC_POWERS;
					}
				}
			}
			// action bar
			if (isWithin(act->numberArea,inpt->mouse) || isWithin(act->mouseArea,inpt->mouse) || isWithin(act->menuArea, inpt->mouse)) {
				inpt->lock[MAIN1] = true;

				// ctrl-click action bar to clear that slot
				if (inpt->pressing[CTRL]) {
					act->remove(inpt->mouse);
				}
				// allow drag-to-rearrange action bar
				else if (!isWithin(act->menuArea, inpt->mouse)) {
					drag_power = act->checkDrag(inpt->mouse);
					if (drag_power > 0) {
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
				// for locked slots forbid power dropping
				} else if (act->locked[act->drag_prev_slot]) {
					act->hotkeys[act->drag_prev_slot] = drag_power;
				}
			}

			// rearranging inventory or dropping items
			else if (drag_src == DRAG_SRC_INVENTORY) {

				if (inv->visible && isWithin(inv->window_area, inpt->mouse)) {
					inv->drop(inpt->mouse, drag_stack);
					drag_stack.item = 0;
				}
				else if (isWithin(act->numberArea,inpt->mouse) || isWithin(act->mouseArea,inpt->mouse)) {
					// The action bar is not storage!
					inv->itemReturn(drag_stack);

					// put an item with a power on the action bar
					if (items->items[drag_stack.item].power != 0) {
						act->drop(inpt->mouse, items->items[drag_stack.item].power, false);
					}
				}
				else if (vendor->visible && isWithin(vendor->slots_area, inpt->mouse)) {
					if (inv->sell( drag_stack)) {
						vendor->setTab(VENDOR_SELL);
						vendor->add( drag_stack);
					}
					else {
						inv->itemReturn(drag_stack);
					}
					drag_stack.item = 0;
				}
				else if (stash->visible && isWithin(stash->slots_area, inpt->mouse)) {
					if (inv->stashAdd( drag_stack) && !stash->full(drag_stack.item)) {
						stash->drop(inpt->mouse, drag_stack);
						stash->updated = true;
					}
					else {
						inv->itemReturn(drag_stack);
					}
					drag_stack.item = 0;
				}
				else {
					// if dragging and the source was inventory, drop item to the floor

					// quest items cannot be dropped
					if (items->items[drag_stack.item].type != "quest") {
						drop_stack = drag_stack;
						drag_stack.item = 0;
						drag_stack.quantity = 0;
						inv->clearHighlight();
					}
					else {
						inv->itemReturn(drag_stack);
					}
				}
			}

			else if (drag_src == DRAG_SRC_VENDOR) {

				// dropping an item from vendor (we only allow to drop into the carried area)
				if (inv->visible && isWithin( inv->carried_area, inpt->mouse)) {
					if (!inv->buy(drag_stack,vendor->getTab())) {
						log->add(msg->get("Not enough money."), LOG_TYPE_MESSAGES);
						hudlog->add(msg->get("Not enough money."));
						vendor->itemReturn( drag_stack);
					} else {
						if (inv->full(drag_stack.item)) {
							log->add(msg->get("Inventory is full."), LOG_TYPE_MESSAGES);
							hudlog->add(msg->get("Inventory is full."));
							drop_stack = drag_stack;
						} else {
							inv->drop(inpt->mouse,drag_stack);
						}
					}
					drag_stack.item = 0;
					drag_stack.quantity = 0;
				}
				else {
					vendor->itemReturn(drag_stack);
				}
			}

			else if (drag_src == DRAG_SRC_STASH) {

				// dropping an item from stash (we only allow to drop into the carried area)
				if (inv->visible && isWithin( inv->carried_area, inpt->mouse)) {
					if (inv->full(drag_stack.item)) {
						log->add(msg->get("Inventory is full."), LOG_TYPE_MESSAGES);
						hudlog->add(msg->get("Inventory is full."));
						// quest items cannot be dropped
						if (items->items[drag_stack.item].type != "quest") {
							drop_stack = drag_stack;
						} else {
							stash->itemReturn(drag_stack);
						}
					} else {
						inv->drop(inpt->mouse,drag_stack);
					}
					stash->updated = true;
					drag_stack.item = 0;
					drag_stack.quantity = 0;
				}
				else if (stash->visible && isWithin(stash->slots_area, inpt->mouse)) {
					stash->drop(inpt->mouse,drag_stack);
				}
				else {
					stash->itemReturn( drag_stack);
				}
			}

			dragging = false;
		}

	} else {
		if (dragging) {
			if (drag_src == DRAG_SRC_VENDOR) vendor->itemReturn(drag_stack);
			else if (drag_src == DRAG_SRC_STASH) stash->itemReturn(drag_stack);
			else if (drag_src == DRAG_SRC_INVENTORY) inv->itemReturn(drag_stack);
			else if (drag_src == DRAG_SRC_ACTIONBAR) act->actionReturn(drag_power);
			drag_src = -1;
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
			if (item_id != -1 && items->items[item_id].type == "consumable") {
				act->slot_item_count[i] = inv->getItemCountCarried(item_id);
				if (act->slot_item_count[i] == 0) {
					act->slot_enabled[i] = false;
				}
			}
			else if (item_id != -1) {

				// if a non-consumable item power is unequipped, disable that slot
				if (!inv->isItemEquipped(item_id)) {
					act->slot_item_count[i] = 0;
					act->slot_enabled[i] = false;
				}
			}
		}
	}

}

void MenuManager::render() {
	for (unsigned int i=0; i<menus.size(); i++) {
		menus[i]->render();
	}

	TooltipData tip_new;

	// Find tooltips depending on mouse position
	if (chr->visible && isWithin(chr->window_area,inpt->mouse)) {
		tip_new = chr->checkTooltip();
	}
	if (vendor->visible && isWithin(vendor->window_area,inpt->mouse)) {
		tip_new = vendor->checkTooltip(inpt->mouse);
	}
	if (stash->visible && isWithin(stash->window_area,inpt->mouse)) {
		tip_new = stash->checkTooltip(inpt->mouse);
	}
	if (pow->visible && isWithin(pow->window_area,inpt->mouse)) {
		tip_new = pow->checkTooltip(inpt->mouse);
	}
	if (inv->visible && !dragging && isWithin(inv->window_area,inpt->mouse)) {
		tip_new = inv->checkTooltip(inpt->mouse);
	}
	if (isWithin(act->window_area,inpt->mouse)) {
		tip_new = act->checkTooltip(inpt->mouse);
	}

	if (!tip_new.isEmpty()) {

		// when we render a tooltip it buffers the rasterized text for performance.
		// If this new tooltip is the same as the existing one, reuse.

		if (!tip_new.compare(&tip_buf)) {
			tip_buf.clear();
			tip_buf = tip_new;
		}
		tip->render(tip_buf, inpt->mouse, STYLE_FLOAT);
		TOOLTIP_CONTEXT = TOOLTIP_MENU;
	} else if (TOOLTIP_CONTEXT != TOOLTIP_MAP) {
		TOOLTIP_CONTEXT = TOOLTIP_NONE;
	}

	// draw icon under cursor if dragging
	if (dragging) {
		if (drag_src == DRAG_SRC_INVENTORY || drag_src == DRAG_SRC_VENDOR || drag_src == DRAG_SRC_STASH)
			items->renderIcon(drag_stack, inpt->mouse.x - ICON_SIZE/2, inpt->mouse.y - ICON_SIZE/2, ICON_SIZE);
		else if (drag_src == DRAG_SRC_POWERS || drag_src == DRAG_SRC_ACTIONBAR)
			renderIcon(powers->powers[drag_power].icon, inpt->mouse.x-ICON_SIZE/2, inpt->mouse.y-ICON_SIZE/2);
	}

}

void MenuManager::closeAll() {
	if (!dragging) {
		closeLeft();
		closeRight();
		vendor->talker_visible = false;
	}
}

void MenuManager::closeLeft() {
	if (!dragging) {
		chr->visible = false;
		log->visible = false;
		vendor->visible = false;
		talker->visible = false;
		exit->visible = false;
		stash->visible = false;
		npc->visible = false;
	}
}

void MenuManager::closeRight() {
	if (!dragging) {
		inv->visible = false;
		pow->visible = false;
		talker->visible = false;
		exit->visible = false;
		npc->visible = false;
	}
}

MenuManager::~MenuManager() {

	tip_buf.clear();

	delete hp;
	delete mp;
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
	delete effects;
	delete stash;
	delete npc;

	SDL_FreeSurface(icons);
}
