#ifndef MENU_H
#define MENU_H

/**
 * class Menu
 *
 * The base class for Menu objects
 *
 * @author kitano
 * @license GPL
 */

#include "SDL_image.h"
#include "FontEngine.h"
#include "InputState.h"

class Menu {
protected:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;

	SDL_Surface *background;
	SDL_Rect window_area;

public:
	Menu(SDL_Surface*, InputState*, FontEngine*);

	bool visible;

	virtual void render() = 0;
};

#endif

