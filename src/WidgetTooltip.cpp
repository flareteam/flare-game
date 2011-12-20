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

#include "WidgetTooltip.h"
#include "SharedResources.h"
#include "Utils.h"

WidgetTooltip::WidgetTooltip() {

	// TODO: Put these values in an engine config file
	
	offset=12; // distance between cursor and tooltip
	width=160; // max width of tooltips (wrap text)
	margin=4; // outer margin between tooltip text and the edge of the tooltip background
}

/**
 * Knowing the total size of the text and the position of origin,
 * calculate the starting position of the background and text
 */
Point WidgetTooltip::calcPosition(int style, Point pos, Point size) {

	Point tip_pos;

	// TopLabel style is fixed and centered over the origin
	if (style == STYLE_TOPLABEL) {
		tip_pos.x = pos.x - size.x/2;
		tip_pos.y = pos.y - offset;
	}
	// Float style changes position based on the screen quadrant of the origin
	// (usually used for tooltips which are long and we don't want them to overflow
	//  off the end of the screen)
	else if (style == STYLE_FLOAT) {
		// upper left
		if (pos.x < VIEW_W_HALF && pos.y < VIEW_H_HALF) {
			tip_pos.x = pos.x + offset;
			tip_pos.y = pos.y + offset;
		}
		// upper right
		else if (pos.x >= VIEW_W_HALF && pos.y < VIEW_H_HALF) {
			tip_pos.x = pos.x - offset - size.x;
			tip_pos.y = pos.y + offset;
		}
		// lower left
		else if (pos.x < VIEW_W_HALF && pos.y >= VIEW_H_HALF) {
			tip_pos.x = pos.x + offset;
			tip_pos.y = pos.y - offset - size.y;
		}
		// lower right
		else if (pos.x >= VIEW_W_HALF && pos.y >= VIEW_H_HALF) {
			tip_pos.x = pos.x - offset - size.x;		
			tip_pos.y = pos.y - offset - size.y;		
		}
	}
	
	return tip_pos;
} 

/**
 * Tooltip position depends on the screen quadrant of the source.
 * Draw the buffered tooltip if it exists, else render the tooltip and buffer it
 */
void WidgetTooltip::render(TooltipData &tip, Point pos, int style) {

	if (tip.tip_buffer == NULL) {
		createBuffer(tip);
	}

	Point size;
	size.x = tip.tip_buffer->w;
	size.y = tip.tip_buffer->h;
		
	Point tip_pos = calcPosition(style, pos, size);

	SDL_Rect dest;
	dest.x = tip_pos.x;
	dest.y = tip_pos.y;
	
	SDL_BlitSurface(tip.tip_buffer, NULL, screen, &dest);
}

/**
 * Clear the given tooltip.
 * Free the buffered tip image
 * Note most tooltip usage will assume WHITE default color, so reset it
 */
void WidgetTooltip::clear(TooltipData &tip) {
	tip.num_lines = 0;
	for (int i=0; i<TOOLTIP_MAX_LINES; i++) {
		tip.lines[i] = "";
		tip.colors[i] = FONT_WHITE;
	}
	SDL_FreeSurface(tip.tip_buffer);
	tip.tip_buffer = NULL;
}

/**
 * Rendering a wordy tooltip (TTF to raster) can be expensive.
 * Instead of doing this each frame, do it once and cache the result.
 */
void WidgetTooltip::createBuffer(TooltipData &tip) {
	
	// concat multi-line tooltip, used in determining total display size
	string fulltext;
	fulltext = tip.lines[0];
	for (int i=1; i<tip.num_lines; i++) {
		fulltext = fulltext + "\n" + tip.lines[i];
	}
	
	// calculate the full size to display a multi-line tooltip
	Point size = font->calc_size(fulltext, width);
	
	// WARNING: dynamic memory allocation. Be careful of memory leaks.
	tip.tip_buffer = createSurface(size.x + margin+margin, size.y + margin+margin);
	
	// Currently tooltips are always opaque
	SDL_SetAlpha(tip.tip_buffer, 0, 0);
	
	// style the tooltip background
	// currently this is plain black
	SDL_FillRect(tip.tip_buffer, NULL, 0);
	
	int cursor_y = margin;
	
	for (int i=0; i<tip.num_lines; i++) {
		font->render(tip.lines[i], margin, cursor_y, JUSTIFY_LEFT, tip.tip_buffer, size.x, tip.colors[i]);
		cursor_y = font->cursor_y;
	}

}

