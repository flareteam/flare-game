/**
 * class MenuTitle
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuTitle.h"

MenuTitle::MenuTitle(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) {
	screen = _screen;
	inp = _inp;
	font = _font;

	exit_game = false;
	load_game = false;
	
	loadGraphics();
	
	// set up buttons
	button_play = new WidgetButton(screen, font, inp);
	button_exit = new WidgetButton(screen, font, inp);
	
	button_play->label = "Play Game";
	button_play->pos.x = VIEW_W_HALF - button_play->pos.w/2;
	button_play->pos.y = VIEW_H - 64;

	button_exit->label = "Exit Game";
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - 32;
	
}

void MenuTitle::loadGraphics() {

	logo = IMG_Load("images/menus/logo.png");

	if(!logo) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = logo;
	logo = SDL_DisplayFormatAlpha(logo);
	SDL_FreeSurface(cleanup);	
}

void MenuTitle::logic() {

	if (button_play->checkClick()) {
		load_game = true;
	}
	
	if (button_exit->checkClick()) {
		exit_game = true;
	}
}

void MenuTitle::render() {

	SDL_Rect src;
	SDL_Rect dest;
	
	// display logo centered
	src.x = src.y = 0;
	src.w = dest.w = logo->w;
	src.h = dest.h = logo->h;
	dest.x = VIEW_W_HALF - (logo->w/2);
	dest.y = VIEW_H_HALF - (logo->h/2);
	SDL_BlitSurface(logo, &src, screen, &dest);

	// display buttons
	button_play->render();
	button_exit->render();
	
}

MenuTitle::~MenuTitle() {
	delete button_play;
	delete button_exit;
	SDL_FreeSurface(logo);
}
