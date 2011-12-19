/*
Copyright 2011 Clint Bellanger

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
 * class WidgetTooltip
 */

#ifndef WIDGET_TOOLTIP_H
#define WIDGET_TOOLTIP_H

#include "FontEngine.h"
#include "Utils.h"
#include "Settings.h"

#include <SDL.h>
#include <SDL_image.h>


const int STYLE_FLOAT = 0;
const int STYLE_TOPLABEL = 1;

const int TOOLTIP_MAX_LINES = 16;

struct TooltipData {
	string lines[TOOLTIP_MAX_LINES];
	int colors[TOOLTIP_MAX_LINES];
	int num_lines;
	SDL_Surface *tip_buffer;
	
	TooltipData() {
		num_lines = 0;
		tip_buffer = NULL;
		for (int i=0; i<TOOLTIP_MAX_LINES; i++) {
			lines[i] = "";
			colors[i] = FONT_WHITE;
		}
	}
	
	~TooltipData() {
		SDL_FreeSurface(tip_buffer);
	}
	
};

class WidgetTooltip {
private:
	int offset;
	int width;
	int margin;
public:
	WidgetTooltip();
	Point calcPosition(int style, Point pos, Point size);
	void render(TooltipData &tip, Point pos, int style);
	void clear(TooltipData &tip);
	void createBuffer(TooltipData &tip);
};

#endif
