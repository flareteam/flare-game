/**
 * class MenuTitle
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_TITLE_H
#define MENU_TITLE_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "InputState.h"
#include "FontEngine.h"
#include "WidgetButton.h"

class MenuTitle {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;

	SDL_Surface *logo;
	WidgetButton *button_play;
	WidgetButton *button_exit;
	
public:
	MenuTitle(SDL_Surface *_screen, InputState *_inp, FontEngine *_font);
	~MenuTitle();
	void loadGraphics();
	void logic();
	void render();
	
	// switch
	bool exit_game;
	bool load_game;
	
};

#endif

