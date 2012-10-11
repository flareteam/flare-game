/*
Copyright © 2011-2012 Clint Bellanger and Thane Brimhall

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

#ifndef FONT_ENGINE_H
#define FONT_ENGINE_H

#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <fstream>
#include <string>
#include <map>
#include <vector>


const int JUSTIFY_LEFT = 0;
const int JUSTIFY_RIGHT = 1;
const int JUSTIFY_CENTER = 2;

const SDL_Color FONT_WHITE = {255,255,255,0};
const SDL_Color FONT_BLACK = {0,0,0,0};

struct FontStyle {
	std::string name;
	std::string path;
	int ptsize;
	bool blend;
	TTF_Font *ttfont;
	int line_height;
	int font_height;

	FontStyle();
};

/**
 * class FontEngine
 * 
 * Handles rendering a bitmap font.
 */

class FontEngine {
private:
	SDL_Rect src;
	SDL_Rect dest;
	SDL_Surface *ttf;
	std::map<std::string,SDL_Color> color_map;
	std::vector<FontStyle> font_styles;
	FontStyle *active_font;

public:
	FontEngine();
	~FontEngine();

	int getLineHeight() { return active_font->line_height; }
	int getFontHeight() { return active_font->font_height; }
	
	SDL_Color getColor(std::string _color);
	void setFont(std::string _font);

	int calc_width(const std::string& text);
	Point calc_size(const std::string& text_with_newlines, int width);

	void render(const std::string& text, int x, int y, int justify, SDL_Surface *target, SDL_Color color);
	void render(const std::string& text, int x, int y, int justify, SDL_Surface *target, int width, SDL_Color color);
	void renderShadowed(const std::string& text, int x, int y, int justify, SDL_Surface *target, SDL_Color color);
	void renderShadowed(const std::string& text, int x, int y, int justify, SDL_Surface *target, int width, SDL_Color color);

	int cursor_y;
};

#endif
