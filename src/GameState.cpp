#include "GameState.h"

GameState::GameState(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, MessageEngine *_msg) {
	screen = _screen;
	inp = _inp;
	font = _font;
	msg = _msg;

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

