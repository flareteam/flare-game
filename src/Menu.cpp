/**
 * class Menu
 *
 * The base class for Menu objects
 *
 * @author kitano
 * @license GPL
 */

#include "Menu.h"

Menu::Menu(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) {
	screen = _screen;
	inp = _inp;
	font = _font;

	visible = false;
}
