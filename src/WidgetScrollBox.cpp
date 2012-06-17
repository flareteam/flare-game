/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Justin Jacobs

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
 * class WidgetScrollBox
 */

#include "WidgetScrollBox.h"

using namespace std;

WidgetScrollBox::WidgetScrollBox (int width, int height) {
    cursor = 0;
    contents = createSurface(width,height);
	scrollbar = new WidgetScrollBar(mods->locate("images/menus/buttons/scrollbar_default.png"));
}

WidgetScrollBox::~WidgetScrollBox () {
	SDL_FreeSurface(contents);
    delete scrollbar;
}

void WidgetScrollBox::scroll(int amount) {
    cursor += amount;
    if (cursor < 0) {
        cursor = 0;
    } else if (cursor > contents->h-pos.h) {
        cursor = contents->h-pos.h;
    }
}

void WidgetScrollBox::logic () {
	// check ScrollBar clicks
    switch (scrollbar->checkClick()) {
        case 1:
            scroll(20);
            break;
        case 2:
            scroll(-20);
            break;
        case 3:
            cursor = scrollbar->getValue();
            break;
        default:
            break;
    }
}

void WidgetScrollBox::refresh() {
    scrollbar->refresh(pos.x+pos.w, pos.y, pos.h, cursor, contents->h-pos.h);
}

void WidgetScrollBox::render () {
	SDL_Rect    src;
	src.x = 0;
	src.y = cursor;
	src.w = contents->w;
	src.h = contents->h;

	SDL_BlitSurface(contents, &src, screen, &pos);
    scrollbar->render();
}

