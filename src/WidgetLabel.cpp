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

WidgetLabel::WidgetLabel(SDL_Surface *_screen, FontEngine *_font) {
	screen = _screen;
	font = _font;
	
	text = "";
	color = FONT_WHITE;
	x = y = 0;
	justify = JUSTIFY_LEFT;
}

void WidgetLabel::render() {
	font->renderShadowed(text, x, y, justify, screen, color);
}

/**
 * A shortcut function to set all attributes simultaneously.
 * All of these options can be set seperately if needed.
 */
void WidgetLabel::set(int _x, int _y, int _justify, int _valign, string _text, int _color) {
	justify = _justify;
	x = _x;
	text = _text;
	color = _color;
	
	if (_valign == VALIGN_TOP) {
		y = _y;
	}
	else if (_valign == VALIGN_BOTTOM) {
		y = _y - font->getFontHeight();
	}
	else if (_valign == VALIGN_CENTER) {
		y = _y - font->getFontHeight()/2;
	}
}

WidgetLabel::~WidgetLabel() {
}
