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

	currentState = new GameStateTitle(screen, inp, font);
	
	game_state = GAME_STATE_TITLE;
	
	done = false;
}

void GameSwitcher::logic() {

	/*
	*  Check if a the game state is to be changed and change it if necessary, deleting the old state
	*/
	if (currentState->getRequestedGameState() != NULL) {
		GameState* newState = currentState->getRequestedGameState();

		delete currentState;
	
		currentState = newState;
	}

	currentState->logic();

	// Check if the GameState wants to quit the application
	done = currentState->isExitRequested();
}

void GameSwitcher::render() {
	/*switch (game_state) {
		
		// title screen
		case GAME_STATE_TITLE:
		
			title->render();
			break;
			
		// main gameplay
		case GAME_STATE_PLAY:
		
			eng->render();
			break;
	
		// load game
		case GAME_STATE_LOAD:
		
			slots->render();
			break;
			
		// new game
		case GAME_STATE_NEW:
		
			break;
	
		default:
			break;
	}
	*/

	currentState->render();
}

GameSwitcher::~GameSwitcher() {
	delete font;
}
