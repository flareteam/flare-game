/**
 * Settings
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#include "Settings.h"
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "FileParser.h"

using namespace std;

// Paths
string PATH_CONF = "";
string PATH_USER = "";
string PATH_DATA = "";

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
bool DOUBLEBUF = true;
bool HWSURFACE = true;

// Audio Settings
int MUSIC_VOLUME = 96;
int SOUND_VOLUME = 128;

// Input Settings
bool MOUSE_MOVE = false;

// Other Settings
bool MENUS_PAUSE = false;


/**
 * Set system paths
 * PATH_CONF is for user-configurable settings files (e.g. keybindings)
 * PATH_USER is for user-specific data (e.g. save games)
 * PATH_DATA is for common game data (e.g. images, music)
 */
void setPaths() {
	
	// set config path
	if (getenv("XDG_CONFIG_HOME") != NULL) {
		PATH_CONF = (string)getenv("XDG_CONFIG_HOME") + "/flare/";
		mkdir(PATH_CONF.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	else if (getenv("HOME") != NULL) {
		PATH_CONF = (string)getenv("HOME") + "/.config/";
		mkdir(PATH_CONF.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		PATH_CONF += "flare/";
		mkdir(PATH_CONF.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);		
	}
	else {
		PATH_CONF = "./config/";
		mkdir(PATH_CONF.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);		
	}

	// set user/save path
	
	PATH_USER = "./";
	PATH_DATA = "./";
	
}

bool loadSettings() {

	FileParser infile;
	
	if (infile.open(PATH_CONF + "settings.txt")) {
		while (infile.next()) {
			if (infile.key == "fullscreen") {
				if (infile.val == "1") FULLSCREEN = true;
			}
			else if (infile.key == "resolution_w") {
				VIEW_W = atoi(infile.val.c_str());
				VIEW_W_HALF = VIEW_W/2;
			}
			else if (infile.key == "resolution_h") {
				VIEW_H = atoi(infile.val.c_str());
				VIEW_H_HALF = VIEW_H/2;
			}
			else if (infile.key == "music_volume") {
				MUSIC_VOLUME = atoi(infile.val.c_str());
			}
			else if (infile.key == "sound_volume") {
				SOUND_VOLUME = atoi(infile.val.c_str());
			}
			else if (infile.key == "mouse_move") {
				if (infile.val == "1") MOUSE_MOVE = true;
			}
			else if (infile.key == "hwsurface") {
				if (infile.val == "1") HWSURFACE = true;
			}
			else if (infile.key == "doublebuf") {
				if (infile.val == "1") DOUBLEBUF = true;
			}
			else if (infile.key == "frames_per_sec") {
				FRAMES_PER_SEC = atoi(infile.val.c_str());
			}
		}
		infile.close();
		return true;
	}
	else {
		saveSettings(); // write the default settings
	}
	return true;
}

/**
 * Save the current main settings (primary video and audio settings)
 */
bool saveSettings() {

	ofstream outfile;
	outfile.open((PATH_CONF + "settings.txt").c_str(), ios::out);

	if (outfile.is_open()) {
	
		outfile << "fullscreen=" << FULLSCREEN << "\n";
		outfile << "resolution_w=" << VIEW_W << "\n";
		outfile << "resolution_h=" << VIEW_H << "\n";
		outfile << "music_volume=" << MUSIC_VOLUME << "\n";
		outfile << "sound_volume=" << SOUND_VOLUME << "\n";
		outfile << "mouse_move=" << MOUSE_MOVE << "\n";
		outfile << "hwsurface=" << HWSURFACE << "\n";
		outfile << "doublebuf=" << DOUBLEBUF << "\n";
		outfile << "frames_per_sec=" << FRAMES_PER_SEC << "\n";

		outfile.close();
	}
	return true;
}
