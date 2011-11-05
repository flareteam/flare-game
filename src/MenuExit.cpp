/*
Copyright 2011 kitano

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class MenuExit
 */

#include "MenuExit.h"

MenuExit::MenuExit(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) : Menu(_screen, inp = _inp, _font) {

	exitClicked = false;

	window_area.w = 192;
	window_area.h = 64;
	window_area.x = (VIEW_W/2) - (window_area.w/2);
	window_area.y = (VIEW_H - window_area.h)/2;
	
	buttonExit = new WidgetButton(screen, font, inp, "images/menus/buttons/button_default.png");
	buttonExit->label = msg->get("Exit");
	buttonExit->pos.x = VIEW_W_HALF - buttonExit->pos.w/2;
	buttonExit->pos.y = VIEW_H/2;

	buttonClose = new WidgetButton(screen, font, inp, "images/menus/buttons/button_x.png");
	buttonClose->pos.x = window_area.x + window_area.w;
	buttonClose->pos.y = window_area.y;

	loadGraphics();
}

void MenuExit::loadGraphics() {
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

void MenuExit::logic() {
	if (visible) {
		if (buttonExit->checkClick()) {
			exitClicked = true;	
		}
		if (buttonClose->checkClick()) {
			visible = false;
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

	font->render(msg->get("Save and exit to title?"), window_area.x + window_area.w/2, window_area.y + 10, JUSTIFY_CENTER, screen, FONT_WHITE);

	buttonExit->render();
	buttonClose->render();
}

MenuExit::~MenuExit() {
	delete buttonExit;
	delete buttonClose;
	SDL_FreeSurface(background);
}

