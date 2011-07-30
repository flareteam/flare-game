/**
 * GameStateLoad
 * 
 * Display the current save-game slots
 * Allow the player to continue a previous game
 * Allow the player to start a new game
 * Allow the player to abandon a previous game
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef GAMESTATELOAD_H
#define GAMESTATELOAD_H

#include <string>
#include <sstream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Settings.h"
#include "InputState.h"
#include "FontEngine.h"
#include "WidgetButton.h"
#include "FileParser.h"
#include "Settings.h"
#include "StatBlock.h"
#include "ItemDatabase.h"
#include "GameState.h"
#include "MenuConfirm.h"
#include "MessageEngine.h"

const int GAME_SLOT_MAX = 4;

class GameStateLoad : public GameState {
private:

	void loadGraphics();
	void loadPortrait(int slot);
	string getMapName(string map_filename);

	ItemDatabase *items;
	WidgetButton *button_exit;
	WidgetButton *button_action;
	WidgetButton *button_alternate;

	MenuConfirm *confirm;

	SDL_Surface *background;
	SDL_Surface *selection;
	SDL_Surface *portrait_border;
	SDL_Surface *portrait;
	SDL_Surface *sprites[GAME_SLOT_MAX];
	StatBlock stats[GAME_SLOT_MAX];
	int equipped[GAME_SLOT_MAX][3];	
	SDL_Rect slot_pos[GAME_SLOT_MAX];
	string current_map[GAME_SLOT_MAX];

	bool loading_requested;
	bool loading;
	bool loaded;
	
	Point name_pos;
	Point level_pos;
	Point map_pos;
	Point sprites_pos;
	
	// animation info
	int current_frame;
	int frame_ticker;
	
public:
	GameStateLoad(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, MessageEngine *_msg);
	~GameStateLoad();

	void logic();
	void logicLoading();
	void render();	
	void readGameSlot(int slot);
	void readGameSlots();
	void loadPreview(int slot);
	
	bool load_game;
	int selected_slot;
};

#endif
