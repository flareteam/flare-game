/*
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
 
#include "GameSwitcher.h"
#include "GameStateTitle.h"
#include "GameStateLoad.h"

GameSwitcher::GameSwitcher(SDL_Surface *_screen, InputState *_inp) {
	inp = _inp;
	screen = _screen;
		
	font = new FontEngine();	

	// The initial state is the title screen
	currentState = new GameStateTitle(screen, inp, font);
	
	done = false;
	music = NULL;
	//loadMusic();
	
}

void GameSwitcher::loadMusic() {

	music = Mix_LoadMUS("./music/title_theme.ogg");
	if (!music) {
	  printf("Mix_LoadMUS: %s\n", Mix_GetError());
	  SDL_Quit();
	}

	Mix_VolumeMusic(MUSIC_VOLUME);
	Mix_PlayMusic(music, -1);
	
}

void GameSwitcher::logic() {

	/*
	*  Check if a the game state is to be changed and change it if necessary, deleting the old state
	*/
	if (currentState->getRequestedGameState() != NULL) {
		GameState* newState = currentState->getRequestedGameState();

		delete currentState;
	
		currentState = newState;
		
		// if this game state does not provide music, use the title theme
		//if (!currentState->hasMusic) {
		//	if (!Mix_PlayingMusic()) {
		//		Mix_PlayMusic(music, -1);
		//	}
		//}
		
	}

	currentState->logic();

	// Check if the GameState wants to quit the application
	done = currentState->isExitRequested();
}

void GameSwitcher::render() {
	currentState->render();
}

GameSwitcher::~GameSwitcher() {
	delete font;
	delete currentState;
	Mix_FreeMusic(music);
}

