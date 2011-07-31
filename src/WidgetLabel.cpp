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

void WidgetLabel::setPosition(int _x, int _y, int _justify, int valign) {
	justify = _justify;
	x = _x;
	
	if (valign == VALIGN_TOP) {
		y = _y;
	}
	else if (valign == VALIGN_BOTTOM) {
		y = _y - font->getFontHeight();
	}
	else if (valign == VALIGN_CENTER) {
		y = _y - font->getFontHeight()/2;
	}

}

WidgetLabel::~WidgetLabel() {
}
