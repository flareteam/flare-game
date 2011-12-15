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
 * class WidgetLabel
 *
 * A simple text display for menus
 */

#include "WidgetLabel.h"
#include "SharedResources.h"

WidgetLabel::WidgetLabel() {
	
	text_buffer = NULL;
	text = "";
	color = FONT_WHITE;
	justify = JUSTIFY_LEFT;
	valign = VALIGN_TOP;
	
	bounds.x = bounds.y = 0;
	bounds.w = bounds.h = 0;
	
}

/**
 * Draw the buffered string surface to the screen
 */
void WidgetLabel::render() {

	SDL_Rect dest;
	dest.x = bounds.x;
	dest.y = bounds.y;
	dest.w = bounds.w;
	dest.h = bounds.h;

	if (text_buffer != NULL) {
		SDL_BlitSurface(text_buffer, NULL, screen, &dest);		
	}
}


/**
 * A shortcut function to set all attributes simultaneously.
 */
void WidgetLabel::set(int _x, int _y, int _justify, int _valign, string _text, int _color) {

	bool changed = false;

	if (justify != _justify) {
		justify = _justify;
		changed = true;
	}
	if (valign != _valign) {
		valign = _valign;
		changed = true;
	}
	if (text != _text) {
		text = _text;
		changed = true;
	}
	if (color != _color) {
		color = _color;
		changed = true;
	}
	if (x_origin != _x) {
		x_origin = _x;
		changed = true;
	}
	if (y_origin != _y) {
		y_origin = _y;
		changed = true;
	}
	
	if (changed) {
		applyOffsets();
		refresh();
	}
}

/**
 * Apply horizontal justify and vertical alignment to label position
 */
void WidgetLabel::applyOffsets() {

	bounds.w = font->calc_width(text);
	bounds.h = font->getFontHeight();

	// apply JUSTIFY
	if (justify == JUSTIFY_LEFT)
		bounds.x = x_origin;
	else if (justify == JUSTIFY_RIGHT)
		bounds.x = x_origin - bounds.w;
	else if (justify == JUSTIFY_CENTER)
		bounds.x = x_origin - bounds.w/2;

	// apply VALIGN
	if (valign == VALIGN_TOP) {
		bounds.y = y_origin;
	}
	else if (valign == VALIGN_BOTTOM) {
		bounds.y = y_origin - bounds.h;;
	}
	else if (valign == VALIGN_CENTER) {
		bounds.y = y_origin - bounds.h/2;
	}
	
}

/**
 * Update the label text only
 */
void WidgetLabel::set(string _text) {
	if (text != _text) {
		this->text = _text;
		applyOffsets();
		refresh();
	}
}

/**
 * We buffer the rendered text instead of calculating it each frame
 * This function refreshes the buffer.
 */
void WidgetLabel::refresh() {

	SDL_FreeSurface(text_buffer);
	text_buffer = createSurface(bounds.w, bounds.h);
	font->renderShadowed(text, 0, 0, JUSTIFY_LEFT, text_buffer, color);
	
}


WidgetLabel::~WidgetLabel() {
	SDL_FreeSurface(text_buffer);
}
