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
 * class MenuTooltip
 */

#include "MenuTooltip.h"

MenuTooltip::MenuTooltip(FontEngine *_font, SDL_Surface *_screen) {
	font = _font;
	screen = _screen;
	offset=12;
	width=160;
	margin=4;
}

/**
 * Knowing the total size of the text and the position of origin,
 * calculate the starting position of the background and text
 */
void MenuTooltip::calcPosition(int style, Point pos, Point size, Sint16 &bgx, Sint16 &bgy, int &curx, int &cury) {

	// TopLabel style is fixed and centered over the origin
	if (style == STYLE_TOPLABEL) {
		bgx = pos.x - size.x/2 - margin;
		bgy = pos.y - offset - margin;
		curx = bgx + margin;
		cury = bgy + margin;
	}
	// Float style changes position based on the screen quadrant of the origin
	// (usually used for tooltips which are long and we don't want them to overflow
	//  off the end of the screen)
	else if (style == STYLE_FLOAT) {
		// upper left
		if (pos.x < VIEW_W_HALF && pos.y < VIEW_H_HALF) {
			bgx = pos.x + offset;
			bgy = pos.y + offset;
			curx = pos.x + offset + margin;
			cury = pos.y + offset + margin;
		}
		// upper right
		else if (pos.x >= VIEW_W_HALF && pos.y < VIEW_H_HALF) {
			bgx = pos.x - offset - size.x - margin - margin;
			bgy = pos.y + offset;
			curx = pos.x - offset - size.x - margin;
			cury = pos.y + offset + margin;
		}
		// lower left
		else if (pos.x < VIEW_W_HALF && pos.y >= VIEW_H_HALF) {
			bgx = pos.x + offset;
			bgy = pos.y - offset - size.y - margin - margin;
			curx = pos.x + offset + margin;
			cury = pos.y - offset - size.y - margin;
		}
		// lower right
		else if (pos.x >= VIEW_W_HALF && pos.y >= VIEW_H_HALF) {
			bgx = pos.x - offset - size.x - margin - margin;		
			bgy = pos.y - offset - size.y - margin - margin;		
			curx = pos.x - offset - size.x - margin;
			cury = pos.y - offset - size.y - margin;
		}
	}
} 

/**
 * Tooltip position depends on the screen quadrant of the source
 */
void MenuTooltip::render(TooltipData tip, Point pos, int style) {
	SDL_Rect background;
	
	string fulltext;
	
	fulltext = tip.lines[0];
	for (int i=1; i<tip.num_lines; i++) {
		fulltext = fulltext + "\n" + tip.lines[i];
	}
	
	Point size = font->calc_size(fulltext, width);
	background.w = size.x + margin + margin;
	background.h = size.y + margin + margin;
	
	int cursor_x;
	int cursor_y;
	
	calcPosition(style, pos, size, background.x, background.y, cursor_x, cursor_y);
	
	SDL_FillRect(screen, &background, 0);
	for (int i=0; i<tip.num_lines; i++) {
		font->render(tip.lines[i], cursor_x, cursor_y, JUSTIFY_LEFT, screen, size.x, tip.colors[i]);
		cursor_y = font->cursor_y;
	}
}
