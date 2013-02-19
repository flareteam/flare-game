/*
Copyright © 2013 Henrik Andersson

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
 * class SoundManager
 */


#pragma once
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <map>
#include <string>
#include <stdint.h>
#include <vector>

const std::string GLOBAL_VIRTUAL_CHANNEL = "__global__";

class SoundManager {
public:
	typedef unsigned long SoundID;

	SoundManager();
	~SoundManager();

	SoundManager::SoundID load(const std::string& filename, const std::string& errormessage);
	void unload(SoundManager::SoundID);

	void play(SoundManager::SoundID, std::string channel = GLOBAL_VIRTUAL_CHANNEL);

private:
	typedef std::map<std::string, int> VirtualChannelMap;
	typedef VirtualChannelMap::iterator VirtualChannelMapIterator;

	typedef std::map<SoundID, class Sound *> SoundMap;
	typedef SoundMap::iterator SoundMapIterator;

	static void channel_finished(int channel);
	void on_channel_finished(int channel);

	SoundMap sounds;
	VirtualChannelMap channels;
};

#endif
