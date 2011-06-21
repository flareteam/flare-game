/**
 * class GameStatePlay
 *
 * Handles logic and rendering of the main action game play
 * Also handles message passing between child objects, often to avoid circular dependencies.
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "GameStatePlay.h"
#include "GameState.h"
#include "GameStateTitle.h"

GameStatePlay::GameStatePlay(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) : GameState(screen, inp, font) {

	hasMusic = true;
	//Mix_HaltMusic(); // maybe not needed? playing new music should auto halt previous music

	// shared resources from GameSwitcher
	screen = _screen;
	inp = _inp;
	
	// GameEngine scope variables
	npc_id = -1;
	game_slot = 0;

	// construct gameplay objects
	powers = new PowerManager();
	font = _font;
	camp = new CampaignManager();
	map = new MapIso(_screen, camp);
	pc = new Avatar(powers, _inp, map);
	enemies = new EnemyManager(powers, map);
	hazards = new HazardManager(powers, pc, enemies);
	menu = new MenuManager(powers, _screen, _inp, font, &pc->stats, camp);
	loot = new LootManager(menu->items, menu->tip, enemies, map);
	npcs = new NPCManager(map, menu->tip, loot, menu->items);
	quests = new QuestLog(camp, menu->log);

	// assign some object pointers after object creation, based on dependency order
	camp->items = menu->items;
	camp->carried_items = &menu->inv->inventory[CARRIED];
	camp->currency = &menu->inv->gold;
	camp->xp = &pc->stats.xp;

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
}

/**
 * Check mouseover for enemies.
 * class variable "enemy" contains a live enemy on mouseover.
 * This function also sets enemy mouseover for Menu Enemy.
 */
void GameStatePlay::checkEnemyFocus() {
	// determine enemies mouseover
	// only check alive enemies for targeting
	enemy = enemies->enemyFocus(inp->mouse, map->cam, true);
	
	if (enemy != NULL) {
	
		// if there's a living creature in focus, display its stats
		menu->enemy->enemy = enemy;
		menu->enemy->timeout = MENU_ENEMY_TIMEOUT;
	}
	else {
		
		// if there's no living creature in focus, look for a dead one instead
		enemy = enemies->enemyFocus(inp->mouse, map->cam, false);
		if (enemy != NULL) {
			menu->enemy->enemy = enemy;
			menu->enemy->timeout = MENU_ENEMY_TIMEOUT;
		}
	}
	enemy = NULL;
}

/**
 * If mouse_move is enabled, and the mouse is over a live enemy,
 * Do not allow power use with button MAIN1
 */
bool GameStatePlay::restrictPowerUse() {
	if(MOUSE_MOVE) {
		if(enemy == NULL && inp->pressing[MAIN1] && !inp->pressing[SHIFT] && !(isWithin(menu->act->numberArea,inp->mouse) || isWithin(menu->act->mouseArea,inp->mouse) || isWithin(menu->act->menuArea, inp->mouse))) {
			return true;
		}
	}
	return false;
}

/**
 * Check to see if the player is picking up loot on the ground
 */
void GameStatePlay::checkLoot() {
	if (inp->pressing[MAIN1] && !inp->lock[MAIN1] && pc->stats.alive) {

		ItemStack pickup;
		int gold;
		
		pickup = loot->checkPickup(inp->mouse, map->cam, pc->stats.pos, gold, menu->inv->full());
		if (pickup.item > 0) {
			inp->lock[MAIN1] = true;
			menu->inv->add(pickup);

			camp->setStatus(menu->items->items[pickup.item].pickup_status);
		}
		else if (gold > 0) {
			inp->lock[MAIN1] = true;
			menu->inv->addGold(gold);
		}
		if (loot->full_msg) {
			inp->lock[MAIN1] = true;
			menu->log->add("Inventory is full.", LOG_TYPE_MESSAGES);
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
			
			// auto-save
			saveGame();
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
		requestedGameState = new GameStateTitle(screen, inp, font);
	}

	// if user closes the window
	if (inp->done) {
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
}

void GameStatePlay::checkEquipmentChange() {
	if (menu->inv->changed_equipment) {
		pc->loadGraphics(menu->items->items[menu->inv->inventory[EQUIPMENT][0].item].gfx, 
		                 menu->items->items[menu->inv->inventory[EQUIPMENT][1].item].gfx, 
		                 menu->items->items[menu->inv->inventory[EQUIPMENT][2].item].gfx);
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
 * If the player has clicked on an NPC, the game mode might be changed.
 * If a player walks away from an NPC, end the interaction with that NPC
 * If an NPC is giving a reward, process it
 */
void GameStatePlay::checkNPCInteraction() {

	int npc_click = -1;
	int max_interact_distance = UNITS_PER_TILE * 4;
	int interact_distance = max_interact_distance+1;
	
	// check for clicking on an NPC
	if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) {
		npc_click = npcs->checkNPCClick(inp->mouse, map->cam);
		if (npc_click != -1) npc_id = npc_click;
	}
	
	// check distance to this npc
	if (npc_id != -1) {
		interact_distance = (int)calcDist(pc->stats.pos, npcs->npcs[npc_id]->pos);
	}
	
	// if close enough to the NPC, open the appropriate interaction screen
	if (npc_click != -1 && interact_distance < max_interact_distance && pc->stats.alive) {
		inp->lock[MAIN1] = true;
		
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
		
		pc->logic(menu->act->checkAction(inp->mouse), restrictPowerUse());
		
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
	font->render(map->title, VIEW_W-2, 2, JUSTIFY_RIGHT, screen, FONT_WHITE);
	
	// mouseover tooltips
	loot->renderTooltips(map->cam);
	npcs->renderTooltips(map->cam, inp->mouse);
	
	menu->hudlog->render();
	menu->mini->render(&map->collider, pc->stats.pos, map->w, map->h);
	menu->render();

}

void GameStatePlay::showFPS(int fps) {
	stringstream ss;
	ss << fps << "fps";
	font->render(ss.str(), VIEW_W >> 1, 2, JUSTIFY_CENTER, screen, FONT_GRAY); 
}

GameStatePlay::~GameStatePlay() {
	delete quests;
	delete camp;
	delete npcs;
	delete hazards;
	delete enemies;
	delete pc;
	delete map;
	delete menu;
	delete loot;
	delete powers;
}

