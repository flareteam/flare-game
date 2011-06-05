#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "FontEngine.h"

class GameState {
public:
	GameState(SDL_Surface *_screen, InputState *_inp, FontEngine *_font);

	virtual void logic();
	virtual void render();

	GameState* getRequestedGameState();
	bool isExitRequested() { return exitRequested; };

protected:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;

	GameState* requestedGameState;	

	bool exitRequested;
};

#endif
