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
#include "SDL_gfxBlitFunc.h"

using namespace std;

WidgetScrollBox::WidgetScrollBox(int width, int height) {
	pos.x = pos.y = 0;
	pos.w = width;
	pos.h = height;
	cursor = 0;
	bg.r = bg.g = bg.b = 0;
	contents = NULL;
	scrollbar = new WidgetScrollBar("images/menus/buttons/scrollbar_default.png");
	update = true;
	render_to_alpha = false;
	resize(height);
}

WidgetScrollBox::~WidgetScrollBox() {
	if (contents != NULL) SDL_FreeSurface(contents);
	delete scrollbar;
}

void WidgetScrollBox::scroll(int amount) {
	cursor += amount;
	if (cursor < 0) {
		cursor = 0;
	} else if (cursor > contents->h-pos.h) {
		cursor = contents->h-pos.h;
	}
	refresh();
}

Point WidgetScrollBox::input_assist(Point mouse) {
	Point new_mouse;
	new_mouse.x = mouse.x-pos.x;
	new_mouse.y = mouse.y-pos.y+cursor;
	return new_mouse;
}

void WidgetScrollBox::logic() {
	logic(inpt->mouse.x,inpt->mouse.y);
}

void WidgetScrollBox::logic(int x, int y) {
	Point mouse(x, y);

	if (isWithin(pos,mouse)) {
		if (inpt->scroll_up) {
			scroll(-20);
			inpt->resetScroll();
		}
		if (inpt->scroll_down) {
			scroll(20);
			inpt->resetScroll();
		}
	} else {
		inpt->resetScroll();
	}

	// check ScrollBar clicks
	if (contents->h > pos.h) {
		switch (scrollbar->checkClick(mouse.x,mouse.y)) {
			case 1:
				scroll(-20);
				break;
			case 2:
				scroll(20);
				break;
			case 3:
				cursor = scrollbar->getValue();
				break;
			default:
				break;
		}
	}
}

void WidgetScrollBox::resize(int h) {
	if (contents != NULL) SDL_FreeSurface(contents);

	if (pos.h > h) h = pos.h;
	contents = createAlphaSurface(pos.w,h);
	SDL_FillRect(contents,NULL,SDL_MapRGB(contents->format,bg.r,bg.g,bg.b));
	SDL_SetAlpha(contents, 0, 0);

	cursor = 0;
	refresh();
}

void WidgetScrollBox::refresh() {
	if (update) {
		int h = pos.h;
		if (contents != NULL) {
			h = contents->h;
			SDL_FreeSurface(contents);
		}

		contents = createAlphaSurface(pos.w,h);
		SDL_FillRect(contents,NULL,SDL_MapRGB(contents->format,bg.r,bg.g,bg.b));
		SDL_SetAlpha(contents, 0, 0);
	}

	scrollbar->refresh(pos.x+pos.w, pos.y, pos.h-scrollbar->pos_down.h, cursor, contents->h-pos.h-scrollbar->pos_knob.h);
}

void WidgetScrollBox::render(SDL_Surface *target) {
	if (target == NULL) {
		target = screen;
	}

	SDL_Rect	src,dest;
	dest = pos;
	src.x = 0;
	src.y = cursor;
	src.w = contents->w;
	src.h = pos.h;

	if (render_to_alpha)
		SDL_gfxBlitRGBA(contents, &src, target, &dest);
	else
		SDL_BlitSurface(contents, &src, target, &dest);
	if (contents->h > pos.h) scrollbar->render(target);
	update = false;
}

