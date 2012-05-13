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
 * class GameStatePlay
 *
 * Handles logic and rendering of the main action game play
 * Also handles message passing between child objects, often to avoid circular dependencies.
 */

#include "GameStatePlay.h"
#include "GameState.h"
#include "GameStateTitle.h"
#include "WidgetLabel.h"
#include "SharedResources.h"

using namespace std;


GameStatePlay::GameStatePlay() : GameState() {

	hasMusic = true;
	
	// GameEngine scope variables
	npc_id = -1;
	game_slot = 0;

	// construct gameplay objects
	powers = new PowerManager();
	items = new ItemManager();
	camp = new CampaignManager();
	map = new MapIso(camp);
	pc = new Avatar(powers, map);
	enemies = new EnemyManager(powers, map);
	hazards = new HazardManager(powers, pc, enemies);
	menu = new MenuManager(powers, &pc->stats, camp, items);
	loot = new LootManager(items, enemies, map);
	npcs = new NPCManager(map, loot, items);
	quests = new QuestLog(camp, menu->log);

	// assign some object pointers after object creation, based on dependency order
	camp->items = items;
	camp->carried_items = &menu->inv->inventory[CARRIED];
	camp->currency = &menu->inv->gold;
	camp->xp = &pc->stats.xp;
	map->powers = powers;

	// display the name of the map in the upper-right hand corner
	label_mapname = new WidgetLabel();

	label_fps = new WidgetLabel();
}

/**
 * Reset all game states to a new game.
 */
void GameStatePlay::resetGame() {
	map->load("spawn.txt");
	camp->clearAll();
	pc->init();
	pc->stats.gold = 0;
	menu->act->clear();
	menu->inv->inventory[0].clear();
	menu->inv->inventory[1].clear();
	menu->inv->changed_equipment = true;
	menu->inv->gold = 0;
	menu->log->clear();
	quests->createQuestList();
	menu->hudlog->clear();
	
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
		menu->enemy->enemy = enemy;
		menu->enemy->timeout = MENU_ENEMY_TIMEOUT;
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
	if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1] && pc->stats.alive) {

		ItemStack pickup;
		int gold;
		
		pickup = loot->checkPickup(inpt->mouse, map->cam, pc->stats.pos, gold, menu->inv->full());
		if (pickup.item > 0) {
			inpt->lock[MAIN1] = true;
			menu->inv->add(pickup);

			camp->setStatus(menu->items->items[pickup.item].pickup_status);
		}
		else if (gold > 0) {
			inpt->lock[MAIN1] = true;
			menu->inv->addGold(gold);
		}
		if (loot->full_msg) {
			inpt->lock[MAIN1] = true;
			menu->log->add(msg->get("Inventory is full."), LOG_TYPE_MESSAGES);
			loot->full_msg = false;
		}
	}
}

void GameStatePlay::checkTeleport() {
	if (map->teleportation || pc->stats.teleportation) {
		
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
			map->load(map->teleport_mapname);
			enemies->handleNewMap();
			hazards->handleNewMap(&map->collider);
			loot->handleNewMap();
			powers->handleNewMap(&map->collider);
			menu->enemy->handleNewMap();
			npcs->handleNewMap();
			menu->vendor->npc = NULL;
			menu->vendor->visible = false;
			npc_id = -1;
			
			// store this as the new respawn point
			map->respawn_map = map->teleport_mapname;
			map->respawn_point.x = pc->stats.pos.x;
			map->respawn_point.y = pc->stats.pos.y;
			
			// return to title (permadeath) OR auto-save
			if (pc->permadeath && pc->stats.corpse) {
			    stringstream filename;
			    filename << PATH_USER << "save" << game_slot << ".txt";
			    if(remove(filename.str().c_str()) != 0)
				    perror("Error deleting save from path");
				
				delete(requestedGameState);
				requestedGameState = new GameStateTitle();
			}
			else {
			    saveGame();
			}
		}

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
}

void GameStatePlay::checkEquipmentChange() {
	if (menu->inv->changed_equipment) {
	
		pc->loadGraphics(menu->items->items[menu->inv->inventory[EQUIPMENT][0].item].gfx, 
		                 menu->items->items[menu->inv->inventory[EQUIPMENT][1].item].gfx, 
		                 menu->items->items[menu->inv->inventory[EQUIPMENT][2].item].gfx);
						 
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
		if (menu->items->items[powers->used_item].type == ITEM_TYPE_CONSUMABLE) {
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
	if (npc_click != -1 && interact_distance < max_interact_distance && pc->stats.alive) {
		inpt->lock[MAIN1] = true;
		
		if (npcs->npcs[npc_id]->vendor) {
			menu->vendor->npc = npcs->npcs[npc_id];
			menu->vendor->setInventory();
			menu->closeAll(false);
			menu->vendor->visible = true;
			menu->inv->visible = true;
			
			if (!npcs->npcs[npc_id]->playSound(NPC_VOX_INTRO))
				Mix_PlayChannel(-1, menu->sfx_open, 0);
		}
		else if(npcs->npcs[npc_id]->talker) {
			menu->talker->npc = npcs->npcs[npc_id];
			menu->talker->chooseDialogNode();
			menu->closeAll(false);
			menu->talker->visible = true;
			
			npcs->npcs[npc_id]->playSound(NPC_VOX_INTRO);           
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
		map->checkEventClick();
		
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
	checkCancel();

	map->logic();
	quests->logic();
	
}


/**
 * Render all graphics for a single frame
 */
void GameStatePlay::render() {

	// Create a list of Renderables from all objects not already on the map.
	renderableCount = 0;

	r[renderableCount++] = pc->getRender(); // Avatar
	
	for (int i=0; i<enemies->enemy_count; i++) { // Enemies
		r[renderableCount++] = enemies->getRender(i);
		if (enemies->enemies[i]->stats.shield_hp > 0) {
			r[renderableCount] = enemies->enemies[i]->stats.getEffectRender(STAT_EFFECT_SHIELD);
			r[renderableCount++].sprite = powers->gfx[powers->powers[POWER_SHIELD].gfx_index]; // TODO: parameter
		}
	}

	for (int i=0; i<npcs->npc_count; i++) { // NPCs
		r[renderableCount++] = npcs->npcs[i]->getRender();
	}
	
	for (int i=0; i<loot->loot_count; i++) { // Loot
		r[renderableCount++] = loot->getRender(i);
	}
	
	for (int i=0; i<hazards->hazard_count; i++) { // Hazards
		if (hazards->h[i]->rendered && hazards->h[i]->delay_frames == 0) {
			r[renderableCount++] = hazards->getRender(i);
		}
	}
	
	// get additional hero overlays
	if (pc->stats.shield_hp > 0) {
		r[renderableCount] = pc->stats.getEffectRender(STAT_EFFECT_SHIELD);
		r[renderableCount++].sprite = powers->gfx[powers->powers[POWER_SHIELD].gfx_index]; // TODO: parameter
	}
	if (pc->stats.vengeance_stacks > 0) {
		r[renderableCount] = pc->stats.getEffectRender(STAT_EFFECT_VENGEANCE);
		r[renderableCount++].sprite = powers->runes;		
	}
		
	sort_by_tile(r,renderableCount);

	// render the static map layers plus the renderables
	map->render(r, renderableCount);
	
	// display the name of the map in the upper-right hand corner
	label_mapname->set(VIEW_W-2, 2, JUSTIFY_RIGHT, VALIGN_TOP, map->title, FONT_WHITE);
	label_mapname->render();
	
	// mouseover tooltips
	loot->renderTooltips(map->cam);
	npcs->renderTooltips(map->cam, inpt->mouse);
	
	menu->hudlog->render();
	menu->mini->renderIso(&map->collider, pc->stats.pos, map->w, map->h);
	menu->render();

    // render combat text last - this should make it obvious you're being
    // attacked, even if you have menus open
    CombatText *combat_text = CombatText::Instance();
    combat_text->setCam(map->cam);
    combat_text->render();
}

void GameStatePlay::showFPS(int fps) {
	stringstream ss;
	ss << fps << "fps";
	label_fps->set(VIEW_W >> 1, 2, JUSTIFY_CENTER, VALIGN_TOP, ss.str(), FONT_GREY);
	label_fps->render();
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
}

