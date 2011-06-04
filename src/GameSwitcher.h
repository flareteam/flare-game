/**
 * class GameSwitcher
 *
 * State machine handler between main game modes that take up the entire view/control
 *
 * Examples:
 * - the main gameplay (GameEngine class)
 * - title screen
 * - new game screen (character create)
 * - load game screen
 * - maybe full-video cutscenes
 *
 * @author Clint Bellanger
 * @license GPL
 *
 */
 
#ifndef GAME_SWITCHER_H
#define GAME_SWITCHER_H

#include "SDL.h"
#include "SDL_image.h"
#include "InputState.h"
#include "GameEngine.h"
#include "FontEngine.h"
#include "MenuTitle.h"
#include "MenuGameSlots.h"

const int GAME_STATE_TITLE = 0;
const int GAME_STATE_PLAY = 1;
const int GAME_STATE_LOAD = 2;
const int GAME_STATE_NEW = 3;

class GameSwitcher {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;
	
	GameEngine *eng; // for GAME_STATE_PLAY
	MenuTitle *title; // for GAME_STATE_TITLE
	MenuGameSlots *slots; // for GAME_STATE_LOAD
	
public:
	GameSwitcher(SDL_Surface *_screen, InputState *_inp);
	void logic();
	void render();
	~GameSwitcher();
	
	int game_state;
	bool done;
};

#endif
