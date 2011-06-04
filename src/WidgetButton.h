/**
 * class WidgetButton
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "FontEngine.h"
#include "InputState.h"

const int BUTTON_GFX_NORMAL = 0;
const int BUTTON_GFX_PRESSED = 1;
const int BUTTON_GFX_HOVER = 2;
const int BUTTON_GFX_DISABLED = 3;

class WidgetButton {
private:
	SDL_Surface *screen;
	FontEngine *font;
	InputState *inp;

	SDL_Surface *buttons;
	Mix_Chunk *click;
	
public:
	WidgetButton(SDL_Surface *_screen, FontEngine *_font, InputState *_inp);
	~WidgetButton();

	void loadArt();
	bool checkClick();
	void render();

	string label;
	SDL_Rect pos;
	bool enabled;
	bool pressed;
};

#endif
