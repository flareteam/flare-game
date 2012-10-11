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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

typedef struct Element{
	std::string name;
	std::string resist;
}Element;

typedef struct HeroClass{
	std::string name;
	std::string description;
	int currency;
	std::string equipment;
	int physical;
	int mental;
	int offense;
	int defense;
	int hotkeys[12];
	std::vector<int> powers;

	HeroClass() {
		name = "";
		description = "";
		currency = 0;
		equipment = "";
		physical = 0;
		mental = 0;
		offense = 0;
		defense = 0;
		for (int i=0; i<12; i++) {
			hotkeys[i] = 0;
		}
		powers.clear();
	}
}HeroClass;

// Path info
extern std::string PATH_CONF; // user-configurable settings files
extern std::string PATH_USER; // important per-user data (saves)
extern std::string PATH_DATA; // common game data

// Filenames
extern std::string FILE_SETTINGS;     // Name of the settings file (e.g. "settings.txt").
extern std::string FILE_KEYBINDINGS;  // Name of the key bindings file (e.g. "keybindings.txt").

// Main Menu frame size
extern unsigned short FRAME_W;
extern unsigned short FRAME_H;

extern unsigned short ICON_SIZE;

// Audio and Video Settings
extern unsigned short MUSIC_VOLUME;
extern unsigned short SOUND_VOLUME;
extern bool FULLSCREEN;
extern unsigned char BITS_PER_PIXEL;
extern unsigned short MAX_FRAMES_PER_SEC;
extern unsigned short VIEW_W;
extern unsigned short VIEW_H;
extern unsigned short VIEW_W_HALF;
extern unsigned short VIEW_H_HALF;
extern short MIN_VIEW_W;
extern short MIN_VIEW_H;
extern bool DOUBLEBUF;
extern bool HWSURFACE;
extern bool CHANGE_GAMMA;
extern float GAMMA;
extern bool TEXTURE_QUALITY;
extern bool ANIMATED_TILES;

// Input Settings
extern bool MOUSE_MOVE;
extern bool ENABLE_JOYSTICK;
extern int JOYSTICK_DEVICE;
extern bool MOUSE_AIM;

// User Options
extern bool COMBAT_TEXT;

// Engine Settings
extern bool MENUS_PAUSE;
extern std::string DEFAULT_NAME;
extern bool SAVE_HPMP;
extern bool ENABLE_PLAYGAME;
extern bool SHOW_FPS;
extern int CORPSE_TIMEOUT;
extern bool SELL_WITHOUT_VENDOR;

// Tile Settings
extern unsigned short UNITS_PER_TILE;
extern unsigned short TILE_SHIFT;
extern unsigned short UNITS_PER_PIXEL_X;
extern unsigned short UNITS_PER_PIXEL_Y;
extern unsigned short TILE_W;
extern unsigned short TILE_H;
extern unsigned short TILE_W_HALF;
extern unsigned short TILE_H_HALF;
extern unsigned short TILESET_ORIENTATION;
extern unsigned short TILESET_ISOMETRIC;
extern unsigned short TILESET_ORTHOGONAL;

// Language Settings
extern std::string LANGUAGE;

// Autopickup Settings
extern unsigned short AUTOPICKUP_RANGE;
extern bool AUTOPICKUP_CURRENCY;

// Combat calculation caps
extern short MAX_ABSORB;
extern short MAX_RESIST;
extern short MAX_BLOCK;
extern short MAX_AVOIDANCE;

// Elemental types
extern std::vector<Element> ELEMENTS;

// Hero classes
extern std::vector<HeroClass> HERO_CLASSES;

// Currency settings
extern std::string CURRENCY;
extern float VENDOR_RATIO;

void setPaths();
void loadTilesetSettings();
void loadMiscSettings();
bool loadSettings();
bool saveSettings();
bool loadDefaults();


#endif
