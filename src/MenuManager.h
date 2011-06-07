/**
 * class MenuManager
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "FontEngine.h"
#include "InputState.h"
#include "MenuInventory.h"
#include "MenuPowers.h"
#include "MenuCharacter.h"
#include "MenuLog.h"
#include "MenuHUDLog.h"
#include "StatBlock.h"
#include "MenuActionBar.h"
#include "MenuHPMP.h"
#include "MenuTooltip.h"
#include "ItemDatabase.h"
#include "PowerManager.h"
#include "MenuMiniMap.h"
#include "MenuExperience.h"
#include "MenuEnemy.h"
#include "MenuVendor.h"
#include "MenuTalker.h"
#include "MenuExit.h"
#include "CampaignManager.h"

const int DRAG_SRC_POWERS = 1;
const int DRAG_SRC_INVENTORY = 2;
const int DRAG_SRC_ACTIONBAR = 3;
const int DRAG_SRC_VENDOR = 4;

class MenuManager {
private:
	
	SDL_Surface *icons;

	PowerManager *powers;
	StatBlock *stats;
	InputState *inp;
	FontEngine *font;
	SDL_Surface *screen;
	CampaignManager *camp;

	bool key_lock;
	void loadSounds();
	void loadIcons();
	
	bool dragging;
	ItemStack drag_stack;
	int drag_power;
	int drag_src;

	bool done;
	
public:
	MenuManager(PowerManager *powers, SDL_Surface *screen, InputState *inp, FontEngine *font, StatBlock *stats, CampaignManager *camp);
	~MenuManager();
	void logic();
	void render();
	void renderIcon(int icon_id, int x, int y);
	void closeAll(bool play_sound);
	void closeLeft(bool play_sound);
	void closeRight(bool play_sound);

	MenuInventory *inv;
	MenuPowers *pow;
	MenuCharacter *chr;
	MenuLog *log;
	MenuHUDLog *hudlog;
	MenuActionBar *act;
	MenuHPMP *hpmp;
	MenuTooltip *tip;
	MenuMiniMap *mini;
	MenuExperience *xp;
	MenuEnemy *enemy;
	MenuVendor *vendor;
	MenuTalker *talker;
	MenuExit *exit;
	ItemDatabase *items;
	
	bool pause;
	bool menus_open;
	ItemStack drop_stack;	

	Mix_Chunk *sfx_open;
	Mix_Chunk *sfx_close;
	
	bool requestingExit() { return done; }
};

#endif
