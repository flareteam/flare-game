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
 * Utils
 *
 * Various utility structures, enums, function
 */

#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

struct Point {
	int x,y;
};

struct FPoint {
	float x,y;
};

// message passing struct for various sprites rendered map inline
struct Renderable {
	Point map_pos;
	SDL_Surface *sprite;
	SDL_Rect src;
	Point offset;
	bool object_layer;
	Point tile;
};

class Event_Component {
public:
	std::string type;
	std::string s;
	int x;
	int y;
	int z;

	Event_Component() {
		type = s = "";
		x = y = z = 0;
	}
};

// Utility Functions
int round(float f);
Point round(FPoint fp);
Point screen_to_map(int x, int y, int camx, int camy);
Point map_to_screen(int x, int y, int camx, int camy);
Point center_tile(Point p);
Point map_to_collision(Point p);
Point collision_to_map(Point p);
FPoint calcVector(Point pos, int direction, int dist);
double calcDist(Point p1, Point p2);
bool isWithin(Point center, int radius, Point target);
bool isWithin(SDL_Rect r, Point target);
void drawPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x0, int y0, int x1, int y1, Uint32 color);
void drawLine(SDL_Surface *screen, Point pos0, Point pos1, Uint32 color);

/**
 * Creates a SDL_Surface.
 * The SDL_HWSURFACE or SDL_SWSURFACE flag is set according
 * to settings. The result is a surface which has the same format as the
 * screen surface.
 * Additionally the alpha flag is set, so transparent blits are possible.
 */
SDL_Surface* createAlphaSurface(int width, int height);

/**
 * Creates a SDL_Surface.
 * The SDL_HWSURFACE or SDL_SWSURFACE flag is set according
 * to settings. The result is a surface which has the same format as the
 * screen surface.
 * The bright pink (rgb 0xff00ff) is set as transparent color.
 */
SDL_Surface* createSurface(int width, int height);
#endif
