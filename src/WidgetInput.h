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

	void loadGraphics(string filename);

	SDL_Surface *background;

	bool enabled;
	bool inFocus;
	bool pressed;

	string text; // the text that has been type into the box
	int max_characters;
	int cursor_frame;
	
	Point font_pos;

public:
	WidgetInput(SDL_Surface *_screen, FontEngine *_font, InputState *_inp);
	
	void logic();
	void render();
	bool checkClick();
	string getText() { return text; }
	void setPosition(int x, int y);

	SDL_Rect pos;


};

#endif
