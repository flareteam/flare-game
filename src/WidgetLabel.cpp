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

/**
 * class WidgetLabel
 *
 * A simple text display for menus
 */

#include "WidgetLabel.h"
#include "SharedResources.h"
#include "SDL_gfxBlitFunc.h"
#include "UtilsParsing.h"

using namespace std;

LabelInfo::LabelInfo() : x(0), y(0), justify(JUSTIFY_LEFT), valign(VALIGN_TOP), hidden(false), font_style("font_regular") {
}

/**
 * This is used in menus (e.g. MenuInventory) when parsing their config files
 */
LabelInfo eatLabelInfo(string val) {
	LabelInfo info;
	string justify,valign,style;

	std::string tmp = eatFirstString(val,',');
	if (tmp == "hidden") {
		info.hidden = true;
	} else {
		info.hidden = false;
		info.x = atoi(tmp.c_str());
		info.y = eatFirstInt(val,',');
		justify = eatFirstString(val,',');
		valign = eatFirstString(val,',');
		style = eatFirstString(val,',');

		if (justify == "left") info.justify = JUSTIFY_LEFT;
		else if (justify == "center") info.justify = JUSTIFY_CENTER;
		else if (justify == "right") info.justify = JUSTIFY_RIGHT;

		if (valign == "top") info.valign = VALIGN_TOP;
		else if (valign == "center") info.valign = VALIGN_CENTER;
		else if (valign == "bottom") info.valign = VALIGN_BOTTOM;

		if (style != "") info.font_style = style;
	}

	return info;
}


WidgetLabel::WidgetLabel() {

	text_buffer = NULL;
	text = "";
	color = font->getColor("widget_normal");
	x_origin = y_origin = 0;
	justify = JUSTIFY_LEFT;
	valign = VALIGN_TOP;
	font_style = "font_regular";

	bounds.x = bounds.y = 0;
	bounds.w = bounds.h = 0;

	render_to_alpha = false;
}

/**
 * Draw the buffered string surface to the screen
 */
void WidgetLabel::render(SDL_Surface *target) {
	if (target == NULL) {
		target = screen;
	}

	SDL_Rect dest;
	dest.x = bounds.x;
	dest.y = bounds.y;
	dest.w = bounds.w;
	dest.h = bounds.h;

	if (text_buffer != NULL) {
		if (render_to_alpha)
			SDL_gfxBlitRGBA(text_buffer, NULL, target, &dest);
		else
			SDL_BlitSurface(text_buffer, NULL, target, &dest);
	}
}


void WidgetLabel::set(int _x, int _y, int _justify, int _valign, const string& _text, SDL_Color _color) {
	set(_x, _y, _justify, _valign, _text, _color, "font_regular");
}

/**
 * A shortcut function to set all attributes simultaneously.
 */
void WidgetLabel::set(int _x, int _y, int _justify, int _valign, const string& _text, SDL_Color _color, std::string _font) {

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
	if (color.r != _color.r || color.g != _color.g || color.b != _color.b) {
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
	if (font_style != _font) {
		font_style = _font;
		changed = true;
	}
	
	if (changed) {
		applyOffsets();
		refresh();
	}
}

/**
 * Set initial X position of label.
 */
void WidgetLabel::setX(int _x) {
	if (x_origin != _x) {
		x_origin = _x;
		applyOffsets();
		refresh();
	}
}

/**
 * Set initial Y position of label.
 */
void WidgetLabel::setY(int _y) {
	if (y_origin != _y) {
		y_origin = _y;
		applyOffsets();
		refresh();
	}
}

/**
 * Get X position of label.
 */
int WidgetLabel::getX() {
	return x_origin;
}

/**
 * Get Y position of label.
 */
int WidgetLabel::getY() {
	return y_origin;
}

/**
 * Set justify value.
 */
void WidgetLabel::setJustify(int _justify) {
	if (justify != _justify) {
		justify = _justify;
		applyOffsets();
		refresh();
	}
}

/**
 * Set valign value.
 */
void WidgetLabel::setValign(int _valign) {
	if (valign != _valign) {
		valign = _valign;
		applyOffsets();
		refresh();
	}
}

/**
 * Set text color.
 */
void WidgetLabel::setColor(SDL_Color _color) {
	if (color.r != _color.r && color.g != _color.g && color.b != _color.b) {
		color = _color;
		applyOffsets();
		refresh();
	}
}

/**
 * Apply horizontal justify and vertical alignment to label position
 */
void WidgetLabel::applyOffsets() {

	font->setFont(font_style);

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
void WidgetLabel::set(const string& _text) {
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
	text_buffer = createAlphaSurface(bounds.w, bounds.h);
	font->setFont(font_style);
	font->renderShadowed(text, 0, 0, JUSTIFY_LEFT, text_buffer, color);
	
}


WidgetLabel::~WidgetLabel() {
	SDL_FreeSurface(text_buffer);
}
