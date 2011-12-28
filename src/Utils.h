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
 * Utils
 *
 * Various utility structures, enums, function
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "math.h"
#include "Settings.h"

using namespace std;

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

struct Event_Component {
	string type;
	string s;
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
Point map_to_collision(Point p);
Point collision_to_map(Point p);
FPoint calcVector(Point pos, int direction, int dist);
double calcDist(Point p1, Point p2);
bool isWithin(Point center, int radius, Point target);
bool isWithin(SDL_Rect r, Point target);
void zsort(Renderable r[], int rnum);
void sort_by_tile(Renderable r[], int rnum);
void drawPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x0, int y0, int x1, int y1, Uint32 color);
void drawLine(SDL_Surface *screen, Point pos0, Point pos1, Uint32 color);
SDL_Surface* createSurface(int width, int height);

/**
 * As implemented here:
 * http://www.algolist.net/Algorithms/Sorting/Bubble_sort
 */
template <typename T>
void bubbleSort(T arr[], int n) {
      bool swapped = true;
      int j = 0;
      T tmp;
      while (swapped) {
            swapped = false;
            j++;
            for (int i = 0; i < n - j; i++) {
                  if (arr[i] > arr[i + 1]) {
                        tmp = arr[i];
                        arr[i] = arr[i + 1];
                        arr[i + 1] = tmp;
                        swapped = true;
                  }
            }
      }
}

template <typename T>
void remove(T arr[], int &n, int index) {
	for (int i=index; i<n-1; i++) {
		arr[i] = arr[i+1];
	}
	n--;
}

template <typename T>
void removeDupes(T arr[], int &n) {
	int i = n;
	while (i>0) {
		if (arr[i] == arr[i-1]) {
			remove(arr, n, i);
		}
		i--;
	}
}
#endif
