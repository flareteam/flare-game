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

/**
 * Settings
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

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

extern unsigned short ICON_SIZE_SMALL;
extern unsigned short ICON_SIZE_LARGE;

// Audio and Video Settings
extern unsigned char MUSIC_VOLUME;
extern unsigned char SOUND_VOLUME;
extern bool FULLSCREEN;
extern unsigned char BITS_PER_PIXEL;
extern unsigned short FRAMES_PER_SEC;
extern unsigned short VIEW_W;
extern unsigned short VIEW_H;
extern unsigned short VIEW_W_HALF;
extern unsigned short VIEW_H_HALF;
extern short MIN_VIEW_W;
extern short MIN_VIEW_H;
extern bool DOUBLEBUF;
extern bool HWSURFACE;
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
extern bool AUTOPICKUP_GOLD;

// Combat calculation caps
extern short MAX_ABSORB;
extern short MAX_RESIST;
extern short MAX_BLOCK;
extern short MAX_AVOIDANCE;

// Elemental types
extern std::vector<std::string> ELEMENTS;

void setPaths();
void loadTilesetSettings();
void loadAutoPickupSettings();
void loadMiscSettings();
bool loadSettings();
bool saveSettings();
bool loadDefaults();

const std::string binding_name[] = {"Cancel", "Accept", "Up", "Down", "Left", "Right",
									"Bar1", "Bar2", "Bar3", "Bar4", "Bar5", "Bar6",
									"Bar7", "Bar8", "Bar9", "Bar0", "Character", "Inventory",
									"Powers", "Log", "Main1", "Main2", "Ctrl",
									"Shift", "Delete"};

const std::string mouse_button[] = {"lmb", "mmb", "rmb", "wheel up", "wheel down", "mbx1", "mbx2"};

#endif
