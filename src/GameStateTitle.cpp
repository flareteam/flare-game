#include "GameStateLoad.h"
#include "GameStateTitle.h"

GameStateTitle::GameStateTitle(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, MessageEngine *_msg) : GameState(_screen, _inp, _font, _msg) {

	exit_game = false;
	load_game = false;
	
	loadGraphics();
	
	// set up buttons
	button_play = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	button_exit = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	
	button_play->label = msg->get("play_button");
	button_play->pos.x = VIEW_W_HALF - button_play->pos.w/2;
	button_play->pos.y = VIEW_H - (button_exit->pos.h*2);

	button_exit->label = msg->get("exit_button");
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	
	// set up labels
	label_version = new WidgetLabel(screen, font);
	label_version->set(VIEW_W, 0, JUSTIFY_RIGHT, VALIGN_TOP, msg->get("version_number"), FONT_WHITE);
}

void GameStateTitle::loadGraphics() {

	logo = IMG_Load((PATH_DATA + "images/menus/logo.png").c_str());

	if(!logo) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = logo;
	logo = SDL_DisplayFormatAlpha(logo);
	SDL_FreeSurface(cleanup);	
}

void GameStateTitle::logic() {

	if (button_play->checkClick()) {
		requestedGameState = new GameStateLoad(screen, inp, font, msg);
	}
	
	if (button_exit->checkClick()) {
		exitRequested = true;
	}
}

void GameStateTitle::render() {

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
	
	// version number
	label_version->render();
}

GameStateTitle::~GameStateTitle() {
	delete button_play;
	delete button_exit;
	delete label_version;
	SDL_FreeSurface(logo);
}
