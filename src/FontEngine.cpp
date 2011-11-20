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

#include "FontEngine.h"
#include "FileParser.h"
#include "ModManager.h"
#include <iostream>

FontEngine::FontEngine() {
	font_pt = 10;

	// Initiate SDL_ttf
	if(!TTF_WasInit() && TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	// load the font
	string font_path;
	FileParser infile;
	if (infile.open(mods->locate("engine/font_settings.txt"))) {
		while (infile.next()) {
			if (infile.key == "font_regular"){
				font_path = infile.val;
			}
			if (infile.key == "ptsize"){
				font_pt = atoi(infile.val.c_str());
			}
		}
	}
	font_path = mods->locate("fonts/" + font_path);
	font = TTF_OpenFont(font_path.c_str(), font_pt);
	if(!font) printf("TTF_OpenFont: %s\n", TTF_GetError());

	// calculate the optimal line height
	line_height = TTF_FontLineSkip(font);
	font_height = TTF_FontHeight(font); 

	// set the font colors
	SDL_Color white = {255,255,255};
	SDL_Color red = {255,0,0};
	SDL_Color green = {0,255,0};
	SDL_Color blue = {0,0,255};
	SDL_Color grey = {128,128,128};
	SDL_Color black = {0,0,0};

	colors[FONT_WHITE] = white;
	colors[FONT_RED] = red;
	colors[FONT_GREEN] = green;
	colors[FONT_BLUE] = blue;
	colors[FONT_GREY] = grey;
	colors[FONT_BLACK] = black;
}

int FontEngine::calc_length(string text) {
	int w, h;
	const char* char_text = text.c_str(); //makes it so SDL_ttf functions can read it
	TTF_SizeUTF8(font, char_text, &w, &h);
	return w;
}

/**
 * Using the given wrap width, calculate the width and height necessary to display this text
 */
Point FontEngine::calc_size(string text_with_newlines, int width) {
	char newline = 10;
	
	string text = text_with_newlines;

	// if this contains newlines, recurse
	int check_newline = text.find_first_of(newline);
	if (check_newline > -1) {
		Point p1 = calc_size(text.substr(0, check_newline), width);
		Point p2 = calc_size(text.substr(check_newline+1, text.length()), width);
		Point p3;
		
		if (p1.x > p2.x) p3.x = p1.x;
		else p3.x = p2.x;
		
		p3.y = p1.y + p2.y;
		return p3;
	}

	int height = 0;
	int max_width = 0;

	string segment;
	string fulltext;
	string builder = "";
	string builder_prev = "";
	char space = 32;
	
	fulltext = text + " ";
	segment = eatFirstString(fulltext, space);
	
	while(segment != "" || fulltext.length() > 0) { // don't exit early on double spaces
		builder = builder + segment;
		
		if (calc_length(builder) > width) {
			height = height + getLineHeight();
			if (calc_length(builder_prev) > max_width) max_width = calc_length(builder_prev);
			builder_prev = "";
			builder = segment + " ";
		}
		else {
			builder = builder + " ";
			builder_prev = builder;
		}
		
		segment = eatFirstString(fulltext, space);
	}
	
	height = height + getLineHeight();
	builder = trim(builder, ' '); //removes whitespace that shouldn't be included in the size
	if (calc_length(builder) > max_width) max_width = calc_length(builder);
		
	Point size;
	size.x = max_width;
	size.y = height;
	return size;
}


/**
 * Render the given text at (x,y) on the target image.
 * Justify is left, right, or center
 */
void FontEngine::render(string text, int x, int y, int justify, SDL_Surface *target, int color) {
	int dest_x = -1;
	int dest_y = -1;

	// calculate actual starting x,y based on justify
	if (justify == JUSTIFY_LEFT) {
		dest_x = x;
		dest_y = y;
	}
	else if (justify == JUSTIFY_RIGHT) {
		dest_x = x - calc_length(text);
		dest_y = y;
	}
	else if (justify == JUSTIFY_CENTER) {
		dest_x = x - calc_length(text)/2;
		dest_y = y;
	}
	else {
		printf("ERROR: FontEngine::render() given unhandled 'justify=%d', assuming left\n",
		       justify);
		dest_x = x;
		dest_y = y;
	}

	// render and blit the text
	SDL_Rect dest;
	dest.x = dest_x;
	dest.y = dest_y;

	const char* char_text = text.c_str(); //makes it so SDL_ttf functions can read it

	ttf = TTF_RenderUTF8_Blended(font, char_text, colors[color]);

	if (ttf != NULL) SDL_BlitSurface(ttf, NULL, target, &dest);
	SDL_FreeSurface(ttf);
	ttf = NULL;
}

/**
 * Word wrap to width
 */
void FontEngine::render(string text, int x, int y, int justify, SDL_Surface *target, int width, int color) {

	cursor_y = y;
	string segment;
	string fulltext;
	string builder = "";
	string builder_prev = "";
	char space = 32;
	
	fulltext = text + " ";
	segment = eatFirstString(fulltext, space);
	
	
	while(segment != "" || fulltext.length() > 0) { // don't exit early on double spaces
		builder = builder + segment;
		
		if (calc_length(builder) > width) {
			render(builder_prev, x, cursor_y, justify, target, color);
			cursor_y += getLineHeight();
			builder_prev = "";
			builder = segment + " ";
		}
		else {
			builder = builder + " ";
			builder_prev = builder;
		}
		
		segment = eatFirstString(fulltext, space);
	}

	render(builder, x, cursor_y, justify, target, color);
	cursor_y += getLineHeight();

}

void FontEngine::renderShadowed(string text, int x, int y, int justify, SDL_Surface *target, int color) {
	render(text, x+1, y+1, justify, target, FONT_BLACK);
	render(text, x, y, justify, target, color);
}

void FontEngine::renderShadowed(string text, int x, int y, int justify, SDL_Surface *target, int width, int color) {
	render(text, x+1, y+1, justify, target, width, FONT_BLACK);
	render(text, x, y, justify, target, width, color);
}

FontEngine::~FontEngine() {
	SDL_FreeSurface(ttf);
	TTF_CloseFont(font);
	TTF_Quit();
}

