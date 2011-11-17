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
 * Settings
 */
 
#include "Settings.h"
#include <fstream>
#include <string>
#include "FileParser.h"
#include "Utils.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"

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
bool ENABLE_JOYSTICK = true;
int JOYSTICK_DEVICE = 0;

// Language Settings
std::string LANGUAGE = "en";

// Other Settings
bool MENUS_PAUSE = false;


/**
 * Set system paths
 * PATH_CONF is for user-configurable settings files (e.g. keybindings)
 * PATH_USER is for user-specific data (e.g. save games)
 * PATH_DATA is for common game data (e.g. images, music)
 */
#ifdef _WIN32
void setPaths() {

	// handle Windows-specific path options
	PATH_CONF = "config";
	PATH_USER = "saves";
	PATH_DATA = "";
	
	// TODO: place config and save data in the user's home, windows style
	createDir(PATH_CONF);
	createDir(PATH_USER);
	
	PATH_CONF = PATH_CONF + "/";
	PATH_USER = PATH_USER + "/";
}
#endif
#ifndef _WIN32
void setPaths() {

	string engine_folder = "flare";
	
	// attempting to follow this spec:
	// http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	
	// set config path (settings, keybindings)
	// $XDG_CONFIG_HOME/flare/
	if (getenv("XDG_CONFIG_HOME") != NULL) {
		PATH_CONF = (string)getenv("XDG_CONFIG_HOME") + "/" + engine_folder + "/";
		createDir(PATH_CONF);
	}
	// $HOME/.config/flare/
	else if (getenv("HOME") != NULL) {
		PATH_CONF = (string)getenv("HOME") + "/.config/";
		createDir(PATH_CONF);
		PATH_CONF += engine_folder + "/";
		createDir(PATH_CONF);		
	}
	// ./config/
	else {
		PATH_CONF = "./config/";
		createDir(PATH_CONF);		
	}

	// set user path (save games)
	// $XDG_DATA_HOME/flare/
	if (getenv("XDG_DATA_HOME") != NULL) {
		PATH_USER = (string)getenv("XDG_DATA_HOME") + "/" + engine_folder + "/";
		createDir(PATH_USER);
	}
	// $HOME/.local/share/flare/
	else if (getenv("HOME") != NULL) {
		PATH_USER = (string)getenv("HOME") + "/.local/";
		createDir(PATH_USER);
		PATH_USER += "share/";
		createDir(PATH_USER);
		PATH_USER += engine_folder + "/";
		createDir(PATH_USER);
	}
	// ./saves/
	else {
		PATH_USER = "./saves/";
		createDir(PATH_USER);	
	}
	
	// data folder
	// while PATH_CONF and PATH_USER are created if not found,
	// PATH_DATA must already have the game data for the game to work.
	// in most released the data will be in the same folder as the executable
	// - Windows apps are released as a simple folder
	// - OSX apps are released in a .app folder
	// Official linux distros might put the executable and data files
	// in a more standard location.

	// NOTE: from here on out, the function exits early when the data dir is found

	// check $XDG_DATA_DIRS options first
	// a list of directories in preferred order separated by :
	if (getenv("XDG_DATA_DIRS") != NULL) {
		string pathlist = (string)getenv("XDG_DATA_DIRS");
		string pathtest;
		pathtest = eatFirstString(pathlist,':');
		while (pathtest != "") {
			PATH_DATA = pathtest + "/" + engine_folder + "/";
			if (dirExists(PATH_DATA)) return; // NOTE: early exit
			pathtest = eatFirstString(pathlist,':');
		}
	}
	
	// check /usr/local/share/flare/ and /usr/share/flare/ next
	PATH_DATA = "/usr/local/share/" + engine_folder + "/";
	if (dirExists(PATH_DATA)) return; // NOTE: early exit
	
	PATH_DATA = "/usr/share/" + engine_folder + "/";
	if (dirExists(PATH_DATA)) return; // NOTE: early exit
	
	// check "games" variants of these
	PATH_DATA = "/usr/local/share/games/" + engine_folder + "/";
	if (dirExists(PATH_DATA)) return; // NOTE: early exit
	
	PATH_DATA = "/usr/share/games/" + engine_folder + "/";
	if (dirExists(PATH_DATA)) return; // NOTE: early exit

	// finally assume the local folder
	PATH_DATA = "./";
}
#endif


bool loadSettings() {

	FileParser infile;
	
	if (infile.open(PATH_CONF + "settings.txt")) {
		while (infile.next()) {
			if (infile.key == "fullscreen") {
				if (infile.val == "1") FULLSCREEN = true;
				else FULLSCREEN = false;
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
				else HWSURFACE = false;
			}
			else if (infile.key == "doublebuf") {
				if (infile.val == "1") DOUBLEBUF = true;
				else DOUBLEBUF = false;
			}
			else if (infile.key == "frames_per_sec") {
				FRAMES_PER_SEC = atoi(infile.val.c_str());
			}
			else if (infile.key == "enable_joystick") {
				if (infile.val == "1") ENABLE_JOYSTICK = true;
				else ENABLE_JOYSTICK = false;
			}
			else if (infile.key == "joystick_device") {
				JOYSTICK_DEVICE = atoi(infile.val.c_str());
			}
			else if (infile.key == "language") {
				LANGUAGE = infile.val.c_str();
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
		outfile << "enable_joystick=" << ENABLE_JOYSTICK << "\n";
		outfile << "joystick_device=" << JOYSTICK_DEVICE << "\n";
		outfile << "language=" << LANGUAGE << "\n";

		outfile.close();
	}
	return true;
}
