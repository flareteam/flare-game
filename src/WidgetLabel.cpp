/**
 * class WidgetLabel
 *
 * @author Clint Bellanger
 * @license GPL
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
	font->render(text, x, y, justify, screen, color);
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
