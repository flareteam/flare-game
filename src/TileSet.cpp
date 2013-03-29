/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller

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
#include "SharedResources.h"
#include "FileParser.h"
#include "UtilsParsing.h"
#include "Settings.h"

#include <cstdio>
#include <iostream>

using namespace std;

TileSet::TileSet() {
	sprites = NULL;
	reset();
}

void TileSet::reset() {

	SDL_FreeSurface(sprites);

	alpha_background = true;
	trans_r = 255;
	trans_g = 0;
	trans_b = 255;

	sprites = NULL;
	tiles.clear();
	anim.clear();

	max_size_x = 0;
	max_size_y = 0;
}

void TileSet::loadGraphics(const std::string& filename) {
	if (sprites)
		SDL_FreeSurface(sprites);

	if (!TEXTURE_QUALITY)
		sprites = loadGraphicSurface("images/tilesets/noalpha/" + filename, "Couldn't load image", false, true);

	if (!sprites)
		sprites = loadGraphicSurface("images/tilesets/" + filename);
}

void TileSet::load(const std::string& filename) {
	if (current_map == filename) return;

	reset();

	FileParser infile;
	string img;

	if (infile.open(mods->locate("tilesetdefs/" + filename))) {
		while (infile.next()) {
			if (infile.key == "tile") {

				infile.val = infile.val + ',';
				unsigned index = eatFirstInt(infile.val, ',');

				if (index >= tiles.size())
					tiles.resize(index + 1);

				tiles[index].src.x = eatFirstInt(infile.val, ',');
				tiles[index].src.y = eatFirstInt(infile.val, ',');
				tiles[index].src.w = eatFirstInt(infile.val, ',');
				tiles[index].src.h = eatFirstInt(infile.val, ',');
				tiles[index].offset.x = eatFirstInt(infile.val, ',');
				tiles[index].offset.y = eatFirstInt(infile.val, ',');
				max_size_x = std::max(max_size_x, (tiles[index].src.w / TILE_W) + 1);
				max_size_y = std::max(max_size_y, (tiles[index].src.h / TILE_H) + 1);
			}
			else if (infile.key == "img") {
				img = infile.val;
			}
			else if (infile.key == "transparency") {
				alpha_background = false;

				infile.val = infile.val + ',';
				trans_r = (Uint8)eatFirstInt(infile.val, ',');
				trans_g = (Uint8)eatFirstInt(infile.val, ',');
				trans_b = (Uint8)eatFirstInt(infile.val, ',');

			}
			else if (infile.key == "animation") {
				int frame = 0;
				unsigned TILE_ID = toInt(infile.nextValue());

				if (TILE_ID >= anim.size())
					anim.resize(TILE_ID + 1);

				string repeat_val = infile.nextValue();
				while (repeat_val != "") {
					anim[TILE_ID].frames++;
					anim[TILE_ID].pos.resize(frame + 1);
					anim[TILE_ID].frame_duration.resize(frame + 1);
					anim[TILE_ID].pos[frame].x = toInt(repeat_val);
					anim[TILE_ID].pos[frame].y = toInt(infile.nextValue());
					anim[TILE_ID].frame_duration[frame] = toInt(infile.nextValue());

					frame++;
					repeat_val = infile.nextValue();
				}
			}
		}
		infile.close();
		loadGraphics(img);
	}  else fprintf(stderr, "Unable to open tilesetdefs/%s!\n", filename.c_str());

	current_map = filename;
}

void TileSet::logic() {

	if (ANIMATED_TILES) {
		for (unsigned i = 0; i < anim.size() ; i++) {
			Tile_Anim &an = anim[i];
			if (!an.frames)
				continue;
			if (an.duration >= an.frame_duration[an.current_frame]) {
				tiles[i].src.x = an.pos[an.current_frame].x;
				tiles[i].src.y = an.pos[an.current_frame].y;
				an.duration = 0;
				an.current_frame = (an.current_frame + 1) % an.frames;
			}
			an.duration++;
		}
	}
}

TileSet::~TileSet() {
	SDL_FreeSurface(sprites);
}
