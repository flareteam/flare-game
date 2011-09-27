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
 * class MenuMiniMap
 */


#include "MenuMiniMap.h"

MenuMiniMap::MenuMiniMap(SDL_Surface *_screen) {
	screen = _screen;
	
	color_wall = SDL_MapRGB(screen->format, 128,128,128);
	color_obst = SDL_MapRGB(screen->format, 64,64,64);
	color_hero = SDL_MapRGB(screen->format, 255,255,255);
}

	// TEMP: let's make a minimap

void MenuMiniMap::render(MapCollision *collider, Point hero_pos, int map_w, int map_h) {
	Point hero_tile;
	Point map_tile;
	hero_tile.x = hero_pos.x / UNITS_PER_TILE;
	hero_tile.y = hero_pos.y / UNITS_PER_TILE;
	for (int i=0; i<127; i++) {
		for (int j=0; j<127; j++) {
			map_tile.x = hero_tile.x + i - 64;
			map_tile.y = hero_tile.y + j - 64;
			if (map_tile.x >= 0 && map_tile.x < map_w && map_tile.y >= 0 && map_tile.y < map_h) {
				if (collider->colmap[map_tile.x][map_tile.y] == 1) {
					drawPixel(screen, VIEW_W - 128 + i, 16+j, color_wall);
				}
				else if (collider->colmap[map_tile.x][map_tile.y] == 2) {
					drawPixel(screen, VIEW_W - 128 + i, 16+j, color_obst);
				}
			}
		}
	}
	drawPixel(screen,VIEW_W-64,80,color_hero); // hero
	drawPixel(screen,VIEW_W-64-1,80,color_hero); // hero
	drawPixel(screen,VIEW_W-64+1,80,color_hero); // hero
	drawPixel(screen,VIEW_W-64,80-1,color_hero); // hero	
	drawPixel(screen,VIEW_W-64,80+1,color_hero); // hero

}

MenuMiniMap::~MenuMiniMap() {
}
