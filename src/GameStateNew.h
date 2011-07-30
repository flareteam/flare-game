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
#include "WidgetInput.h"
#include "FileParser.h"
#include "Settings.h"
#include "GameState.h"
#include "UtilsParsing.h"
#include "MessageEngine.h"

const int PLAYER_OPTION_MAX = 32;

class GameStateNew : public GameState {
private:

	void loadGraphics();
	void loadPortrait(string portrait_filename);
	void loadOptions(string option_filename);

	string base[PLAYER_OPTION_MAX];
	string head[PLAYER_OPTION_MAX];
	string portrait[PLAYER_OPTION_MAX];
	int option_count;
	int current_option;

	SDL_Surface *portrait_image;
	SDL_Surface *portrait_border;
	WidgetButton *button_exit;
	WidgetButton *button_create;
	WidgetButton *button_next;
	WidgetButton *button_prev;
	WidgetInput *input_name;

public:
	GameStateNew(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, MessageEngine *_msg);
	~GameStateNew();
	void logic();
	void render();
	int game_slot;
	
};

#endif
