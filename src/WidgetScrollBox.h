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

#ifndef WIDGET_SCROLLBOX_H
#define WIDGET_SCROLLBOX_H

#include "SharedResources.h"
#include "Widget.h"
#include "WidgetScrollBar.h"

class Widget;

class WidgetScrollBox : public Widget {
public:
	WidgetScrollBox (int width, int height);
	~WidgetScrollBox ();

	Point input_assist(Point mouse);
	void logic();
	void logic(int x, int y);
	void resize(int h);
	void refresh();
	void render(SDL_Surface *target = NULL);

	SDL_Rect pos;
	SDL_Surface * contents;
	bool update;
	SDL_Color bg;

private:
	void scroll(int amount);

	int cursor;
	WidgetScrollBar * scrollbar;
};

#endif

