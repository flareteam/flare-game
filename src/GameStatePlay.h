/**
 * class GameStatePlay
 *
 * Handles logic and rendering of the main action game play
 * Also handles message passing between child objects, often to avoid circular dependencies.
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef GAMESTATEPLAY_H
#define GAMESTATEPLAY_H

#include "SDL.h"
#include "SDL_image.h"
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
#include "MessageEngine.h"

class GameStatePlay : public GameState {
private:
	SDL_Surface *screen;
	
	InputState *inp;
	MapIso *map;
	Enemy *enemy;
	Renderable r[1024];
	int renderableCount;
	HazardManager *hazards;
	EnemyManager *enemies;
	FontEngine *font;
	MenuManager *menu;
	LootManager *loot;
	PowerManager *powers;
	NPCManager *npcs;
	CampaignManager *camp;
	QuestLog *quests;
	MessageEngine *msg;
	
	bool restrictPowerUse();
	void checkEnemyFocus();
	void checkLoot();
	void checkLootDrop();
	void checkTeleport();
	void checkCancel();
	void checkLog();
	void checkEquipmentChange();
	void checkConsumable();
	void checkNPCInteraction();

	int npc_id;
	
public:
	GameStatePlay(SDL_Surface *screen, InputState *inp, FontEngine *font, MessageEngine *msg);
	~GameStatePlay();

	void logic();
	void render();
	void showFPS(int fps);
	void saveGame();
	void loadGame();
	void resetGame();

	Avatar *pc;
	int game_slot;

};

#endif

