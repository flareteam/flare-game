/*
Copyright 2011 Clint Bellanger and Thane Brimhall

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

/*
 * class FontEngine
 * Handles rendering a bitmap font
 */

#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H


#include "Settings.h"
#include "Utils.h"
#include "UtilsParsing.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <fstream>
#include <string>


const int JUSTIFY_LEFT = 0;
const int JUSTIFY_RIGHT = 1;
const int JUSTIFY_CENTER = 2;

const int FONT_WHITE = 0;
const int FONT_RED = 1;
const int FONT_GREEN = 2;
const int FONT_BLUE = 3;
const int FONT_GRAY = 4;
const int FONT_GREY = 4;
const int FONT_BLACK = 5;

class FontEngine {
private:
	SDL_Color colors[6];
	int font_pt;
	int font_height;
	int line_height;
	SDL_Rect src;
	SDL_Rect dest;
	SDL_Surface *ttf;
	TTF_Font *ttfont;

public:
	FontEngine();
	~FontEngine();

	int getLineHeight() { return line_height; }
	int getFontHeight() { return font_height; }
	
	int calc_width(std::string text);
	Point calc_size(std::string text_with_newlines, int width);

	void render(std::string text, int x, int y, int justify, SDL_Surface *target, int color);
	void render(std::string text, int x, int y, int justify, SDL_Surface *target, int width, int color);
	void renderShadowed(std::string text, int x, int y, int justify, SDL_Surface *target, int color);
	void renderShadowed(std::string text, int x, int y, int justify, SDL_Surface *target, int width, int color);

	int cursor_y;
};

#endif
