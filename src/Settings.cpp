/**
 * Settings
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#include "Settings.h"
#include <fstream>
#include <string>
#include "UtilsParsing.h"

using namespace std;

// Tile Settings
int UNITS_PER_TILE = 64;
int TILE_SHIFT = 6; // for fast bitshift divides
int UNITS_PER_PIXEL_X = 2;
int UNITS_PER_PIXEL_Y = 4;
int TILE_W = 64;
int TILE_H = 32;
int TILE_W_HALF = TILE_W/2;
int TILE_H_HALF = TILE_H/2;

// Video Settings
bool FULLSCREEN = false;
int FRAMES_PER_SEC = 30;
int VIEW_W = 720;
int VIEW_H = 480;
int VIEW_W_HALF = VIEW_W/2;
int VIEW_H_HALF = VIEW_H/2;
bool DOUBLEBUF = false;
bool HWSURFACE = false;

// Audio Settings
int MUSIC_VOLUME = 64;
int SOUND_VOLUME = 64;
bool MENUS_PAUSE = false;

// Input Settings
bool MOUSE_MOVE = false;

bool loadSettings() {

	ifstream infile;
	string line;
	string key;
	string val;
	string starts_with;
	
	infile.open("config/settings.txt", ios::in);

	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);

			if (line.length() > 0) {
				starts_with = line.at(0);
				
				if (starts_with == "#") {
					// skip comments
				}
				else if (starts_with == "[") {
					// skip headers
				}
				else { // this is data.  treatment depends on key
					parse_key_pair(line, key, val);          
					key = trim(key, ' ');
					val = trim(val, ' ');
				
					if (key == "fullscreen") {
						if (val == "1") FULLSCREEN = true;
					}
					else if (key == "resolution_w") {
						VIEW_W = atoi(val.c_str());
						VIEW_W_HALF = VIEW_W/2;
					}
					else if (key == "resolution_h") {
						VIEW_H = atoi(val.c_str());
						VIEW_H_HALF = VIEW_H/2;
					}
					else if (key == "frames_per_sec") {
						FRAMES_PER_SEC = atoi(val.c_str());
					}
					else if (key == "music_volume") {
						MUSIC_VOLUME = atoi(val.c_str());
					}
					else if (key == "sound_volume") {
						SOUND_VOLUME = atoi(val.c_str());
					}
					else if (key == "mouse_move") {
						if (val == "1") MOUSE_MOVE = true;
					}
					else if (key == "hwsurface") {
						if (val == "1") HWSURFACE = true;
					}
					else if (key == "doublebuf") {
						if (val == "1") DOUBLEBUF = true;
					}
				}
			}
		}
	}
	else {
		return false; // can't find the config file
	}
	infile.close();
	
	return true;
}
