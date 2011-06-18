/**
 * class TileSet
 *
 * TileSet storage and file loading
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#ifndef TILE_SET_H
#define TILE_SET_H

#include <iostream>
#include <fstream>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "Utils.h"

using namespace std;


struct Tile_Def {
	SDL_Rect src;
	Point offset;
};

class TileSet {
private:
	void loadGraphics(string filename);
	
	string current_map;
public:
	// functions
	TileSet();
	~TileSet();
	void load(string filename);
	
	Tile_Def tiles[256];
	SDL_Surface *sprites;


};

#endif
