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
 * class MenuMiniMap
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuMiniMap.h"
#include "MapCollision.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsParsing.h"

#include <iostream>
using namespace std;

MenuMiniMap::MenuMiniMap() {

	map_surface = 0;
	createMapSurface();
	color_wall = SDL_MapRGB(map_surface->format, 128,128,128);
	color_obst = SDL_MapRGB(map_surface->format, 64,64,64);
	color_hero = SDL_MapRGB(map_surface->format, 255,255,255);

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/minimap.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "pos") {
				pos.x = eatFirstInt(infile.val,',');
				pos.y = eatFirstInt(infile.val,',');
				pos.w = eatFirstInt(infile.val,',');
				pos.h = eatFirstInt(infile.val,',');
			} else if(infile.key == "text_pos") {
				text_pos = eatLabelInfo(infile.val);
			}
		}
		infile.close();
	}

	// label for map name
	label = new WidgetLabel();

}

void MenuMiniMap::getMapTitle(std::string map_title) {
	label->set(window_area.x+text_pos.x, window_area.y+text_pos.y, text_pos.justify, text_pos.valign, map_title, font->getColor("menu_normal"), text_pos.font_style);
}

void MenuMiniMap::createMapSurface() {

	SDL_FreeSurface(map_surface);
	map_surface = createSurface(512, 512);
}

void MenuMiniMap::render() {
}

void MenuMiniMap::render(Point hero_pos) {
	if (!text_pos.hidden) label->render();

	if (TILESET_ORIENTATION == TILESET_ISOMETRIC)
		renderIso(hero_pos);
	else // TILESET_ORTHOGONAL
		renderOrtho(hero_pos);
}

void MenuMiniMap::prerender(MapCollision *collider, int map_w, int map_h) {
	map_size.x = map_w;
	map_size.y = map_h;
	SDL_FillRect(map_surface, 0, SDL_MapRGB(map_surface->format,255,0,255));

	if (TILESET_ORIENTATION == TILESET_ISOMETRIC)
		prerenderIso(collider);
	else // TILESET_ORTHOGONAL
		prerenderOrtho(collider);
}

/**
 * Render a top-down version of the map (90 deg angle)
 */
void MenuMiniMap::renderOrtho(Point hero_pos) {

	const int herox = hero_pos.x / UNITS_PER_TILE;
	const int heroy = hero_pos.y / UNITS_PER_TILE;

	SDL_Rect clip;
	clip.x = herox - pos.w/2;
	clip.y = heroy - pos.h/2;
	clip.w = pos.w;
	clip.h = pos.h;

	SDL_Rect map_area;
	map_area.x = window_area.x + pos.x;
	map_area.y = window_area.y + pos.y;
	map_area.w = pos.w;
	map_area.h = pos.h;

	SDL_BlitSurface(map_surface, &clip ,screen, &map_area);

	SDL_LockSurface(screen);
	drawPixel(screen, window_area.x + pos.x + pos.w/2, window_area.y + pos.y + pos.h/2, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2 + 1, window_area.y + pos.y + pos.h/2, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2 - 1, window_area.y + pos.y + pos.h/2, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2, window_area.y + pos.y + pos.h/2 + 1, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2, window_area.y + pos.y + pos.h/2 - 1, color_hero);
	SDL_UnlockSurface(screen);
}

/**
 * Render an "isometric" version of the map (45 deg angle)
 */
void MenuMiniMap::renderIso(Point hero_pos) {

	const int herox = hero_pos.x / UNITS_PER_TILE;
	const int heroy = hero_pos.y / UNITS_PER_TILE;
	const int heroy_screen = herox + heroy;
	const int herox_screen = herox - heroy + std::max(map_size.x, map_size.y);

	SDL_Rect clip;
	clip.x = herox_screen - pos.w/2;
	clip.y = heroy_screen - pos.h/2;
	clip.w = pos.w;
	clip.h = pos.h;

	SDL_Rect map_area;
	map_area.x = window_area.x + pos.x;
	map_area.y = window_area.y + pos.y;
	map_area.w = pos.w;
	map_area.h = pos.h;

	SDL_BlitSurface(map_surface, &clip ,screen, &map_area);
	SDL_LockSurface(screen);
	drawPixel(screen, window_area.x + pos.x + pos.w/2 + 1, window_area.y + pos.y + pos.h/2, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2 - 1, window_area.y + pos.y + pos.h/2, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2, window_area.y + pos.y + pos.h/2 + 1, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2, window_area.y + pos.y + pos.h/2 - 1, color_hero);
	drawPixel(screen, window_area.x + pos.x + pos.w/2, window_area.y + pos.y + pos.h/2, color_hero);
	SDL_UnlockSurface(screen);
}

void MenuMiniMap::prerenderOrtho(MapCollision *collider) {
	for (int i=0; i<std::min(map_surface->w, map_size.x); i++) {
		for (int j=0; j<std::min(map_surface->h, map_size.y); j++) {
			if (collider->colmap[i][j] == 1) {
				drawPixel(map_surface, i, j, color_wall);
			}
			else if (collider->colmap[i][j] == 2) {
				drawPixel(map_surface, i, j, color_obst);
			}
		}
	}
}

void MenuMiniMap::prerenderIso(MapCollision *collider) {
	// a 2x1 pixel area correlates to a tile, so we can traverse tiles using pixel counting
	Uint32 draw_color;
	int tile_type;

	Point tile_cursor;
	tile_cursor.x = -std::max(map_size.x, map_size.y)/2;
	tile_cursor.y = std::max(map_size.x, map_size.y)/2;

	bool odd_row = false;

	// for each pixel row
	for (int j=0; j<map_surface->h; j++) {

		// for each 2-px wide column
		for (int i=0; i<map_surface->w; i+=2) {

			// if this tile is the max map size
			if (tile_cursor.x >= 0 && tile_cursor.y >= 0 && tile_cursor.x < map_size.x && tile_cursor.y < map_size.y) {

				tile_type = collider->colmap[tile_cursor.x][tile_cursor.y];
				bool draw_tile = true;

				// walls and low obstacles show as different colors
				if (tile_type == 1) draw_color = color_wall;
				else if (tile_type == 2) draw_color = color_obst;
				else draw_tile = false;

				if (draw_tile) {
					if (odd_row) {
						drawPixel(map_surface, i, j, draw_color);
						drawPixel(map_surface, i+1, j, draw_color);
					}
					else {
						drawPixel(map_surface, i-1, j, draw_color);
						drawPixel(map_surface, i, j, draw_color);
					}
				}
			}

			// moving screen-right in isometric is +x -y in map coordinates
			tile_cursor.x++;
			tile_cursor.y--;
		}

		// return tile cursor to next row of tiles
		if (odd_row) {
			odd_row = false;
			tile_cursor.x -= map_surface->w/2;
			tile_cursor.y += (map_surface->w/2 +1);
		}
		else {
			odd_row = true;
			tile_cursor.x -= (map_surface->w/2 -1);
			tile_cursor.y += map_surface->w/2;
		}
	}
}

MenuMiniMap::~MenuMiniMap() {
	SDL_FreeSurface(map_surface);
	delete label;
}
