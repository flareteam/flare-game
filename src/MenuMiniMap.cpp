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
	
	map_center.x = VIEW_W - 64;
	map_center.y = 80;
	map_area.x = VIEW_W - 128;
	map_area.y = 16;
	map_area.w = map_area.h = 128;

}

/**
 * Render a top-down version of the map (90 deg angle)
 */
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

/**
 * Render an "isometric" version of the map (45 deg angle)
 */
void MenuMiniMap::renderIso(MapCollision *collider, Point hero_pos, int map_w, int map_h) {
	int tile_type;
	Point screen_pos;
	Uint32 draw_color;
	Point hero_tile;
	
	hero_tile.x = hero_pos.x / UNITS_PER_TILE;
	hero_tile.y = hero_pos.y / UNITS_PER_TILE;
	
	for (int j=0; j<map_h; j++) {
		for (int i=0; i<map_w; i++) {
		
			tile_type = collider->colmap[i][j];

			// the hero, walls, and low obstacles show as different colors
			if (i == hero_tile.x && j == hero_tile.y) draw_color = color_hero;
			else if (tile_type == 1) draw_color = color_wall;
			else if (tile_type == 2) draw_color = color_obst;
			else continue; // not visible on mini-map
						
			// isometric transform
			screen_pos.x = (i - hero_tile.x) - (j - hero_tile.y) + map_center.x;
			screen_pos.y = (i - hero_tile.x) + (j - hero_tile.y) + map_center.y;
			
			// each tile is 2 pixels wide to mimic isometric view
			if (isWithin(map_area, screen_pos)) {
				drawPixel(screen, screen_pos.x, screen_pos.y, draw_color);
			}
			screen_pos.x++;
			if (isWithin(map_area, screen_pos)) {
				drawPixel(screen, screen_pos.x, screen_pos.y, draw_color);			
			}
		}
	}
}

MenuMiniMap::~MenuMiniMap() {
}
