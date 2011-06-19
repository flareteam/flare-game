#ifndef WIDGETINPUT_H
#define WIDGETINPUT_H

/**
 * class WidgetInput
 *
 * @author kitano
 * @license GPl
 *
 * A simple text box with a label above it.
 * It has two images - one for focused and one for unfocused.
 */

#include "FontEngine.h"
#include "InputState.h"
#include "SDL.h"
#include <string>

using namespace std;

class WidgetInput {

protected:
	SDL_Surface *screen;
	FontEngine *font;
	InputState *inp;

	SDL_Surface *background;

	SDL_Rect pos;

	bool enabled;
	bool inFocus;
	bool pressed;

	string text; // the text that has been type into the box
	string label; // the label that appears above the text box
	int max_characters;

public:
	WidgetInput(SDL_Surface *_screen, FontEngine *_font, InputState *_inp, string _label);
	
	void logic();
	void render();

	bool checkClick();

	string getText() { return text; }
};

#endif
