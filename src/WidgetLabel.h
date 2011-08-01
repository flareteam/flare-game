/**
 * class WidgetLabel
 *
 * @author Clint Bellanger
 * @license GPL
 *
 * A simple text display for menus.
 * This is preferred to directly displaying text because it helps handle positioning
 */

#ifndef WIDGET_LABEL_H
#define WIDGET_LABEL_H

#include "SDL.h"
#include "FontEngine.h"
#include <string>

const int VALIGN_CENTER = 0;
const int VALIGN_TOP = 1;
const int VALIGN_BOTTOM = 2;

class WidgetLabel {
private:
	SDL_Surface *screen;
	FontEngine *font;

public:
	WidgetLabel(SDL_Surface *_screen, FontEngine *_font);
	~WidgetLabel();
	void render();
	void set(int _x, int _y, int _justify, int _valign, string _text, int _color);
	
	string text;
	int color;
	int x;
	int y;
	int justify;
};

#endif
