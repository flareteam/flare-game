/*
Copyright 2011 Clint Bellanger

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

/**
 * class TileSet
 *
 * TileSet storage and file loading
 */
 
#include "TileSet.h"
#include "UtilsParsing.h"
#include "ModManager.h"

TileSet::TileSet() {
	sprites = NULL;
	for (int i=0; i<256; i++) {
		tiles[i].src.x = 0;
		tiles[i].src.y = 0;
		tiles[i].src.w = 0;
		tiles[i].src.h = 0;
		tiles[i].offset.x = 0;
		tiles[i].offset.y = 0;
	}
}

void TileSet::loadGraphics(string filename) {
	if (sprites) SDL_FreeSurface(sprites);
	
	sprites = IMG_Load((mods->locate("images/tilesets/" + filename)).c_str());
	if(!sprites) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	SDL_SetColorKey( sprites, SDL_SRCCOLORKEY, SDL_MapRGB(sprites->format, 255, 0, 255) ); 
	
	// optimize
	SDL_Surface *cleanup = sprites;
	sprites = SDL_DisplayFormatAlpha(sprites);
	SDL_FreeSurface(cleanup);	
}

void TileSet::load(string filename) {
	if (current_map == filename) return;
	
	ifstream infile;
	string line;
	unsigned short index;

	infile.open((mods->locate("tilesetdefs/" + filename)).c_str(), ios::in);

	if (infile.is_open()) {
		string img;
		
		// first line is the tileset image filename
		line = getLine(infile);
		
		img = line;

		while (!infile.eof()) {
			line = getLine(infile);

			if (line.length() > 0) {
				line = line + ',';

				// split across comma
				// line contains:
				// index, x, y, w, h, ox, oy

				index = eatFirstHex(line, ',');
				tiles[index].src.x = eatFirstInt(line, ',');
				tiles[index].src.y = eatFirstInt(line, ',');
				tiles[index].src.w = eatFirstInt(line, ',');
				tiles[index].src.h = eatFirstInt(line, ',');
				tiles[index].offset.x = eatFirstInt(line, ',');
				tiles[index].offset.y = eatFirstInt(line, ',');
			}
		}

		infile.close();
		loadGraphics(img);
	}

	current_map = filename;
}

TileSet::~TileSet() {
	SDL_FreeSurface(sprites);
}
