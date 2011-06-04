/**
 * MenuEnemy
 *
 * Handles the display of the Enemy bar on the HUD
 *
 * @author Pavel Kirpichyov (Cheshire)
 * @license GPL
 */

#include "MenuEnemy.h"

MenuEnemy::MenuEnemy(SDL_Surface *_screen, FontEngine *_font) {
	screen = _screen;
	font = _font;
	loadGraphics();
	enemy = NULL;
	timeout = 0;
}

void MenuEnemy::loadGraphics() {

	background = IMG_Load("images/menus/bar_enemy.png");
	bar_hp = IMG_Load("images/menus/bar_hp.png");
	
	if(!background || !bar_hp) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
	
	cleanup = bar_hp;
	bar_hp = SDL_DisplayFormatAlpha(bar_hp);
	SDL_FreeSurface(cleanup);
}

void MenuEnemy::handleNewMap() {
	enemy = NULL;
}

void MenuEnemy::logic() {

	// after a fixed amount of time, hide the enemy display
	if (timeout > 0) timeout--;
	if (timeout == 0) enemy = NULL;
}

void MenuEnemy::render() {
	if (enemy == NULL) return;
	
	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;
	
	// draw trim/background
	dest.x = VIEW_W_HALF-53;
	dest.y = 0;
	dest.w = 106;
	dest.h = 33;
	
	SDL_BlitSurface(background, NULL, screen, &dest);
	
	if (enemy->stats.maxhp == 0)
		hp_bar_length = 0;
	else
		hp_bar_length = (enemy->stats.hp * 100) / enemy->stats.maxhp;

	// draw hp bar
	
	dest.x = VIEW_W_HALF-50;
	dest.y = 18;

	src.x = 0;
	src.y = 0;
	src.h = 12;
	src.w = hp_bar_length;	
	
	SDL_BlitSurface(bar_hp, &src, screen, &dest);
	
	stringstream ss;
	ss << enemy->stats.name << " level " << enemy->stats.level;
	font->render(ss.str(), VIEW_W_HALF, 4, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	if (enemy->stats.hp > 0)
		ss << enemy->stats.hp << "/" << enemy->stats.maxhp;
	else
		ss << "Dead";
	font->render(ss.str(), VIEW_W_HALF, 19, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	//SDL_UpdateRects(screen, 1, &dest);
}

MenuEnemy::~MenuEnemy() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar_hp);		
}
