/*
 * class FontEngine
 * Handles rendering a bitmap font
 *
 * @author Clint Bellanger
 * @license GPL
 *
 */

#include "FontEngine.h"


FontEngine::FontEngine() {

	for (int i=0; i<256; i++) {
		width[i] = 0;
	}
	load();
}



void FontEngine::load() {

	string imgfile;
	string line;
	ifstream infile;
	char str[8];
	
	// load the definition file
	infile.open("fonts/font.txt", ios::in);

	if (infile.is_open()) {
			
		line = getLine(infile);
		font_width = atoi(line.c_str());
		
		line = getLine(infile);
				
		font_height = atoi(line.c_str());
		src.h = font_height;
		dest.h = font_height;
		
		line = getLine(infile);
				
		line_height = atoi(line.c_str());
		
		line = getLine(infile);
			
		kerning = atoi(line.c_str());
		
		// the rest of the file is character pixel widths
		while (!infile.eof()) {
			line = getLine(infile);
			
			if (line.length() > 0) {
				strcpy(str, line.c_str());
				width[(int)str[0]] = line.c_str()[2] - 48;
				
			}
		}
	}
	infile.close();
	
	// load the font images
	sprites[FONT_WHITE] = IMG_Load("fonts/white.png");
	sprites[FONT_RED] = IMG_Load("fonts/red.png");
	sprites[FONT_GREEN] = IMG_Load("fonts/green.png");
	sprites[FONT_BLUE] = IMG_Load("fonts/blue.png");
	sprites[FONT_GRAY] = IMG_Load("fonts/gray.png");
	
}

int FontEngine::calc_length(string text) {
	int size=0;
	char c;
	for (unsigned int i=0; i<text.length(); i++) {
		c = text.c_str()[i];
		size = size + width[(int)c] + kerning;
	}
	size = size - kerning;
	return size;
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
			height = height + line_height;
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
	
	height = height + line_height;
	if (calc_length(builder) > max_width) max_width = calc_length(builder);
				
	Point size;
	size.x = max_width - this->width[32] - kerning; // remove the extra blankspace at the end
	size.y = height;
	return size;

}

/**
 * Render the given text at (x,y) on the target image.
 * Justify is left, right, or center
 */
void FontEngine::render(string text, int x, int y, int justify, SDL_Surface *target, int color) {

	unsigned char c;
	char str[256];
	int dest_x;
	int dest_y;
	
	strcpy(str, text.c_str());

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

	for (unsigned int i=0; i<text.length(); i++) {
	
		// Note, SDL_BlitSurface rewrites dest to show clipping.
		// So we have to remember dest locally.  - cpb 2010/07/03
		dest.x = dest_x;
		dest.y = dest_y;
	
		// set the bounding rect of the char to render
		c = str[i];
		if (c >= 32 && c <= 127) {
			src.x = ((c-32) % 16) * font_width;
			src.y = ((c-32) / 16) * font_height;
			src.w = width[c];
		
			// draw the font
			SDL_BlitSurface(sprites[color], &src, target, &dest);
		
			// move dest
			dest_x = dest_x + width[c] + kerning;
		}
	}
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
			cursor_y += line_height;
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
	cursor_y += line_height;

}


FontEngine::~FontEngine() {
	for (int i=0; i<5; i++)
		SDL_FreeSurface(sprites[i]);
}

