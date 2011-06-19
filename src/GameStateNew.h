/**
 * GameStateNew
 * 
 * Handle player choices when starting a new game
 * (e.g. character appearance)
 * 
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef GAMESTATENEW_H
#define GAMESTATENEW_H

#include <string>
#include <sstream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "FontEngine.h"
#include "WidgetButton.h"
#include "FileParser.h"
#include "Settings.h"
#include "GameState.h"
#include "UtilsParsing.h"

const int BASE_AND_LOOK_MAX = 32;
const int NAME_LENGTH_MAX = 20;

class GameStateNew : public GameState {
private:

	void loadGraphics();
	void loadPortrait(string base, string look);
	void loadOptions(string option_filename);

	string base[BASE_AND_LOOK_MAX];
	string look[BASE_AND_LOOK_MAX];
	int option_count;
	int current_option;
	string character_name;

	SDL_Surface *portrait;
	SDL_Surface *portrait_border;
	WidgetButton *button_exit;
	WidgetButton *button_create;
	WidgetButton *button_next;
	WidgetButton *button_prev;

public:
	GameStateNew(SDL_Surface *_screen, InputState *_inp, FontEngine *_font);
	~GameStateNew();
	void logic();
	void render();
	int game_slot;
	
};

#endif
