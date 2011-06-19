/**
 * GameStateNew
 * 
 * Handle player choices when starting a new game
 * (e.g. character appearance)
 * 
 * @author Clint Bellanger
 * @license GPL
 */

#include "GameStateNew.h"
#include "GameStateLoad.h"
#include "GameStatePlay.h"

GameStateNew::GameStateNew(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) : GameState(_screen, _inp, _font) {
	game_slot = 0;
	option_count = 0;
	current_option = 0;
	portrait = NULL;
	character_name = "";
	
	button_exit = new WidgetButton(screen, font, inp, "./images/menus/buttons/button_default.png");
	button_exit->label = "Cancel";
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;
	
	button_create = new WidgetButton(screen, font, inp, "./images/menus/buttons/button_default.png");
	button_create->label = "Create Character";
	button_create->pos.x = VIEW_W_HALF + button_create->pos.w/2;
	button_create->pos.y = VIEW_H - button_create->pos.h;

	button_prev = new WidgetButton(screen, font, inp, "./images/menus/buttons/left.png");
	button_prev->pos.x = VIEW_W_HALF - 160 - button_prev->pos.w;
	button_prev->pos.y = VIEW_H_HALF - button_prev->pos.h;
	
	button_next = new WidgetButton(screen, font, inp, "./images/menus/buttons/right.png");
	button_next->pos.x = VIEW_W_HALF + 160;
	button_next->pos.y = VIEW_H_HALF - button_next->pos.h;
	
	loadGraphics();
	loadOptions("./config/base_and_look.txt");
	loadPortrait(base[0], look[0]);
}	

void GameStateNew::loadGraphics() {
	portrait_border = NULL;
	
	portrait_border = IMG_Load("images/menus/portrait_border.png");
	if(!portrait_border) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	SDL_SetColorKey( portrait_border, SDL_SRCCOLORKEY, SDL_MapRGB(portrait_border->format, 255, 0, 255) ); 
	
	// optimize
	SDL_Surface *cleanup = portrait_border;
	portrait_border = SDL_DisplayFormatAlpha(portrait_border);
	SDL_FreeSurface(cleanup);
}

void GameStateNew::loadPortrait(string base, string look) {
	SDL_FreeSurface(portrait);
	portrait = NULL;
	
	portrait = IMG_Load(("images/portraits/" + base + "_" + look + ".png").c_str());
	if (!portrait) return;
	
	// optimize
	SDL_Surface *cleanup = portrait;
	portrait = SDL_DisplayFormatAlpha(portrait);
	SDL_FreeSurface(cleanup);
}

/**
 * Load body type "base" and portrait/head "look" options from a config file
 *
 * @param filename File containing entries for option=base,look
 */
void GameStateNew::loadOptions(string filename) {
	FileParser fin;
	if (!fin.open(filename)) return;
	
	while (fin.next()) {
	
		// if at the max allowed base+look options, skip the rest of the file
		// TODO: remove static array size limit
		if (option_count == BASE_AND_LOOK_MAX-1) break;

		if (fin.key == "option") {
			base[option_count] = eatFirstString(fin.val, ',');
			look[option_count] = fin.val;
			option_count++;
		}
	}
	fin.close();
}

void GameStateNew::logic() {

	// require character name
	if (character_name == "") {
		button_create->enabled = false;
	}
	else {
		button_create->enabled = true;
	}

	if (button_exit->checkClick()) {
		requestedGameState = new GameStateLoad(screen, inp, font);
	}
	
	if (button_create->checkClick()) {
		// start the new game
		GameStatePlay* play = new GameStatePlay(screen, inp, font);
		play->pc->stats.base = base[current_option];
		play->pc->stats.look = look[current_option];
		play->pc->stats.name = character_name;
		play->game_slot = game_slot;
		play->resetGame();
		requestedGameState = play;
	}
	
	// scroll through portrait options	
	if (button_next->checkClick()) {
		current_option++;
		if (current_option == option_count) current_option = 0;
		loadPortrait(base[current_option], look[current_option]);
	}
	else if (button_prev->checkClick()) {
		current_option--;
		if (current_option == -1) current_option = option_count-1;
		loadPortrait(base[current_option], look[current_option]);
	}
	
	// type the character's name
	character_name += inp->inkeys;
	if (character_name.length() > NAME_LENGTH_MAX) {
		character_name = character_name.substr(0, NAME_LENGTH_MAX);
	}
		
	// handle backspaces
	if (!inp->lock[DELETE] && inp->pressing[DELETE]) {
		inp->lock[DELETE] = true;
		character_name = character_name.substr(0, character_name.length()-1);
	}

}

void GameStateNew::render() {
	// display buttons
	button_exit->render();
	button_create->render();
	button_prev->render();
	button_next->render();
	
	// display portrait option
	SDL_Rect src;
	SDL_Rect dest;
	
	src.w = src.h = dest.w = dest.h = 320;
	src.x = src.y = 0;
	dest.x = VIEW_W_HALF - 160;
	dest.y = VIEW_H_HALF - 160;

	if (portrait != NULL) {
		SDL_BlitSurface(portrait, &src, screen, &dest);		
	}
	if (portrait_border != NULL) {
		SDL_BlitSurface(portrait_border, &src, screen, &dest);
	}
	
	// display character name
	if (character_name == "") {
		font->render("Type your character name", VIEW_W_HALF, VIEW_H_HALF+176, JUSTIFY_CENTER, screen, FONT_GREY);
	}
	else {
		font->render(character_name, VIEW_W_HALF, VIEW_H_HALF+176, JUSTIFY_CENTER, screen, FONT_WHITE);
	}
}

GameStateNew::~GameStateNew() {
	SDL_FreeSurface(portrait);
	SDL_FreeSurface(portrait_border);
	delete button_exit;
	delete button_create;
	delete button_next;
	delete button_prev;
}

