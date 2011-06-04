/*
 * class FontEngine
 * Handles rendering a bitmap font
 *
 * @author Clint Bellanger
 * @license GPL
 *
 */

#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H


#include <fstream>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "Utils.h"
#include "UtilsParsing.h"

using namespace std;

const int JUSTIFY_LEFT = 0;
const int JUSTIFY_RIGHT = 1;
const int JUSTIFY_CENTER = 2;

const int FONT_WHITE = 0;
const int FONT_RED = 1;
const int FONT_GREEN = 2;
const int FONT_BLUE = 3;
const int FONT_GRAY = 4;
const int FONT_GREY = 4;

class FontEngine {
private:
	SDL_Surface *sprites[5];
	int font_width;
	int font_height;
	int kerning;
	int width[256]; // width of each ASCII character
	SDL_Rect src;
	SDL_Rect dest;

public:
	FontEngine();
	~FontEngine();
	void load();

	int calc_length(string text);
	Point calc_size(string text_with_newlines, int width);
	
	void render(string text, int x, int y, int justify, SDL_Surface *target, int color);
	void render(string text, int x, int y, int justify, SDL_Surface *target, int width, int color);
	
	int cursor_y;
	int line_height;
};

#endif
