/*
Copyright © 2011-2012 kitano
Copyright © 2013 Henrik Andersson

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
 * class Menu
 *
 * The base class for Menu objects
 */

#include "Menu.h"
#include "Settings.h"

Menu::Menu()
	: background(NULL)
	, visible(false)
	, sfx_open(0)
	, sfx_close(0)
{}

Menu::~Menu() {
}

void Menu::align() {
	SDL_Rect temp = window_area;

	if (alignment == "topleft") {
		window_area.x = 0+temp.x;
		window_area.y = 0+temp.y;
	} else if (alignment == "top") {
		window_area.x = (VIEW_W_HALF-temp.w/2)+temp.x;
		window_area.y = 0+temp.y;
	} else if (alignment == "topright") {
		window_area.x = (VIEW_W-temp.w)+temp.x;
		window_area.y = 0+temp.y;
	} else if (alignment == "left") {
		window_area.x = 0+temp.x;
		window_area.y = (VIEW_H_HALF-temp.h/2)+temp.y;
	} else if (alignment == "center") {
		window_area.x = (VIEW_W_HALF-temp.w/2)+temp.x;
		window_area.y = (VIEW_H_HALF-temp.h/2)+temp.y;
	} else if (alignment == "right") {
		window_area.x = (VIEW_W-temp.w)+temp.x;
		window_area.y = (VIEW_H_HALF-temp.h/2)+temp.y;
	} else if (alignment == "bottomleft") {
		window_area.x = 0+temp.x;
		window_area.y = (VIEW_H-temp.h)+temp.y;
	} else if (alignment == "bottom") {
		window_area.x = (VIEW_W_HALF-temp.w/2)+temp.x;
		window_area.y = (VIEW_H-temp.h)+temp.y;
	} else if (alignment == "bottomright") {
		window_area.x = (VIEW_W-temp.w)+temp.x;
		window_area.y = (VIEW_H-temp.h)+temp.y;
	}
}
