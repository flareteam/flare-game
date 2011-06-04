/**
 * class MenuMiniMap
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_MINI_MAP_H
#define MENU_MINI_MAP_H

#include "SDL.h"
#include "Utils.h"
#include "MapCollision.h"

class MenuMiniMap {
private:
	SDL_Surface *screen;
	Uint32 color_wall;
	Uint32 color_obst;
	Uint32 color_hero;
public: 
	MenuMiniMap(SDL_Surface *_screen);
	~MenuMiniMap();

	void render(MapCollision *collider, Point hero_pos, int map_w, int map_h);

};


#endif
