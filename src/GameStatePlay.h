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

#ifndef GAMESTATEPLAY_H
#define GAMESTATEPLAY_H

#include "InputState.h"
#include "Avatar.h"
#include "Enemy.h"
#include "MapIso.h"
#include "Utils.h"
#include "HazardManager.h"
#include "EnemyManager.h"
#include "FontEngine.h"
#include "MenuManager.h"
#include "LootManager.h"
#include "PowerManager.h"
#include "NPCManager.h"
#include "CampaignManager.h"
#include "QuestLog.h"
#include "GameState.h"
#include "WidgetLabel.h"
#include "CombatText.h"

#include <SDL.h>
#include <SDL_image.h>

class GameStatePlay : public GameState {
private:

	MapIso *map;
	Enemy *enemy;
	Renderable r[1024];
	int renderableCount;
	HazardManager *hazards;
	EnemyManager *enemies;
	MenuManager *menu;
	LootManager *loot;
	PowerManager *powers;
	ItemManager *items;
	NPCManager *npcs;
	CampaignManager *camp;
	QuestLog *quests;

	WidgetLabel *label_mapname;
	WidgetLabel *label_fps;
	
	bool restrictPowerUse();
	void checkEnemyFocus();
	void checkLoot();
	void checkLootDrop();
	void checkTeleport();
	void checkCancel();
	void checkLog();
	void checkEquipmentChange();
	void checkConsumable();
	void checkNotifications();
	void checkNPCInteraction();

	int npc_id;
	
public:
	GameStatePlay();
	~GameStatePlay();

	void logic();
	void render();
	void showFPS(int fps);
	void saveGame();
	void loadGame();
	void resetGame();

	Avatar *pc;
    Avatar *partypl;
	int game_slot;
};

#endif

