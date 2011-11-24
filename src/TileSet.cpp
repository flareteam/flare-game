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
#include "SharedResources.h"
#include "FileParser.h"

TileSet::TileSet() {
	alpha_background = false;
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
	
	// only set a color key if the tile set doesn't have an alpha channel
	if (!alpha_background) {
		SDL_SetColorKey( sprites, SDL_SRCCOLORKEY, SDL_MapRGB(sprites->format, 255, 0, 255) ); 
	}
	
	// optimize
	SDL_Surface *cleanup = sprites;
	sprites = SDL_DisplayFormatAlpha(sprites);
	SDL_FreeSurface(cleanup);	
}

void TileSet::load(string filename) {
	if (current_map == filename) return;
	
	alpha_background = false;
	
	FileParser infile;
	unsigned short index;
	string img;

	if (infile.open(mods->locate("tilesetdefs/" + filename))) {
		while (infile.next()) {
			if (infile.key == "tile") {

				infile.val = infile.val + ',';
				index = eatFirstInt(infile.val, ',');
				tiles[index].src.x = eatFirstInt(infile.val, ',');
				tiles[index].src.y = eatFirstInt(infile.val, ',');
				tiles[index].src.w = eatFirstInt(infile.val, ',');
				tiles[index].src.h = eatFirstInt(infile.val, ',');
				tiles[index].offset.x = eatFirstInt(infile.val, ',');
				tiles[index].offset.y = eatFirstInt(infile.val, ',');
				
			}
			else if (infile.key == "img") {
				img = infile.val;
			}
			else if (infile.key == "alpha_background") {
				if (infile.val == "1") alpha_background = true;
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
