/*
Copyright © 2011-2012 Clint Bellanger

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

#ifndef WIDGET_H
#define WIDGET_H

/**
 * Base interface all widget needs to implement
 */
#include <SDL.h>
class Widget {
public:
	Widget()
	: render_to_alpha(false)
	{};

	virtual ~Widget() {};
	virtual void render(SDL_Surface *target = NULL) = 0;
	bool render_to_alpha;
private:
};

#endif

