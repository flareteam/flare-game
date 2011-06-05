#ifndef GAMESTATETITLE_H
#define GAMESTATETITLE_H

#include "GameState.h"
#include "WidgetButton.h"

class GameStateTitle : public GameState {
private:
	SDL_Surface *logo;
	WidgetButton *button_play;
	WidgetButton *button_exit;
	
public:
	GameStateTitle(SDL_Surface *_screen, InputState *_inp, FontEngine *_font);
	~GameStateTitle();
	void loadGraphics();
	void logic();
	void render();
	
	// switch
	bool exit_game;
	bool load_game;
	
};

#endif

