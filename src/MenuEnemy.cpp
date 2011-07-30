/**
 * MenuEnemy
 *
 * Handles the display of the Enemy bar on the HUD
 *
 * @author Pavel Kirpichyov (Cheshire)
 * @license GPL
 */

#include "MenuEnemy.h"

MenuEnemy::MenuEnemy(SDL_Surface *_screen, FontEngine *_font, MessageEngine *_msg) {
	screen = _screen;
	font = _font;
	msg = _msg;
	loadGraphics();
	enemy = NULL;
	timeout = 0;
}

void MenuEnemy::loadGraphics() {

	background = IMG_Load((PATH_DATA + "images/menus/bar_enemy.png").c_str());
	bar_hp = IMG_Load((PATH_DATA + "images/menus/bar_hp.png").c_str());
	
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
	
	font->render(msg->get("enemy_info", enemy->stats.level, enemy->stats.name), VIEW_W_HALF, 4, JUSTIFY_CENTER, screen, FONT_WHITE);
	stringstream ss;
	ss.str("");
	if (enemy->stats.hp > 0)
		ss << enemy->stats.hp << "/" << enemy->stats.maxhp;
	else
		ss << msg->get("dead");
	font->render(ss.str(), VIEW_W_HALF, 19, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	//SDL_UpdateRects(screen, 1, &dest);
}

MenuEnemy::~MenuEnemy() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar_hp);		
}
