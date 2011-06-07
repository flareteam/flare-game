/**
 * class MenuManager
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuManager.h"

MenuManager::MenuManager(PowerManager *_powers, SDL_Surface *_screen, InputState *_inp, FontEngine *_font, StatBlock *_stats, CampaignManager *_camp) {
	powers = _powers;
	screen = _screen;
	inp = _inp;
	font = _font;
	stats = _stats;
	powers = _powers;
	camp = _camp;

	loadIcons();

	items = new ItemDatabase(screen, font);
	inv = new MenuInventory(screen, font, items, stats, powers);
	pow = new MenuPowers(screen, font, stats, powers);
	chr = new MenuCharacter(screen, font, stats);
	log = new MenuLog(screen, font);
	hudlog = new MenuHUDLog(screen, font);
	act = new MenuActionBar(screen, font, inp, powers, icons);
	hpmp = new MenuHPMP(screen, font);
	tip = new MenuTooltip(font, screen);
	mini = new MenuMiniMap(screen);
	xp = new MenuExperience(screen, font);
	enemy = new MenuEnemy(screen, font);
	vendor = new MenuVendor(screen, font, items, stats);
	talker = new MenuTalker(screen, font, camp);
	exit = new MenuExit(screen, inp, font);
	
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
	
	icons = IMG_Load("images/icons/icons32.png");
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
	sfx_open = Mix_LoadWAV("soundfx/inventory/inventory_page.ogg");
	sfx_close = Mix_LoadWAV("soundfx/inventory/inventory_book.ogg");
	
	if (!sfx_open || !sfx_close) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
		SDL_Quit();
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
	inv->logic();
	talker->logic(inp->pressing[ACCEPT]);

	if (!inp->pressing[INVENTORY] && !inp->pressing[POWERS] && !inp->pressing[CHARACTER] && !inp->pressing[LOG])
		key_lock = false;
	
	// check if mouse-clicking a menu button
	act->checkMenu(inp->mouse, clicking_character, clicking_inventory, clicking_powers, clicking_log);

	// exit menu toggle
	if ((inp->pressing[CANCEL] && !inp->lock[CANCEL] && !key_lock && !dragging)) {
		inp->lock[CANCEL] = true;
		key_lock = true;
		if (menus_open) {
			closeAll(true);
		}
		else {
			exit->visible = !exit->visible;
		}
	}

	// inventory menu toggle
	if ((inp->pressing[INVENTORY] && !key_lock && !dragging) || clicking_inventory) {
		key_lock = true;
		if (inv->visible) {
			closeRight(true);
		}
		else {
			closeRight(false);
			inv->visible = true;
			Mix_PlayChannel(-1, sfx_open, 0);
		}
		
	}

	// powers menu toggle
	if ((inp->pressing[POWERS] && !key_lock && !dragging) || clicking_powers) {
		key_lock = true;
		if (pow->visible) {
			closeRight(true);
		}
		else {
			closeRight(false);
			pow->visible = true;
			Mix_PlayChannel(-1, sfx_open, 0);
		}
	}

	// character menu toggle
	if ((inp->pressing[CHARACTER] && !key_lock && !dragging) || clicking_character) {
		key_lock = true;
		if (chr->visible) {
			closeLeft(true);
		}
		else {
			closeLeft(false);
			chr->visible = true;
			Mix_PlayChannel(-1, sfx_open, 0);
		}
	}
	
	// log menu toggle
	if ((inp->pressing[LOG] && !key_lock && !dragging) || clicking_log) {
		key_lock = true;
		if (log->visible) {
			closeLeft(true);
		}
		else {
			closeLeft(false);
			log->visible = true;
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
		if (!dragging && inp->pressing[MAIN2] && !inp->lock[MAIN2]) {

			// activate inventory item
			if (inv->visible && isWithin( inv->carried_area, inp->mouse)) {
				inv->activate(inp);
				inp->lock[MAIN2] = true;
			}
		}
		
		// handle left-click
		if (!dragging && inp->pressing[MAIN1] && !inp->lock[MAIN1]) {
		
			// left side menu
			if (inp->mouse.x <= 320 && inp->mouse.y >= offset_y && inp->mouse.y <= offset_y+416) {
				if (chr->visible) {
				
					// applied a level-up
					if (chr->checkUpgrade(inp->mouse)) {
						inp->lock[MAIN1] = true;
						
						// apply equipment and max hp/mp
						items->applyEquipment(stats, inv->inventory[EQUIPMENT].storage);
						stats->hp = stats->maxhp;
						stats->mp = stats->maxmp;
					}
				}
				else if (vendor->visible) {
				
					if (inp->pressing[CTRL]) {
						inp->lock[MAIN1] = true;
						
						// buy item from a vendor
						if (!inv->full()) {
							stack = vendor->click(inp);
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
						drag_stack = vendor->click(inp);
						if (drag_stack.item > 0) {
							dragging = true;
							drag_src = DRAG_SRC_VENDOR;
							inp->lock[MAIN1] = true;
						}
					}
				
				}
				else if (log->visible) {
				
					// click on a log tab to make it the active display
					if (isWithin(log->tabs_area, inp->mouse)) {
						log->clickTab(inp->mouse);
						inp->lock[MAIN1] = true;
					}
				}
			}
		
			// right side menu
			else if (inp->mouse.x >= offset_x && inp->mouse.y >= offset_y && inp->mouse.y <= offset_y+416) {
			
				// pick up an inventory item
				if (inv->visible) {
				
					if (inp->pressing[CTRL]) {
						inp->lock[MAIN1] = true;
						stack = inv->click(inp);
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
						drag_stack = inv->click(inp);
						if (drag_stack.item > 0) {
							dragging = true;
							drag_src = DRAG_SRC_INVENTORY;
							inp->lock[MAIN1] = true;
						}
					}
				}
				// pick up a power
				else if (pow->visible) {
					drag_power = pow->click(inp->mouse);
					if (drag_power > -1) {
						dragging = true;
						drag_src = DRAG_SRC_POWERS;
						inp->lock[MAIN1] = true;
					}
				}
			}
			// action bar
			else if (isWithin(act->numberArea,inp->mouse) || isWithin(act->mouseArea,inp->mouse) || isWithin(act->menuArea, inp->mouse)) {
			
				// ctrl-click action bar to clear that slot
				if (inp->pressing[CTRL]) {
					act->remove(inp->mouse);
					inp->lock[MAIN1] = true;
				}
				// allow drag-to-rearrange action bar
				else if (!isWithin(act->menuArea, inp->mouse)) {
					drag_power = act->checkDrag(inp->mouse);
					if (drag_power > -1) {
						dragging = true;
						drag_src = DRAG_SRC_ACTIONBAR;
						inp->lock[MAIN1] = true;
					}
				}
				
				// else, clicking action bar to use a power?
				// this check is done by GameEngine when calling Avatar::logic()


			}
		}
		// handle dropping
		if (dragging && !inp->pressing[MAIN1]) {
			
			// putting a power on the Action Bar
			if (drag_src == DRAG_SRC_POWERS) {
				if (isWithin(act->numberArea,inp->mouse) || isWithin(act->mouseArea,inp->mouse)) {
					act->drop(inp->mouse, drag_power, 0);
				}
			}
			
			// rearranging the action bar
			else if (drag_src == DRAG_SRC_ACTIONBAR) {
				if (isWithin(act->numberArea,inp->mouse) || isWithin(act->mouseArea,inp->mouse)) {
					act->drop(inp->mouse, drag_power, 1);
				}
			}
		
			// rearranging inventory or dropping items
			else if (drag_src == DRAG_SRC_INVENTORY) {
			
				if (inv->visible && inp->mouse.x >= offset_x && inp->mouse.y >= offset_y && inp->mouse.y <= offset_y+416) {
					inv->drop(inp->mouse, drag_stack);
					drag_stack.item = 0;
				}
				else if (isWithin(act->numberArea,inp->mouse) || isWithin(act->mouseArea,inp->mouse)) {
					// The action bar is not storage!
					inv->itemReturn(drag_stack);

					// put an item with a power on the action bar
					if (items->items[drag_stack.item].power != -1) {
						act->drop(inp->mouse, items->items[drag_stack.item].power, false);
					}
				}
				else if (vendor->visible && isWithin(vendor->slots_area, inp->mouse)) {
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
				if (inv->visible && isWithin( inv->carried_area, inp->mouse)) {
					if( inv->full()) {
						// Can we say "Not enough place" ?
						vendor->itemReturn( drag_stack);
					}
					else if( ! inv->buy( drag_stack, inp->mouse)) {
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

		exit->logic();
		if (exit->isExitRequested()) {
			done = true;
		}
	}
	
	// handle equipment changes affecting hero stats
	if (inv->changed_equipment || inv->changed_artifact) {
		items->applyEquipment(stats, inv->inventory[EQUIPMENT].storage);
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
	hpmp->render(stats, inp->mouse);
	xp->render(stats, inp->mouse);
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
	
	TooltipData tooltip;
	int offset_x = (VIEW_W - 320);
	int offset_y = (VIEW_H - 416)/2;

	// Find tooltips depending on mouse position	
	if (inp->mouse.x < 320 && inp->mouse.y >= offset_y && inp->mouse.y <= offset_y+416) {
		if (chr->visible) {
			tooltip = chr->checkTooltip(inp->mouse);
		}
		else if (vendor->visible) {
			tooltip = vendor->checkTooltip(inp->mouse);
		}
	}
	else if (inp->mouse.x >= offset_x && inp->mouse.y >= offset_y && inp->mouse.y <= offset_y+416) {
		if (pow->visible) {
			tooltip = pow->checkTooltip(inp->mouse);
		}
		else if (inv->visible && !dragging) {
			tooltip = inv->checkTooltip(inp->mouse);
		}
	}
	else if (inp->mouse.y >= VIEW_H-32) {
		tooltip = act->checkTooltip(inp->mouse);
	}
	
	if (tooltip.num_lines > 0) {
		tip->render(tooltip, inp->mouse, STYLE_FLOAT);
	}
	
	// draw icon under cursor if dragging
	if (dragging) {
		if (drag_src == DRAG_SRC_INVENTORY || drag_src == DRAG_SRC_VENDOR)
			items->renderIcon(drag_stack, inp->mouse.x - 16, inp->mouse.y - 16, ICON_SIZE_32);
		else if (drag_src == DRAG_SRC_POWERS || drag_src == DRAG_SRC_ACTIONBAR)
			renderIcon(powers->powers[drag_power].icon, inp->mouse.x-16, inp->mouse.y-16);
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

		if (play_sound) Mix_PlayChannel(-1, sfx_close, 0);
		
	}
}

void MenuManager::closeRight(bool play_sound) {
	if (!dragging) {
		inv->visible = false;
		pow->visible = false;	
		talker->visible = false;
		exit->visible = false;

		if (play_sound) Mix_PlayChannel(-1, sfx_close, 0);
	}
}

MenuManager::~MenuManager() {
	delete xp;
	delete mini;
	delete items;
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
	
	Mix_FreeChunk(sfx_open);
	Mix_FreeChunk(sfx_close);
}
