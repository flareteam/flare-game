/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Igor Paliychuk
Copyright © 2012 Stefan Beller

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
#include <cmath>
using namespace std;

#include "FileParser.h"
#include "Settings.h"
#include "Utils.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"
#include "SharedResources.h"

#ifdef _MSC_VER
#define log2(x)	logf(x)/logf(2)
#endif

class ConfigEntry
{
public:
	const char * name;
	const type_info * type;
	const char * default_val;
	void * storage;
	const char * comment;
};

ConfigEntry config[] = {
	{ "fullscreen",       &typeid(FULLSCREEN),      "0",   &FULLSCREEN,      "fullscreen mode. 1 enable, 0 disable."},
	{ "resolution_w",     &typeid(VIEW_W),          "640", &VIEW_W,          "display resolution. 640x480 minimum."},
	{ "resolution_h",     &typeid(VIEW_H),          "480", &VIEW_H,          NULL},
	{ "audio",            &typeid(AUDIO),           "1",   &AUDIO,           "Enable music and sound subsystem."},
	{ "music_volume",     &typeid(MUSIC_VOLUME),    "96",  &MUSIC_VOLUME,    "music and sound volume (0 = silent, 128 = max)"},
	{ "sound_volume",     &typeid(SOUND_VOLUME),    "128", &SOUND_VOLUME,    NULL},
	{ "combat_text",      &typeid(COMBAT_TEXT),     "0",   &COMBAT_TEXT,     "display floating damage text. 1 enable, 0 disable."},
	{ "mouse_move",       &typeid(MOUSE_MOVE),      "0",   &MOUSE_MOVE,      "use mouse to move (experimental). 1 enable, 0 disable."},
	{ "hwsurface",        &typeid(HWSURFACE),       "1",   &HWSURFACE,       "hardware surfaces, double buffering. Try disabling for performance. 1 enable, 0 disable."},
	{ "doublebuf",        &typeid(DOUBLEBUF),       "1",   &DOUBLEBUF,       NULL},
	{ "animated_tiles",   &typeid(ANIMATED_TILES),  "1",   &ANIMATED_TILES,  "animated tiles. Try disabling for performance. 1 enable, 0 disable."},
	{ "enable_joystick",  &typeid(ENABLE_JOYSTICK), "0",   &ENABLE_JOYSTICK, "joystick settings."},
	{ "joystick_device",  &typeid(JOYSTICK_DEVICE), "0",   &JOYSTICK_DEVICE, NULL},
	{ "language",         &typeid(LANGUAGE),        "en",  &LANGUAGE,        "2-letter language code."},
	{ "change_gamma",     &typeid(CHANGE_GAMMA),    "0",   &CHANGE_GAMMA,    "allow changing gamma (experimental). 1 enable, 0 disable."},
	{ "gamma",            &typeid(GAMMA),           "1.0", &GAMMA,           "screen gamma (0.5 = darkest, 2.0 = lightest)"},
	{ "texture_quality",  &typeid(TEXTURE_QUALITY), "1",   &TEXTURE_QUALITY, "texture quality (0 = low quality, 1 = high quality)"},
	{ "mouse_aim",        &typeid(MOUSE_AIM),       "1",   &MOUSE_AIM,       "use mouse to aim. 1 enable, 0 disable."},
	{ "show_fps",         &typeid(SHOW_FPS),        "0",   &SHOW_FPS,        "show frames per second. 1 enable, 0 disable."}
};
const int config_size = sizeof(config) / sizeof(ConfigEntry);

// Paths
string PATH_CONF = "";
string PATH_USER = "";
string PATH_DATA = "";
string USER_PATH_DATA = "";

// Filenames
string FILE_SETTINGS	= "settings.txt";
string FILE_KEYBINDINGS = "keybindings.txt";

// Tile Settings
unsigned short UNITS_PER_TILE = 64;
unsigned short TILE_SHIFT = 6; // for fast bitshift divides
unsigned short UNITS_PER_PIXEL_X = 2;
unsigned short UNITS_PER_PIXEL_Y = 4;
unsigned short TILE_W = 64;
unsigned short TILE_H = 32;
unsigned short TILE_W_HALF = TILE_W/2;
unsigned short TILE_H_HALF = TILE_H/2;
unsigned short TILESET_ISOMETRIC = 0;
unsigned short TILESET_ORTHOGONAL = 1;
unsigned short TILESET_ORIENTATION = TILESET_ISOMETRIC;

// Main Menu frame size
unsigned short FRAME_W;
unsigned short FRAME_H;

unsigned short ICON_SIZE;

// Video Settings
bool FULLSCREEN;
unsigned short MAX_FRAMES_PER_SEC = 30;
unsigned char BITS_PER_PIXEL = 32;
unsigned short VIEW_W;
unsigned short VIEW_H;
unsigned short VIEW_W_HALF = VIEW_W/2;
unsigned short VIEW_H_HALF = VIEW_H/2;
short MIN_VIEW_W = -1;
short MIN_VIEW_H = -1;
bool DOUBLEBUF;
bool HWSURFACE;
bool CHANGE_GAMMA;
float GAMMA;
bool TEXTURE_QUALITY;
bool ANIMATED_TILES;

// Audio Settings
bool AUDIO;
unsigned short MUSIC_VOLUME;
unsigned short SOUND_VOLUME;

// User Preferences
bool COMBAT_TEXT;

// Input Settings
bool MOUSE_MOVE;
bool ENABLE_JOYSTICK;
int JOYSTICK_DEVICE;
bool MOUSE_AIM;

// Language Settings
std::string LANGUAGE = "en";

// Autopickup Settings
unsigned short AUTOPICKUP_RANGE = 0;
bool AUTOPICKUP_CURRENCY = false;

// Combat calculation caps (percentage)
short MAX_ABSORB = 90;
short MAX_RESIST = 90;
short MAX_BLOCK = 100;
short MAX_AVOIDANCE = 99;

// Elemental types
std::vector<Element> ELEMENTS;

// Hero classes
std::vector<HeroClass> HERO_CLASSES;

// Currency settings
std::string CURRENCY = "Gold";
float VENDOR_RATIO = 0.25;

// Other Settings
bool MENUS_PAUSE = false;
bool SAVE_HPMP = false;
bool ENABLE_PLAYGAME = false;
bool SHOW_FPS = false;
int CORPSE_TIMEOUT = 1800;
bool SELL_WITHOUT_VENDOR = true;
int AIM_ASSIST = 0;
std::string GAME_PREFIX = "";
std::string WINDOW_TITLE = "Flare";
int SOUND_FALLOFF = 15;

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
	if (dirExists(USER_PATH_DATA)) PATH_DATA = USER_PATH_DATA;
	else if (!USER_PATH_DATA.empty()) fprintf(stderr, "Error: Could not find specified game data directory.\n");

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
	if (dirExists(USER_PATH_DATA)) PATH_DATA = USER_PATH_DATA;
	else if (!USER_PATH_DATA.empty()) fprintf(stderr, "Error: Could not find specified game data directory.\n");
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

	// if the user specified a data path, try to use it
	if (dirExists(USER_PATH_DATA)) {
		PATH_DATA = USER_PATH_DATA;
		return;
	}
	else if (!USER_PATH_DATA.empty()) fprintf(stderr, "Error: Could not find specified game data directory.\n");

	// Check for the local data before trying installed ones.
	if (dirExists("./mods")) {
		PATH_DATA = "./";
		return;
	}

	// check $XDG_DATA_DIRS options
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

void loadTilesetSettings() {
	FileParser infile;
	// load tileset settings from engine config
	if (infile.open(mods->locate("engine/tileset_config.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "units_per_tile") {
				UNITS_PER_TILE = toInt(infile.val);
			}
			else if (infile.key == "tile_size") {
				TILE_W = toInt(infile.nextValue());
				TILE_H = toInt(infile.nextValue());
				TILE_W_HALF = TILE_W /2;
				TILE_H_HALF = TILE_H /2;
			}
			else if (infile.key == "orientation") {
				if (infile.val == "isometric")
					TILESET_ORIENTATION = TILESET_ISOMETRIC;
				else if (infile.val == "orthogonal")
					TILESET_ORIENTATION = TILESET_ORTHOGONAL;
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/tileset_config.txt! Defaulting to 64x32 isometric tiles.\n");

	// Init automatically calculated parameters
	TILE_SHIFT = log2(UNITS_PER_TILE);
	VIEW_W_HALF = VIEW_W / 2;
	VIEW_H_HALF = VIEW_H / 2;
	if (TILESET_ORIENTATION == TILESET_ISOMETRIC) {
		UNITS_PER_PIXEL_X = (UNITS_PER_TILE * 2) / TILE_W;
		UNITS_PER_PIXEL_Y = (UNITS_PER_TILE * 2) / TILE_H;
	}
	else { // TILESET_ORTHOGONAL
		UNITS_PER_PIXEL_X = UNITS_PER_TILE / TILE_W;
		UNITS_PER_PIXEL_Y = UNITS_PER_TILE / TILE_H;
	}
	if (UNITS_PER_PIXEL_X == 0 || UNITS_PER_PIXEL_Y == 0)
	{
		fprintf(stderr, "One of UNITS_PER_PIXEL values is zero! %dx%d\n", UNITS_PER_PIXEL_X, UNITS_PER_PIXEL_Y);
		SDL_Quit();
		exit(1);
	}
}

void loadMiscSettings() {
	FileParser infile;
	// load miscellaneous settings from engine config
	// misc.txt
	if (infile.open(mods->locate("engine/misc.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "save_hpmp") {
				if (toInt(infile.val) == 1)
					SAVE_HPMP = true;
			} else if (infile.key == "corpse_timeout") {
				CORPSE_TIMEOUT = toInt(infile.val);
			} else if (infile.key == "sell_without_vendor") {
				if (toInt(infile.val) == 1)
					SELL_WITHOUT_VENDOR = true;
				else
					SELL_WITHOUT_VENDOR = false;
			} else if (infile.key == "aim_assist") {
				AIM_ASSIST = toInt(infile.val);
			} else if (infile.key == "window_title") {
				WINDOW_TITLE = infile.val;
			} else if (infile.key == "game_prefix") {
				GAME_PREFIX = infile.val;
			} else if (infile.key == "sound_falloff") {
				SOUND_FALLOFF = toInt(infile.val);
			}

		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/misc.txt!\n");
	// resolutions.txt
	if (infile.open(mods->locate("engine/resolutions.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "menu_frame_width")
				FRAME_W = toInt(infile.val);
			else if (infile.key == "menu_frame_height")
				FRAME_H = toInt(infile.val);
			else if (infile.key == "icon_size")
				ICON_SIZE = toInt(infile.val);
			else if (infile.key == "required_width") {
				MIN_VIEW_W = toInt(infile.val);
				if (VIEW_W < MIN_VIEW_W) VIEW_W = MIN_VIEW_W;
				VIEW_W_HALF = VIEW_W/2;
			} else if (infile.key == "required_height") {
				MIN_VIEW_H = toInt(infile.val);
				if (VIEW_H < MIN_VIEW_H) VIEW_H = MIN_VIEW_H;
				VIEW_H_HALF = VIEW_H/2;
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/resolutions.txt!\n");
	// gameplay.txt
	if (infile.open(mods->locate("engine/gameplay.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "enable_playgame") {
				if (toInt(infile.val) == 1)
					ENABLE_PLAYGAME = true;
				else
					ENABLE_PLAYGAME = false;
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/gameplay.txt!\n");
	// combat.txt
	if (infile.open(mods->locate("engine/combat.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "max_absorb_percent") {
				MAX_ABSORB = toInt(infile.val);
			} else if (infile.key == "max_resist_percent") {
				MAX_RESIST = toInt(infile.val);
			} else if (infile.key == "max_block_percent") {
				MAX_BLOCK = toInt(infile.val);
			} else if (infile.key == "max_avoidance_percent") {
				MAX_AVOIDANCE = toInt(infile.val);
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/combat.txt!\n");
	// elements.txt
	if (infile.open(mods->locate("engine/elements.txt").c_str())) {
		Element e;
		ELEMENTS.clear();
		while (infile.next()) {
			if (infile.key == "name") e.name = infile.val;
			else if (infile.key == "resist") e.resist = infile.val;

			if (e.name != "" && e.resist != "") {
				ELEMENTS.push_back(e);
				e.name = e.resist = "";
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/elements.txt!\n");
	// classes.txt
	if (infile.open(mods->locate("engine/classes.txt").c_str())) {
		HeroClass c;
		HERO_CLASSES.clear();
		while (infile.next()) {
			if (infile.key == "name") c.name = infile.val;

			if (c.name != "") {
				HERO_CLASSES.push_back(c);
				c.name = "";
			}

			if (!HERO_CLASSES.empty()) {
				if (infile.key == "description") HERO_CLASSES.back().description = infile.val;
				else if (infile.key == "currency") HERO_CLASSES.back().currency = toInt(infile.val);
				else if (infile.key == "equipment") HERO_CLASSES.back().equipment = infile.val;
				else if (infile.key == "physical") HERO_CLASSES.back().physical = toInt(infile.val);
				else if (infile.key == "mental") HERO_CLASSES.back().mental = toInt(infile.val);
				else if (infile.key == "offense") HERO_CLASSES.back().offense = toInt(infile.val);
				else if (infile.key == "defense") HERO_CLASSES.back().defense = toInt(infile.val);
				else if (infile.key == "actionbar") {
					for (int i=0; i<12; i++) {
						HERO_CLASSES.back().hotkeys[i] = toInt(infile.nextValue());
					}
				}
				else if (infile.key == "powers") {
					string power;
					while ( (power = infile.nextValue()) != "") {
						HERO_CLASSES.back().powers.push_back(toInt(power));
					}
				}
				else if (infile.key == "campaign") {
					string status;
					while ( (status = infile.nextValue()) != "") {
						HERO_CLASSES.back().statuses.push_back(status);
					}
				}
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/classes.txt!\n");

	// Make a default hero class if none were found
	if (HERO_CLASSES.empty()) {
		HeroClass c;
		c.name = msg->get("Adventurer");
		HERO_CLASSES.push_back(c);
	}
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
	if (!infile.open(PATH_CONF + FILE_SETTINGS)) {
		if (!infile.open(mods->locate("engine/default_settings.txt").c_str())) {
			saveSettings();
			return true;
		} else saveSettings();
	}

	while (infile.next()) {

		ConfigEntry * entry = getConfigEntry(infile.key);
		if (entry) {
			// TODO: handle errors
			tryParseValue(*entry->type, infile.val, entry->storage);
		}
	}
	infile.close();

	return true;
}

/**
 * Save the current main settings (primary video and audio settings)
 */
bool saveSettings() {

	ofstream outfile;
	outfile.open((PATH_CONF + FILE_SETTINGS).c_str(), ios::out);

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

		if (outfile.bad()) fprintf(stderr, "Unable to write settings file. No write access or disk is full!\n");
		outfile.close();
		outfile.clear();
	}
	return true;
}

/**
 * Load all default settings, except video settings.
 */
bool loadDefaults() {

	// HACK init defaults except video
	for (int i = 3; i < config_size; i++) {
		// TODO: handle errors
		ConfigEntry * entry = config + i;
		tryParseValue(*entry->type, entry->default_val, entry->storage);
	}

	// Init automatically calculated parameters
	VIEW_W_HALF = VIEW_W / 2;
	VIEW_H_HALF = VIEW_H / 2;

	return true;
}
