#include "Utils.h"
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
	int scrx = x - VIEW_W_HALF;
	int scry = y - VIEW_H_HALF;
	r.x = scrx + scry*2 + camx;
	r.y = scry*2 - scrx + camy;
	return r;
}

Point map_to_screen(int x, int y, int camx, int camy) {
	Point r;
	r.x = VIEW_W_HALF + (x/UNITS_PER_PIXEL_X - camx/UNITS_PER_PIXEL_X) - (y/UNITS_PER_PIXEL_X - camy/UNITS_PER_PIXEL_X);
	r.y = VIEW_H_HALF + (x/UNITS_PER_PIXEL_Y - camx/UNITS_PER_PIXEL_Y) + (y/UNITS_PER_PIXEL_Y - camy/UNITS_PER_PIXEL_Y);
	return r;
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

/**
 * draw pixel to the screen
 */
void drawPixel(SDL_Surface *screen, int x, int y, Uint32 color) {
	Uint32 *pixmem32;
	pixmem32 = (Uint32*) screen->pixels + (y * ((screen->pitch)/4)) + x;
	*pixmem32 = color;
}

