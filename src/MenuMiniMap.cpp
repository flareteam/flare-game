/*
Copyright © 2011-2012 Clint Bellanger

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


#include "Menu.h"
#include "MenuMiniMap.h"
#include "MapCollision.h"
#include "SharedResources.h"
#include "Settings.h"

MenuMiniMap::MenuMiniMap() {
	map_center.x = VIEW_W - 64;
	map_center.y = 80;

	old_hero_tile.x = old_hero_tile.y = 0;

	map_surface = 0;
	createMapSurface();
}

void MenuMiniMap::createMapSurface() {

	map_surface = createSurface(128, 128);

	color_wall = SDL_MapRGB(map_surface->format, 128,128,128);
	color_obst = SDL_MapRGB(map_surface->format, 64,64,64);
	color_hero = SDL_MapRGB(map_surface->format, 255,255,255);
}

void MenuMiniMap::render() {
}

void MenuMiniMap::render(MapCollision *collider, Point hero_pos, int map_w, int map_h) {

	if (TILESET_ORIENTATION == TILESET_ISOMETRIC)
		renderIso(collider, hero_pos, map_w, map_h);
	else // TILESET_ORTHOGONAL
		renderOrtho(collider, hero_pos, map_w, map_h);
}

/**
 * Render a top-down version of the map (90 deg angle)
 */
void MenuMiniMap::renderOrtho(MapCollision *collider, Point hero_pos, int map_w, int map_h) {

	map_area.x = window_area.x;
	map_area.y = window_area.y;
	map_area.w = map_area.h = window_area.w;

	SDL_LockSurface(screen);

	Point hero_tile;
	Point map_tile;
	hero_tile.x = hero_pos.x / UNITS_PER_TILE;
	hero_tile.y = hero_pos.y / UNITS_PER_TILE;
	for (int i=0; i<map_area.w; i++) {
		for (int j=0; j<map_area.h; j++) {
			map_tile.x = hero_tile.x + i - map_area.w/2;
			map_tile.y = hero_tile.y + j - map_area.h/2;
			if (map_tile.x >= 0 && map_tile.x < map_w && map_tile.y >= 0 && map_tile.y < map_h) {
				if (collider->colmap[map_tile.x][map_tile.y] == 1) {
					drawPixel(screen, map_area.x+i, map_area.y+j, color_wall);
				}
				else if (collider->colmap[map_tile.x][map_tile.y] == 2) {
					drawPixel(screen, map_area.x+i, map_area.y+j, color_obst);
				}
			}
		}
	}
	drawPixel(screen,window_area.x+64,80,color_hero); // hero
	drawPixel(screen,window_area.x+64-1,80,color_hero); // hero
	drawPixel(screen,window_area.x+64+1,80,color_hero); // hero
	drawPixel(screen,window_area.x+64,80-1,color_hero); // hero
	drawPixel(screen,window_area.x+64,80+1,color_hero); // hero

	SDL_UnlockSurface(screen);

}

/**
 * Render an "isometric" version of the map (45 deg angle)
 */
void MenuMiniMap::renderIso(MapCollision *collider, Point hero_pos, int map_w, int map_h) {

	map_area.x = window_area.x;
	map_area.y = window_area.y;
	map_area.w = map_area.h = window_area.w;

	int tile_type;
	Uint32 draw_color;
	Point hero_tile;

	hero_tile.x = hero_pos.x / UNITS_PER_TILE;
	hero_tile.y = hero_pos.y / UNITS_PER_TILE;

	if (hero_tile.x == old_hero_tile.x && hero_tile.y == old_hero_tile.y) {
		SDL_BlitSurface(map_surface, 0 ,screen, &window_area);
		return;
	}

	old_hero_tile = hero_tile;
	SDL_FillRect(map_surface, 0, SDL_MapRGB(map_surface->format,255,0,255));

	// half the width of the minimap is used in several calculations
	// a 2x1 pixel area correlates to a tile, so we can traverse tiles using pixel counting
	// This is also the number of tiles we'll draw per screen row (the i loop below)
	int minimap_half = map_area.w/2;

	// Because the minimap is always 128x128px, we know which tile represents the corner of the map
	Point tile_cursor;
	tile_cursor.x = hero_tile.x - minimap_half;
	tile_cursor.y = hero_tile.y;

	int map_end_x = map_area.x + map_area.w;
	int map_end_y = map_area.y + map_area.h;

	bool odd_row = false;

	// for each pixel row
	for (int j=0; j<128; j++) {

		// for each 2-px wide column
		for (int i=0; i<128; i+=2) {

			// if this tile is the max map size
			if (tile_cursor.x >= 0 && tile_cursor.y >= 0 && tile_cursor.x < map_w && tile_cursor.y < map_h) {

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
			tile_cursor.x -= minimap_half;
			tile_cursor.y += (minimap_half +1);
		}
		else {
			odd_row = true;
			tile_cursor.x -= (minimap_half -1);
			tile_cursor.y += minimap_half;
		}
	}
	// the hero
	drawPixel(map_surface, 64, 64, color_hero);
	drawPixel(map_surface, 65, 64, color_hero);

	SDL_BlitSurface(map_surface, 0 ,screen, &window_area);
}

MenuMiniMap::~MenuMiniMap() {
	SDL_FreeSurface(map_surface);
}
