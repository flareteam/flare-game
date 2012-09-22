/*
Copyright � 2011-2012 Clint Bellanger
Copyright � 2012 Igor Paliychuk
Copyright � 2012 Henrik Andersson

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
 * class GameStatePlay
 *
 * Handles logic and rendering of the main action game play
 * Also handles message passing between child objects, often to avoid circular dependencies.
 */

#include "Avatar.h"
#include "CampaignManager.h"
#include "CombatText.h"
#include "EnemyManager.h"
#include "GameStatePlay.h"
#include "GameState.h"
#include "GameStateTitle.h"
#include "Hazard.h"
#include "HazardManager.h"
#include "LootManager.h"
#include "Menu.h"
#include "MenuActionBar.h"
#include "MenuCharacter.h"
#include "MenuEnemy.h"
#include "MenuHUDLog.h"
#include "MenuInventory.h"
#include "MenuLog.h"
#include "MenuManager.h"
#include "MenuMiniMap.h"
#include "MenuStash.h"
#include "MenuTalker.h"
#include "MenuVendor.h"
#include "NPC.h"
#include "NPCManager.h"
#include "QuestLog.h"
#include "WidgetLabel.h"
#include "SharedResources.h"

using namespace std;

const int MENU_ENEMY_TIMEOUT = MAX_FRAMES_PER_SEC * 10;

GameStatePlay::GameStatePlay() : GameState() {

	hasMusic = true;

	// GameEngine scope variables
	npc_id = -1;
	game_slot = 0;

	// construct gameplay objects
	powers = new PowerManager();
	items = new ItemManager();
	camp = new CampaignManager();
	map = new MapRenderer(camp);
	pc = new Avatar(powers, map);
	enemies = new EnemyManager(powers, map);
	hazards = new HazardManager(powers, pc, enemies);
	menu = new MenuManager(powers, &pc->stats, camp, items);
	loot = new LootManager(items, map, &pc->stats);
	npcs = new NPCManager(map, loot, items, &pc->stats);
	quests = new QuestLog(camp, menu->log);

	// assign some object pointers after object creation, based on dependency order
	camp->items = items;
	camp->carried_items = &menu->inv->inventory[CARRIED];
	camp->currency = &menu->inv->currency;
	camp->hero = &pc->stats;
	map->powers = powers;

	color_normal = font->getColor("menu_normal");

	loading = new WidgetLabel();
	loading->set(VIEW_W_HALF, VIEW_H_HALF, JUSTIFY_CENTER, VALIGN_CENTER, msg->get("Loading..."), color_normal);

	// Load the loading screen image (we currently use the confirm dialog background)
	loading_bg = IMG_Load(mods->locate("images/menus/confirm_bg.png").c_str());
	if(!loading_bg) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_Surface *cleanup = loading_bg;
	loading_bg = SDL_DisplayFormatAlpha(loading_bg);
	SDL_FreeSurface(cleanup);
}

/**
 * Reset all game states to a new game.
 */
void GameStatePlay::resetGame() {
	map->load("spawn.txt");
	camp->clearAll();
	pc->init();
	pc->stats.currency = 0;
	menu->act->clear();
	menu->inv->inventory[0].clear();
	menu->inv->inventory[1].clear();
	menu->inv->changed_equipment = true;
	menu->inv->currency = 0;
	menu->log->clear();
	quests->createQuestList();
	menu->hudlog->clear();
	loadStash();

	// Finalize new character settings
	menu->talker->setHero(pc->stats.name, pc->stats.portrait);
	pc->loadSounds();
}

/**
 * Check mouseover for enemies.
 * class variable "enemy" contains a live enemy on mouseover.
 * This function also sets enemy mouseover for Menu Enemy.
 */
void GameStatePlay::checkEnemyFocus() {
	// determine enemies mouseover
	// only check alive enemies for targeting
	enemy = enemies->enemyFocus(inpt->mouse, map->cam, true);

	if (enemy != NULL) {

		// if there's a living creature in focus, display its stats
		if (!enemy->stats.suppress_hp) {
			menu->enemy->enemy = enemy;
			menu->enemy->timeout = MENU_ENEMY_TIMEOUT;
		}
	}
	else {

		// if there's no living creature in focus, look for a dead one instead
		Enemy *temp_enemy = enemies->enemyFocus(inpt->mouse, map->cam, false);
		if (temp_enemy != NULL) {
			menu->enemy->enemy = temp_enemy;
			menu->enemy->timeout = MENU_ENEMY_TIMEOUT;
		}
	}

}

/**
 * If mouse_move is enabled, and the mouse is over a live enemy,
 * Do not allow power use with button MAIN1
 */
bool GameStatePlay::restrictPowerUse() {
	if(MOUSE_MOVE) {
		if(enemy == NULL && inpt->pressing[MAIN1] && !inpt->pressing[SHIFT] && !(isWithin(menu->act->numberArea,inpt->mouse) || isWithin(menu->act->mouseArea,inpt->mouse) || isWithin(menu->act->menuArea, inpt->mouse))) {
			return true;
		}
	}
	return false;
}

/**
 * Check to see if the player is picking up loot on the ground
 */
void GameStatePlay::checkLoot() {

	ItemStack pickup;
	int currency;

	// Autopickup
    if (pc->stats.alive && AUTOPICKUP_CURRENCY) {
        pickup = loot->checkAutoPickup(pc->stats.pos, currency);
        if (currency > 0) {
            menu->inv->addCurrency(currency);
        }
    }

	// Pickup with mouse click
	if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1] && pc->stats.alive) {

		pickup = loot->checkPickup(inpt->mouse, map->cam, pc->stats.pos, currency, menu->inv);
		if (pickup.item > 0) {
			inpt->lock[MAIN1] = true;
			menu->inv->add(pickup);

			camp->setStatus(menu->items->items[pickup.item].pickup_status);
		}
		else if (currency > 0) {
			inpt->lock[MAIN1] = true;
			menu->inv->addCurrency(currency);
		}
		if (loot->full_msg) {
			inpt->lock[MAIN1] = true;
			menu->log->add(msg->get("Inventory is full."), LOG_TYPE_MESSAGES);
			menu->hudlog->add(msg->get("Inventory is full."));
			loot->full_msg = false;
		}
	}
}

void GameStatePlay::checkTeleport() {

	// both map events and player powers can cause teleportation
	if (map->teleportation || pc->stats.teleportation) {

		map->collider.unblock(pc->stats.pos.x, pc->stats.pos.y);

		if (map->teleportation) {
			map->cam.x = pc->stats.pos.x = map->teleport_destination.x;
			map->cam.y = pc->stats.pos.y = map->teleport_destination.y;
		}
		else {
			map->cam.x = pc->stats.pos.x = pc->stats.teleport_destination.x;
			map->cam.y = pc->stats.pos.y = pc->stats.teleport_destination.y;
		}

		// process intermap teleport
		if (map->teleportation && map->teleport_mapname != "") {
			showLoading();
			map->load(map->teleport_mapname);
			enemies->handleNewMap();
			hazards->handleNewMap(&map->collider);
			loot->handleNewMap();
			powers->handleNewMap(&map->collider);
			menu->enemy->handleNewMap();
			npcs->handleNewMap();
			menu->vendor->npc = NULL;
			menu->vendor->visible = false;
			menu->talker->visible = false;
			menu->stash->visible = false;
			menu->mini->prerender(&map->collider, map->w, map->h);
			npc_id = -1;

			// store this as the new respawn point
			map->respawn_map = map->teleport_mapname;
			map->respawn_point.x = pc->stats.pos.x;
			map->respawn_point.y = pc->stats.pos.y;

			// return to title (permadeath) OR auto-save
			if (pc->stats.permadeath && pc->stats.corpse) {
			    stringstream filename;
			    filename << PATH_USER << "save" << game_slot << ".txt";
			    if(remove(filename.str().c_str()) != 0)
				    perror("Error deleting save from path");

				delete requestedGameState;
				requestedGameState = new GameStateTitle();
			}
			else {
			    saveGame();
			}
		}

		map->collider.block(pc->stats.pos.x, pc->stats.pos.y);

		map->teleportation = false;
		pc->stats.teleportation = false; // teleport spell

	}
}

/**
 * Check for cancel key to exit menus or exit the game.
 * Also check closing the game window entirely.
 */
void GameStatePlay::checkCancel() {

	// if user has clicked exit game from exit menu
	if (menu->requestingExit()) {
		saveGame();
		Mix_HaltMusic();
		delete requestedGameState;
		requestedGameState = new GameStateTitle();
	}

	// if user closes the window
	if (inpt->done) {
		saveGame();
		Mix_HaltMusic();
		exitRequested = true;
	}
}

/**
 * Check for log messages from various child objects
 */
void GameStatePlay::checkLog() {

	// Map events can create messages
	if (map->log_msg != "") {
		menu->log->add(map->log_msg, LOG_TYPE_MESSAGES);
		menu->hudlog->add(map->log_msg);
		map->log_msg = "";
	}

	// The avatar can create messages (e.g. level up)
	if (pc->log_msg != "") {
		menu->log->add(pc->log_msg, LOG_TYPE_MESSAGES);
		menu->hudlog->add(pc->log_msg);
		pc->log_msg = "";
	}

	// Campaign events can create messages (e.g. quest rewards)
	if (camp->log_msg != "") {
		menu->log->add(camp->log_msg, LOG_TYPE_MESSAGES);
		menu->hudlog->add(camp->log_msg);
		camp->log_msg = "";
	}

	// MenuInventory has hints to help the player use items properly
	if (menu->inv->log_msg != "") {
		menu->hudlog->add(menu->inv->log_msg);
		menu->inv->log_msg = "";
	}

	// PowerManager has hints for powers
	if (powers->log_msg != "") {
		menu->hudlog->add(powers->log_msg);
		powers->log_msg = "";
	}
}

void GameStatePlay::checkEquipmentChange() {
	if (menu->inv->changed_equipment) {

		vector<Layer_gfx> img_gfx;
		Layer_gfx gfx;
		// load only displayable layers
		for (int i=0; i<menu->inv->inventory[EQUIPMENT].getSlotNumber(); i++) {
			for (unsigned int j=0; j<pc->layer_def.size(); j++) {
				if (menu->inv->inventory[EQUIPMENT].slot_type[i] == pc->layer_def[j].type) {
					gfx.gfx = menu->items->items[menu->inv->inventory[EQUIPMENT][i].item].gfx;
					gfx.type = menu->inv->inventory[EQUIPMENT].slot_type[i];
					img_gfx.push_back(gfx);
					break;
				}
			}
			if (menu->inv->inventory[EQUIPMENT].slot_type[i] == "body") {
				gfx.gfx = menu->items->items[menu->inv->inventory[EQUIPMENT][i].item].gfx;
				gfx.type = menu->inv->inventory[EQUIPMENT].slot_type[i];
				img_gfx.push_back(gfx);
			}
		}
		pc->loadGraphics(img_gfx);

		pc->loadStepFX(menu->items->items[menu->inv->inventory[EQUIPMENT][1].item].stepfx);

		menu->inv->changed_equipment = false;
	}
}

void GameStatePlay::checkLootDrop() {

	// if the player has dropped an item from the inventory
	if (menu->drop_stack.item > 0) {
		loot->addLoot(menu->drop_stack, pc->stats.pos);
		menu->drop_stack.item = 0;
		menu->drop_stack.quantity = 0;
	}

	// if the player has dropped a quest rward because inventory full
	if (camp->drop_stack.item > 0) {
		loot->addLoot(camp->drop_stack, pc->stats.pos);
		camp->drop_stack.item = 0;
		camp->drop_stack.quantity = 0;
	}

}

/**
 * When a consumable-based power is used, we need to remove it from the inventory.
 */
void GameStatePlay::checkConsumable() {
	if (powers->used_item != -1) {
		if (menu->items->items[powers->used_item].type == "consumable") {
			menu->inv->remove(powers->used_item);
			powers->used_item = -1;
		}
	}
}

/**
 * Marks the menu if it needs attention.
 */
void GameStatePlay::checkNotifications() {
    if (pc->newLevelNotification) {
        pc->newLevelNotification = false;
        menu->act->requires_attention[MENU_CHARACTER] = true;
    }
    if (menu->chr->newPowerNotification) {
        menu->chr->newPowerNotification = false;
        menu->act->requires_attention[MENU_POWERS] = true;
    }
    if (quests->resetQuestNotification) { //remove if no quests
        quests->resetQuestNotification = false;
        menu->act->requires_attention[MENU_LOG] = false;
    }
    if (quests->newQuestNotification) {
        quests->newQuestNotification = false;
        menu->act->requires_attention[MENU_LOG] = true;
    }
}

/**
 * If the player has clicked on an NPC, the game mode might be changed.
 * If a player walks away from an NPC, end the interaction with that NPC
 * If an NPC is giving a reward, process it
 */
void GameStatePlay::checkNPCInteraction() {

	int npc_click = -1;
	int max_interact_distance = UNITS_PER_TILE * 4;
	int interact_distance = max_interact_distance+1;

	// check for clicking on an NPC
	if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) {
		npc_click = npcs->checkNPCClick(inpt->mouse, map->cam);
		if (npc_click != -1) npc_id = npc_click;
	}

	// check distance to this npc
	if (npc_id != -1) {
		interact_distance = (int)calcDist(pc->stats.pos, npcs->npcs[npc_id]->pos);
	}

	// if close enough to the NPC, open the appropriate interaction screen
	if (npc_click != -1 && interact_distance < max_interact_distance && pc->stats.alive && pc->stats.humanoid) {
		inpt->lock[MAIN1] = true;
		bool npc_have_dialog = !(npcs->npcs[npc_id]->chooseDialogNode() == NPC_NO_DIALOG_AVAIL);

		if (npcs->npcs[npc_id]->vendor && !npc_have_dialog) {
			menu->vendor->talker_visible = false;
			menu->talker->vendor_visible = true;
		}
		else if (npcs->npcs[npc_id]->talker && npc_have_dialog) {
			menu->vendor->talker_visible = true;
			menu->talker->vendor_visible = false;

			npcs->npcs[npc_id]->playSound(NPC_VOX_INTRO);
		}
	}

	if (npc_id != -1 && interact_distance < max_interact_distance && pc->stats.alive && pc->stats.humanoid) {

		if (menu->talker->vendor_visible && !menu->vendor->talker_visible) {

			// begin trading
			menu->vendor->setTab(0); // Show the NPC's inventory as opposed to the buyback tab
			menu->vendor->npc = npcs->npcs[npc_id];
			menu->vendor->setInventory();
			menu->closeAll(false);
			menu->talker->visible = false;
			menu->vendor->visible = true;
			menu->inv->visible = true;

			// if this vendor has voice-over, play it
			if (!npcs->npcs[npc_id]->talker) {
				if (!npcs->npcs[npc_id]->playSound(NPC_VOX_INTRO)) {
					Mix_PlayChannel(-1, menu->sfx_open, 0);
				}
			}
			else {
				// unless the vendor has dialog; then they've already given their vox intro
				Mix_PlayChannel(-1, menu->sfx_open, 0);
			}

			menu->talker->vendor_visible = false;
			menu->vendor->talker_visible = false;

		} else if (!menu->talker->vendor_visible && menu->vendor->talker_visible && npcs->npcs[npc_id]->talker) {

			// begin talking
			if (npcs->npcs[npc_id]->vendor) {
				menu->talker->has_vendor_button = true;
				menu->talker->vendor_visible = false;
				menu->vendor->talker_visible = true;
			} else {
				menu->talker->has_vendor_button = false;
			}
			menu->talker->npc = npcs->npcs[npc_id];
			menu->talker->chooseDialogNode();
			menu->closeAll(false);
			menu->talker->visible = true;
			menu->vendor->visible = false;
			menu->inv->visible = false;

			menu->talker->vendor_visible = false;
			menu->vendor->talker_visible = false;
		}
	}

	// check for walking away from an NPC
	if (npc_id != -1) {
		if (interact_distance > max_interact_distance || !pc->stats.alive) {
			menu->vendor->npc = NULL;
			menu->talker->npc = NULL;
			if (menu->vendor->visible || menu->talker->visible) {
 				menu->vendor->visible = false;
				menu->talker->visible = false;
 			}
			npc_id = -1;
		}
	}

}

void GameStatePlay::checkStash() {
	int max_interact_distance = UNITS_PER_TILE * 4;
	int interact_distance = max_interact_distance+1;

	if (map->stash) {
		// If triggered, open the stash and inventory menus
		menu->inv->visible = true;
		menu->stash->visible = true;
		map->stash = false;
	} else {
		// Close stash if inventory is closed
		if (!menu->inv->visible) menu->stash->visible = false;

		// If the player walks away from the stash, close its menu
		interact_distance = (int)calcDist(pc->stats.pos, map->stash_pos);
		if (interact_distance > max_interact_distance || !pc->stats.alive) {
			menu->stash->visible = false;
		}

		// If the stash has been updated, save the game
		if (menu->stash->updated) {
			menu->stash->updated = false;
			saveGame();
		}
	}
}

/**
 * Process all actions for a single frame
 * This includes some message passing between child object
 */
void GameStatePlay::logic() {

	// check menus first (top layer gets mouse click priority)
	menu->logic();

	if (!menu->pause) {

		// these actions only occur when the game isn't paused
		checkLoot();
		checkEnemyFocus();
		checkNPCInteraction();
		map->checkHotspots();

		pc->logic(menu->act->checkAction(inpt->mouse), restrictPowerUse());

		// transfer hero data to enemies, for AI use
		enemies->hero_pos = pc->stats.pos;
		enemies->hero_alive = pc->stats.alive;

		enemies->logic();
		hazards->logic();
		loot->logic();
		enemies->checkEnemiesforXP(&pc->stats);
		npcs->logic();

	}

	// these actions occur whether the game is paused or not.
    checkNotifications();
	checkLootDrop();
	checkTeleport();
	checkLog();
	checkEquipmentChange();
	checkConsumable();
	checkStash();
	checkCancel();

	map->logic();
	quests->logic();


	// change hero powers on transformation
	if (pc->setPowers) {
		pc->setPowers = false;
		menu->closeAll(false);
		// save ActionBar state and lock slots from removing/replacing power
		for (int i=0; i<12 ; i++) {
			menu->act->actionbar[i] = menu->act->hotkeys[i];
			menu->act->hotkeys[i] = 0;
		}
		int count = 10;
		for (int i=0; i<4 ; i++) {
			if (pc->charmed_stats->power_index[i] != 0) {
				menu->act->hotkeys[count] = pc->charmed_stats->power_index[i];
				menu->act->locked[count] = true;
				count++;
			}
			if (count == 12) count = 0;
		}
		if (pc->stats.manual_untransform && pc->untransform_power > 0) {
			menu->act->hotkeys[count] = pc->untransform_power;
			menu->act->locked[count] = true; 
		} else if (pc->stats.manual_untransform && pc->untransform_power == 0)
			fprintf(stderr, "Untransform power not found, you can't untransform manually\n");
	}
	// revert hero powers
	if (pc->revertPowers) {
		pc->revertPowers = false;

		// restore ActionBar state
		for (int i=0; i<12 ; i++) {
			menu->act->hotkeys[i] = menu->act->actionbar[i];
			menu->act->locked[i] = false;
		}
	}
}


/**
 * Render all graphics for a single frame
 */
void GameStatePlay::render() {

	// Create a list of Renderables from all objects not already on the map.
	// split the list into the beings alive (may move) and dead beings (must not move)
	vector<Renderable> rens;
	vector<Renderable> rens_dead;

	Renderable pc_hero = pc->getRender();
	rens.push_back(pc_hero); // Avatar

	// get additional hero overlays
	pc->stats.updateEffects();
	Renderable re = powers->renderEffects(&pc->stats);
	rens.push_back(re);

	enemies->addRenders(rens, rens_dead);

	npcs->addRenders(rens); // npcs cannot be dead

	loot->addRenders(rens, rens_dead);

	hazards->addRenders(rens, rens_dead);


	// render the static map layers plus the renderables
	map->render(rens, rens_dead);

	// mouseover tooltips
	loot->renderTooltips(map->cam);
	npcs->renderTooltips(map->cam, inpt->mouse);

	if (map->map_change) {
		menu->mini->prerender(&map->collider, map->w, map->h);
		map->map_change = false;
	}
	menu->mini->getMapTitle(map->title);
	menu->mini->render(pc->stats.pos);
	menu->render();

    // render combat text last - this should make it obvious you're being
    // attacked, even if you have menus open
    CombatText *combat_text = CombatText::Instance();
    combat_text->setCam(map->cam);
    combat_text->render();
}

void GameStatePlay::showLoading() {
	// SDL_FillRect(screen,NULL,0);

	SDL_Rect dest;
	dest.x = VIEW_W_HALF - loading_bg->w/2;
	dest.y = VIEW_H_HALF - loading_bg->h/2;

	SDL_BlitSurface(loading_bg,NULL,screen,&dest);
	loading->render();

	SDL_Flip(screen);
}

GameStatePlay::~GameStatePlay() {
	delete quests;
	delete npcs;
	delete hazards;
	delete enemies;
	delete pc;
	delete map;
	delete menu;
	delete loot;
	delete camp;
	delete items;
	delete powers;

	delete loading;

	SDL_FreeSurface(loading_bg);
}

