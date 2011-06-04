/**
 * MenuExperience
 *
 * Handles the display of the Experience bar on the HUD
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuExperience.h"

MenuExperience::MenuExperience(SDL_Surface *_screen, FontEngine *_font) {
	screen = _screen;
	font = _font;
	loadGraphics();
	
	
	// move these settings to a config file
	hud_position.x = 0;
	hud_position.y = 32;
	hud_position.w = 106;
	hud_position.h = 26;
	background_offset.x = 0;
	background_offset.y = 0;
	background_size.x = 106;
	background_size.y = 10;
	bar_offset.x = 3;
	bar_offset.y = 3;
	bar_size.x = 100;
	bar_size.y = 4;
	text_offset.x = 2;
	text_offset.y = 12;
	text_justify = JUSTIFY_LEFT;
	text_label = "XP: ";
}

void MenuExperience::loadGraphics() {

	background = IMG_Load("images/menus/menu_xp.png");
	bar = IMG_Load("images/menus/bar_xp.png");
	
	if(!background || !bar) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		Mix_CloseAudio();
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);	
	
	cleanup = bar;
	bar = SDL_DisplayFormatAlpha(bar);
	SDL_FreeSurface(cleanup);
}

/**
 * Display the XP bar background and current progress.
 * On mouseover, display progress in text form.
 */
void MenuExperience::render(StatBlock *stats, Point mouse) {
	SDL_Rect src;
	SDL_Rect dest;
	int xp_bar_length;
	
	// don't display anything if max level
	// TODO: change this implementation if max level is configurable
	if (stats->level < 1 || stats->level >= 17) return;
	
	// lay down the background image first
	src.x = 0;
	src.y = 0;
	src.w = background_size.x;
	src.h = background_size.y;
	dest.x = hud_position.x + background_offset.x;
	dest.y = hud_position.y + background_offset.y;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// calculate the length of the xp bar
	// when at a new level, 0% progress
	src.x = 0;
	src.y = 0;
	int required = stats->xp_table[stats->level] - stats->xp_table[stats->level-1];
	int current = stats->xp - stats->xp_table[stats->level-1];
	xp_bar_length = (current * bar_size.x) / required;
	src.w = xp_bar_length;
	src.h = bar_size.y;
	dest.x = hud_position.x + bar_offset.x;
	dest.y = hud_position.y + bar_offset.y;
		
	// draw xp bar
	SDL_BlitSurface(bar, &src, screen, &dest);		
	
	// if mouseover, draw text
	if (isWithin(hud_position, mouse)) {
		stringstream ss;
		ss.str("");
		ss << text_label << stats->xp << "/" << stats->xp_table[stats->level];
		font->render(ss.str(), hud_position.x + text_offset.x, hud_position.y + text_offset.y, text_justify, screen, FONT_WHITE);
	}
}

MenuExperience::~MenuExperience() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(bar);
}

