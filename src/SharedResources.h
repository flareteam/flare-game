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
SharedResources

"Global" sort of system resources that are used by most game classes.
Only one instance of these classes are needed by the engine.
Generic objects only. Game-specific objects don't belong here.
Created and destroyed by main.cpp
**/

#ifndef SHARED_RESOURCES_H
#define SHARED_RESOURCES_H

#include <SDL.h>
#include "ModManager.h"
#include "MessageEngine.h"
#include "InputState.h"
#include "FontEngine.h"
#include "Network.h"

extern SDL_Surface *screen;
extern ModManager *mods;
extern MessageEngine *msg;
extern InputState *inpt;
extern FontEngine *font;

extern Multiplayer *network;
extern bool multiplayer;
extern bool isHost;

extern bool audio;

#endif
