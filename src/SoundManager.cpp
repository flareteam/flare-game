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
SoundManager

SoundManager take care of loading and playing of sound effects,
each sound is references with a hash SoundID for playing. If a
sound is already loaded the SoundID for currently loaded sound
will be returned by SoundManager::load().

**/

#include <map>
#include <locale>
#include <string>
#include <SDL_mixer.h>

#include "Settings.h"
#include "SoundManager.h"
#include "SharedResources.h"

using namespace std;

class Sound {
public:
	Mix_Chunk *chunk;
	Sound() :  refCnt(0) {};
private:
	friend class SoundManager;
	int refCnt;
};

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
	SoundManager::SoundMapIterator it;
	while((it = sounds.begin()) != sounds.end())
		unload(it->first);
}

SoundManager::SoundID SoundManager::load(const std::string& filename, const std::string& errormessage) {

	Sound lsnd, *psnd;
	SoundID sid = 0;
	SoundMapIterator it;
	std::locale loc;

	if (AUDIO && SOUND_VOLUME)
	{
		const collate<char>& coll = use_facet<collate<char> >(loc);
		const string realfilename = mods->locate(filename);

		/* create sid hash and check if already loaded */
		sid = coll.hash(realfilename.data(), realfilename.data()+realfilename.length());
		it = sounds.find(sid);
		if (it != sounds.end())
		{
			it->second->refCnt++;
			return sid;
		}

		/* load non existing sound */
		lsnd.chunk = Mix_LoadWAV(realfilename.c_str());
		lsnd.refCnt = 1;
		if (!lsnd.chunk)
		{
			fprintf(stderr, "%s: Loading sound %s (%s) failed: %s \n", errormessage.c_str(), 
				realfilename.c_str(), filename.c_str(), Mix_GetError());
			return 0;
		}

		/* instansiate and add sound to manager */
		psnd = new Sound;
		*psnd = lsnd;
		sounds.insert(pair<SoundID,Sound *>(sid, psnd));
	}

	return sid;
}

void SoundManager::unload(SoundManager::SoundID sid) {

	SoundMapIterator it;
	it = sounds.find(sid);
	if (it == sounds.end())
		return;

	if (--it->second->refCnt == 0)
	{
		//fprintf(stderr,"Unload %lx\n", sid);
		Mix_FreeChunk(it->second->chunk);
		delete it->second;
		sounds.erase(it);
	}
}

int SoundManager::play(SoundManager::SoundID sid) {

	SoundMapIterator it;

	//fprintf(stderr,"Play %lx\n", sid);

	if (!sid || !AUDIO || !SOUND_VOLUME)
		return -1;

	it = sounds.find(sid);
	if (it == sounds.end())
		return -1;

	return Mix_PlayChannel(-1, it->second->chunk, 0);
}
