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

#include "Settings.h"
#include "Utils.h"

#include <cmath>

using namespace std;


int round(float f) {
	return (int)(f + 0.5);
}

Point round(FPoint fp) {
	Point result;
	result.x = round(fp.x);
	result.y = round(fp.y);
	return result;
}

// TODO: rewrite using UNITS_PER_PIXEL_X,Y?
Point screen_to_map(int x, int y, int camx, int camy) {
	Point r;
	if (TILESET_ORIENTATION == TILESET_ISOMETRIC) {
		int scrx = x - VIEW_W_HALF;
		int scry = y - VIEW_H_HALF;
		r.x = scrx + scry*2 + camx;
		r.y = scry*2 - scrx + camy;
	}
	else {
		r.x = x - VIEW_W_HALF + camx;
		r.y = y - VIEW_H_HALF + camy;
	}
	return r;
}

Point map_to_screen(int x, int y, int camx, int camy) {
	Point r;
	if (TILESET_ORIENTATION == TILESET_ISOMETRIC) {
		r.x = VIEW_W_HALF + (x - camx - y + camy)/UNITS_PER_PIXEL_X;
		r.y = VIEW_H_HALF + (x - camx + y - camy)/UNITS_PER_PIXEL_Y;
	}
	else { //TILESET_ORTHOGONAL
		r.x = VIEW_W_HALF - (camx - x)/UNITS_PER_PIXEL_X;
		r.y = VIEW_H_HALF - (camy - y)/UNITS_PER_PIXEL_Y;
	}
	return r;
}

Point center_tile(Point p) {
	if (TILESET_ORIENTATION == TILESET_ORTHOGONAL) {
		p.x += TILE_W_HALF;
		p.y += TILE_H_HALF;
	}
	else //TILESET_ISOMETRIC
		p.y += TILE_H_HALF;
	return p;
}

Point collision_to_map(Point p) {
	p.x = (p.x << TILE_SHIFT) + TILE_W_HALF;
	p.y = (p.y << TILE_SHIFT) + TILE_H_HALF;
	return p;
}

Point map_to_collision(Point p) {
	p.x = p.x >> TILE_SHIFT;
	p.y = p.y >> TILE_SHIFT;
	return p;
}

/**
 * Apply parameter distance to position and direction
 */
FPoint calcVector(Point pos, int direction, int dist) {
	FPoint p;
	p.x = (float)(pos.x);
	p.y = (float)(pos.y);

	float dist_straight = (float)dist;
	float dist_diag = ((float)dist) * (float)(0.7071); //  1/sqrt(2)
	
	switch (direction) {
		case 0:
			p.x -= dist_diag;
			p.y += dist_diag;
			break;
		case 1:
			p.x -= dist_straight;
			break;
		case 2:
			p.x -= dist_diag;
			p.y -= dist_diag;
			break;
		case 3:
			p.y -= dist_straight;
			break;
		case 4:
			p.x += dist_diag;
			p.y -= dist_diag;
			break;
		case 5:
			p.x += dist_straight;
			break;
		case 6:
			p.x += dist_diag;
			p.y += dist_diag;
			break;
		case 7:
			p.y += dist_straight;
			break;
	}
	return p;
}

double calcDist(Point p1, Point p2) {
	int x = p2.x - p1.x;
	int y = p2.y - p1.y;
	double step1 = x*x + y*y;
	return sqrt(step1);
}

/**
 * is target within the area defined by center and radius?
 */
bool isWithin(Point center, int radius, Point target) {
	return (calcDist(center, target) < radius);
}

/**
 * is target within the area defined by rectangle r?
 */
bool isWithin(SDL_Rect r, Point target) {
	return target.x >= r.x && target.y >= r.y && target.x < r.x+r.w && target.y < r.y+r.h;
}

/**
 * Sort back-to-front in isometric view
 */
void zsort(Renderable r[], int rnum) {

	int zpos[1024];
	int ztemp;
	Renderable rtemp;
	
	// calculate zpos
	for (int i=0; i<rnum; i++) {
		zpos[i] = r[i].map_pos.x/2 + r[i].map_pos.y/2;
	}
	
	// sort
	// TODO: better sort algo
	for (int i=0; i<rnum; i++) {
		for (int j=0; j<rnum-1; j++) {
			if (zpos[j] > zpos[j+1]) {
				ztemp = zpos[j];
				zpos[j] = zpos[j+1];
				zpos[j+1] = ztemp;
				rtemp = r[j];
				r[j] = r[j+1];
				r[j+1] = rtemp;
			}
		}
	}
	
}

/**
 * Sort in the same order as the tiles are drawn
 * Depends upon the map implementation
 */
void sort_by_tile(Renderable r[], int rnum) {

	// For MapIso the sort order is:
	// tile column first, then tile row.  Within each tile, z-order
	int zpos[1024];
	int ztemp;
	Renderable rtemp;
	
	// prep	
	for (int i=0; i<rnum; i++) {
		// calculate zpos
		zpos[i] = r[i].map_pos.x/2 + r[i].map_pos.y/2;
		// calculate tile
		r[i].tile.x = r[i].map_pos.x >> TILE_SHIFT;
		r[i].tile.y = r[i].map_pos.y >> TILE_SHIFT;
	}
	
	// sort
	for (int i=0; i<rnum; i++) {
		for (int j=0; j<rnum-1; j++) {
		
			// check tile y
			if (r[j].tile.y > r[j+1].tile.y) {
				ztemp = zpos[j];
				zpos[j] = zpos[j+1];
				zpos[j+1] = ztemp;
				rtemp = r[j];
				r[j] = r[j+1];
				r[j+1] = rtemp;
			}
			else if (r[j].tile.y == r[j+1].tile.y) {
			
				// check tile x
				if (r[j].tile.x > r[j+1].tile.x) {
					ztemp = zpos[j];
					zpos[j] = zpos[j+1];
					zpos[j+1] = ztemp;
					rtemp = r[j];
					r[j] = r[j+1];
					r[j+1] = rtemp;
				}
				else if (r[j].tile.x == r[j+1].tile.x) {
				
					// check zpos
					if (zpos[j] > zpos[j+1]) {
						ztemp = zpos[j];
						zpos[j] = zpos[j+1];
						zpos[j+1] = ztemp;
						rtemp = r[j];
						r[j] = r[j+1];
						r[j+1] = rtemp;
					}
				}
			}
			
		}	
	}
	
}


/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 *
 * Source: SDL Documentation
 * http://www.libsdl.org/docs/html/guidevideo.html
 */
void drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}



/**
 * draw line to the screen
 * from http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#Simplification
 */
void drawLine(SDL_Surface *screen, int x0, int y0, int x1, int y1, Uint32 color) {
	const int dx = abs(x1-x0);
	const int dy = abs(y1-y0);
	const int sx = x0 < x1 ? 1 : -1;
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx-dy;

	do {
		//skip draw if outside screen
		if (x0 > 0 && y0 > 0 && x0 < VIEW_W && y0 < VIEW_H)
			drawPixel(screen,x0,y0,color);

		int e2 = 2*err;
		if (e2 > -dy) {
			err = err - dy;
			x0 = x0 + sx;
		}
		if (e2 <  dx) {
			err = err + dx;
			y0 = y0 + sy;
		}
	} while(x0 != x1 || y0 != y1);
}

void drawLine(SDL_Surface *screen, Point pos0, Point pos1, Uint32 color) {
	drawLine(screen, pos0.x, pos0.y, pos1.x, pos1.y, color);
}

/**
 * create blank surface
 * based on example: http://www.libsdl.org/docs/html/sdlcreatergbsurface.html
 */
SDL_Surface* createSurface(int width, int height) {

	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	if (HWSURFACE) 
		surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, width, height, 32, rmask, gmask, bmask, amask);
	else
		surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, width, height, 32, rmask, gmask, bmask, amask);	
	
	if(surface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
	}
	
	// optimize
	SDL_Surface *cleanup = surface;
	surface = SDL_DisplayFormatAlpha(surface);
	SDL_FreeSurface(cleanup);

	return surface;
}


