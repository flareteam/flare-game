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

GameSwitcher::GameSwitcher(SDL_Surface *_screen, InputState *_inp) {
	inp = _inp;
	screen = _screen;
		
	font = new FontEngine();	
	eng = new GameEngine(screen, inp, font);
	title = new MenuTitle(screen, inp, font);
	slots = new MenuGameSlots(screen, inp, font);
	
	game_state = GAME_STATE_TITLE;
	eng->game_slot = 0;
	
	done = false;
}

void GameSwitcher::logic() {
	switch (game_state) {
		
		// title screen
		case GAME_STATE_TITLE:
		
			title->logic();
			done = title->exit_game;
			
			if (title->load_game) {
				title->load_game = false;
				game_state = GAME_STATE_LOAD;				
				
			}
			break;
			
		// new game
		case GAME_STATE_NEW:
		
			break;
				
		// load game
		case GAME_STATE_LOAD:
		
			slots->logic();
			if (slots->exit_slots) {
				slots->exit_slots = false;
				game_state = GAME_STATE_TITLE;
			}
			else if (slots->load_game) {
				slots->load_game = false;
				game_state = GAME_STATE_PLAY;
				eng->resetGame();
				eng->game_slot = slots->selected_slot+1;
				eng->loadGame();
				eng->logic(); // run one frame of logic to set up the render
			}
			else if (slots->new_game) {
				slots->new_game = false;
				game_state = GAME_STATE_PLAY;
				eng->resetGame();
				eng->game_slot = slots->selected_slot+1;
				eng->loadGame();
				eng->logic(); // run one frame of logic to set up the render
			}
			
			break;
	
		// main gameplay
		case GAME_STATE_PLAY:
		
			eng->logic();
			
			if (eng->done) {
				eng->done = false;
				game_state = GAME_STATE_TITLE;
				slots->readGameSlots();
			}
			
			break;
			
		default:
			break;
	}
	
}

void GameSwitcher::render() {
	switch (game_state) {
		
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

}

GameSwitcher::~GameSwitcher() {
	
	delete font;
	delete title;
	delete eng;
	delete slots;
}
