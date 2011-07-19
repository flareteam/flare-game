#include "MenuConfirm.h"

MenuConfirm::MenuConfirm(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, string _buttonMsg, string _boxMsg) : Menu(_screen, inp = _inp, _font) {
	confirmClicked = false;

	// Text to display in confirmation box
	boxMsg = _boxMsg;

	window_area.w = 192;
	window_area.h = 64;
	window_area.x = (VIEW_W/2) - (window_area.w/2);
	window_area.y = (VIEW_H - window_area.h)/2;
	
	buttonConfirm = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	buttonConfirm->label = _buttonMsg;
	buttonConfirm->pos.x = VIEW_W_HALF - buttonConfirm->pos.w/2;
	buttonConfirm->pos.y = VIEW_H/2;
	
	buttonClose = new WidgetButton(screen, font, inp,
				       "images/menus/buttons/button_x.png");
	buttonClose->pos.x = window_area.x + window_area.w;
	buttonClose->pos.y = window_area.y;

	loadGraphics();
}

void MenuConfirm::loadGraphics() {
	background = IMG_Load((PATH_DATA + "images/menus/confirm_bg.png").c_str());
	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
}

void MenuConfirm::logic() {
	if (visible) {
	  if(buttonConfirm->checkClick())
	    confirmClicked = true;
	  if(buttonClose->checkClick())
	    visible = false;
	}
}

void MenuConfirm::render() {
	SDL_Rect src;

	// background
	src.x = 0;
	src.y = 0;
	src.w = window_area.w;
	src.h = window_area.h;
	SDL_BlitSurface(background, &src, screen, &window_area);

	font->render(boxMsg, window_area.x + window_area.w/2, window_area.y + 10, JUSTIFY_CENTER, screen, FONT_WHITE);

	buttonConfirm->render();
	buttonClose->render();
}

MenuConfirm::~MenuConfirm() {
	delete buttonConfirm;
	delete buttonClose;
	SDL_FreeSurface(background);
}

