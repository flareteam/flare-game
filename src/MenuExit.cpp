#include "MenuExit.h"

MenuExit::MenuExit(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) : Menu(_screen, inp = _inp, _font) {
	exitClicked = false;

	window_area.w = 192;
	window_area.h = 64;
	window_area.x = (VIEW_W/2) - (window_area.w/2);
	window_area.y = (VIEW_H - window_area.h)/2;
	
	buttonExit = new WidgetButton(screen, font, inp);
	buttonExit->label = "Exit";
	buttonExit->pos.x = VIEW_W_HALF - buttonExit->pos.w/2;
	buttonExit->pos.y = VIEW_H/2;

	loadGraphics();
}

void MenuExit::loadGraphics() {
	background = IMG_Load("images/menus/confirm_bg.png");
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
}

void MenuExit::logic() {
	if (visible) {
		if (buttonExit->checkClick()) {
			exitClicked = true;	
		}
	}
}

void MenuExit::render() {
	SDL_Rect src;

	// background
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &window_area);

	font->render("Save and exit to title?", window_area.x + window_area.w/2, window_area.y + 10, JUSTIFY_CENTER, screen, FONT_WHITE);

	buttonExit->render();
}

MenuExit::~MenuExit() {
}

