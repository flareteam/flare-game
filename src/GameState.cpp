#include "GameState.h"

GameState::GameState(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) {
	screen = _screen;
	inp = _inp;
	font = _font;

	requestedGameState = NULL;

	exitRequested = false;
	hasMusic = false;
}

GameState* GameState::getRequestedGameState() {
	return requestedGameState;
}

void GameState::logic() {
}

void GameState::render() {
}

