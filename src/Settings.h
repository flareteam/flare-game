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

// Path info
extern std::string PATH_CONF; // user-configurable settings files
extern std::string PATH_USER; // important per-user data (saves)
extern std::string PATH_DATA; // common game data

// Filenames
extern std::string FILE_SETTINGS;     // Name of the settings file (e.g. "settings.txt").
extern std::string FILE_KEYBINDINGS;  // Name of the key bindings file (e.g. "keybindings.txt").

// Audio and Video Settings
extern int MUSIC_VOLUME;
extern int SOUND_VOLUME;
extern bool FULLSCREEN;
extern int FRAMES_PER_SEC;
extern int VIEW_W;
extern int VIEW_H;
extern int VIEW_W_HALF;
extern int VIEW_H_HALF;
extern bool DOUBLEBUF;
extern bool HWSURFACE;
extern float GAMMA;

// Input Settings
extern bool MOUSE_MOVE;
extern bool ENABLE_JOYSTICK;
extern int JOYSTICK_DEVICE;

// User Options
extern bool COMBAT_TEXT;

// Engine Settings
extern bool MENUS_PAUSE;

// Tile Settings
extern int UNITS_PER_TILE;
extern int TILE_SHIFT;
extern int UNITS_PER_PIXEL_X;
extern int UNITS_PER_PIXEL_Y;
extern int TILE_W;
extern int TILE_H;
extern int TILE_W_HALF;
extern int TILE_H_HALF;
extern int TILESET_ORIENTATION;
extern int TILESET_ISOMETRIC;
extern int TILESET_ORTHOGONAL;

// Language Settings
extern std::string LANGUAGE;

void setPaths();
void loadTilesetSettings();
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
