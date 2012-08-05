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

#include "GameState.h"

#include <SDL.h>
#include <SDL_image.h>


class Avatar;
class CampaignManager;
class Enemy;
class EnemyManager;
class HazardManager;
class ItemManager;
class LootManager;
class MapRenderer;
class MenuManager;
class NPCManager;
class PowerManager;
class QuestLog;
class WidgetLabel;

class GameStatePlay : public GameState {
private:

	MapRenderer *map;
	Enemy *enemy;
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
	void checkStash();

	int npc_id;

	SDL_Color color_normal;

public:
	GameStatePlay();
	~GameStatePlay();

	void logic();
	void render();
	void showFPS(int fps);
	void saveGame();
	void loadGame();
	void loadStash();
	void resetGame();

	Avatar *pc;
	int game_slot;
};

#endif

