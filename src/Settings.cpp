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

#include <fstream>
#include <cstring>
#include <string>
#include <typeinfo>
using namespace std;

#include "FileParser.h"
#include "Utils.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"

struct ConfigEntry
{
	const char * name;
	const type_info * type;
	const char * default_val;
	void * storage;
	const char * comment;
};

ConfigEntry config[] = {
	{ "fullscreen",	     &typeid(FULLSCREEN),      "0",   &FULLSCREEN,      "fullscreen mode. 1 enable, 0 disable."},
	{ "resolution_w",    &typeid(VIEW_W),          "720", &VIEW_W,          "display resolution. 640x480 minimum. 720x480 recommended."},
	{ "resolution_h",    &typeid(VIEW_H),          "480", &VIEW_H,          NULL},
	{ "music_volume",    &typeid(MUSIC_VOLUME),    "96",  &MUSIC_VOLUME,    "music and sound volume (0 = silent, 128 = max)"},
	{ "sound_volume",    &typeid(SOUND_VOLUME),    "128", &SOUND_VOLUME,    NULL},
	{ "mouse_move",      &typeid(MOUSE_MOVE),      "0",   &MOUSE_MOVE,      "use mouse to move (experimental). 1 enable, 0 disable."},
	{ "hwsurface",       &typeid(HWSURFACE),       "1",   &HWSURFACE,       "hardware surfaces, double buffering. Try disabling for performance. 1 enable, 0 disable."},
	{ "doublebuf",       &typeid(DOUBLEBUF),       "1",   &DOUBLEBUF,       NULL},
	{ "enable_joystick", &typeid(ENABLE_JOYSTICK), "1",   &ENABLE_JOYSTICK, "joystick settings."},
	{ "joystick_device", &typeid(JOYSTICK_DEVICE), "0",   &JOYSTICK_DEVICE, NULL},
	{ "language",        &typeid(LANGUAGE),        "en",  &LANGUAGE,        "2-letter language code."}
};
const int config_size = sizeof(config) / sizeof(ConfigEntry);

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
bool FULLSCREEN;
int FRAMES_PER_SEC = 30;
int VIEW_W;
int VIEW_H;
int VIEW_W_HALF = VIEW_W/2;
int VIEW_H_HALF = VIEW_H/2;
bool DOUBLEBUF;
bool HWSURFACE;

// Audio Settings
int MUSIC_VOLUME;
int SOUND_VOLUME;

// Input Settings
bool MOUSE_MOVE;
bool ENABLE_JOYSTICK;
int JOYSTICK_DEVICE;

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
// Windows paths
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
#elif __amigaos4__
// AmigaOS paths
void setPaths() {
	PATH_CONF = "PROGDIR:";
	PATH_USER = "PROGDIR:";
	PATH_DATA = "PROGDIR:";
}
#else
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
	// in most releases the data will be in the same folder as the executable
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

#if defined DATA_INSTALL_DIR
	PATH_DATA = DATA_INSTALL_DIR "/";
	if (dirExists(PATH_DATA)) return; // NOTE: early exit
#endif
	
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

static ConfigEntry * getConfigEntry(const char * name) {

	for (int i = 0; i < config_size; i++) {
		if (std::strcmp(config[i].name, name) == 0) return config + i;
	}
	return NULL;
}

static ConfigEntry * getConfigEntry(const std::string & name) {
	return getConfigEntry(name.c_str());
}

bool loadSettings() {

	// init defaults
	for (int i = 0; i < config_size; i++) {
		// TODO: handle errors
		ConfigEntry * entry = config + i;
		tryParseValue(*entry->type, entry->default_val, entry->storage);
	}

	// try read from file
	FileParser infile;
	if (infile.open(PATH_CONF + "settings.txt")) {

		while (infile.next()) {

			ConfigEntry * entry = getConfigEntry(infile.key);
			if (entry) {
				// TODO: handle errors
				tryParseValue(*entry->type, infile.val, entry->storage);
			}
		}
		infile.close();
	}
	else {
		saveSettings(); // write the default settings
	}

	// Init automatically calculated parameters
	VIEW_W_HALF = VIEW_W / 2;
	VIEW_H_HALF = VIEW_H / 2;

	return true;
}

/**
 * Save the current main settings (primary video and audio settings)
 */
bool saveSettings() {

	ofstream outfile;
	outfile.open((PATH_CONF + "settings.txt").c_str(), ios::out);

	if (outfile.is_open()) {
	
		for (int i = 0; i < config_size; i++) {

			// write additional newline before the next section
			if (i != 0 && config[i].comment != NULL)
				outfile<<"\n";

			if (config[i].comment != NULL) {
				outfile<<"# "<<config[i].comment<<"\n";
			}
			outfile<<config[i].name<<"="<<toString(*config[i].type, config[i].storage)<<"\n";
		}

		outfile.close();
	}
	return true;
}
