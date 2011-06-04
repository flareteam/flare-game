/**
 * MenuHPMP
 *
 * Handles the display of the HP and MP bars at the top/left of the screen
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuHPMP.h"

MenuHPMP::MenuHPMP(SDL_Surface *_screen, FontEngine *_font) {
	screen = _screen;
	font = _font;
	
	loadGraphics();
}

void MenuHPMP::loadGraphics() {

	background = IMG_Load("images/menus/bar_hp_mp.png");
	bar_hp = IMG_Load("images/menus/bar_hp.png");
	bar_mp = IMG_Load("images/menus/bar_mp.png");
	
	if(!background || !bar_hp || !bar_mp) {
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
	
	cleanup = bar_mp;
	bar_mp = SDL_DisplayFormatAlpha(bar_mp);
	SDL_FreeSurface(cleanup);
	
}

void MenuHPMP::render(StatBlock *stats, Point mouse) {
	SDL_Rect src;
	SDL_Rect dest;
	int hp_bar_length;
	int mp_bar_length;
	
	// draw trim/background
	src.x = dest.x = 0;
	src.y = dest.y = 0;
	src.w = dest.w = 106;
	src.h = dest.h = 33;
	
	SDL_BlitSurface(background, &src, screen, &dest);
	
	if (stats->maxhp == 0)
		hp_bar_length = 0;
	else
		hp_bar_length = (stats->hp * 100) / stats->maxhp;
		
	if (stats->maxmp == 0)
		mp_bar_length = 0;
	else
		mp_bar_length = (stats->mp * 100) / stats->maxmp;
	
	// draw hp bar
	src.x = 0;
	src.y = 0;
	src.h = 12;
	dest.x = 3;
	dest.y = 3;
	src.w = hp_bar_length;	
	SDL_BlitSurface(bar_hp, &src, screen, &dest);
	
	// draw mp bar
	dest.y = 18;
	src.w = mp_bar_length;
	SDL_BlitSurface(bar_mp, &src, screen, &dest);		
	
	// if mouseover, draw text
	if (mouse.x <= 106 && mouse.y <= 33) {

		stringstream ss;
		ss << stats->hp << "/" << stats->maxhp;
		font->render(ss.str(), 53,4,JUSTIFY_CENTER, screen, FONT_WHITE);
		ss.str("");
		ss << stats->mp << "/" << stats->maxmp;
		font->render(ss.str(), 53,19,JUSTIFY_CENTER, screen, FONT_WHITE);
	 
	}
}

MenuHPMP::~MenuHPMP() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar_hp);
	SDL_FreeSurface(bar_mp);
	
}

