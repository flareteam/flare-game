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
#include <math.h>
#include "Settings.h"
#include "SoundManager.h"
#include "SharedResources.h"

using namespace std;

#define SOUND_FALLOFF (UNITS_PER_TILE * 15.0f)

class Sound {
public:
	Mix_Chunk *chunk;
	Sound() :  chunk(0), refCnt(0) {};
private:
	friend class SoundManager;
	int refCnt;
};

class Playback {
public:
	Sound *sound;
	std::string virtual_channel;
	Point location;
};

SoundManager::SoundManager() {
	Mix_AllocateChannels(30);
}

SoundManager::~SoundManager() {
	SoundManager::SoundMapIterator it;
	while((it = sounds.begin()) != sounds.end())
		unload(it->first);
}

void SoundManager::logic(Point c) {


  PlaybackMapIterator it = playback.begin();
  if (it == playback.end())
    return;

  while(it != playback.end()) {

	  /* dont process playback sounds without location */
	  if (it->second.location.x == 0 && it->second.location.y == 0) {
		  ++it; 
		  continue;
	  }

	  /* calculate distance and angle */
	  Uint8 d = 0, a = 0;
	  float dx = c.x - it->second.location.x;
	  float dy = c.y - it->second.location.y;
	  float dist = sqrt(dx*dx + dy*dy);
	  
	  /* update sound mix with new distance/location to hero */
	  dist = 255.0f * (dist / SOUND_FALLOFF);
	  d = max(0.0f, min(dist, 255.0f));
	  // a = atan2(dy,dx) * 180 / M_PI;
	  Mix_SetPosition(it->first, a, d);
	  it++;
  }

}

void SoundManager::reset() {
	Mix_HaltChannel(-1);
}

SoundManager::SoundID SoundManager::load(const std::string& filename, const std::string& errormessage) {

	Sound lsnd;
	SoundID sid = 0;
	SoundMapIterator it;
	std::locale loc;

	if (!AUDIO || !SOUND_VOLUME)
		return 0;

	const collate<char>& coll = use_facet<collate<char> >(loc);
	const string realfilename = mods->locate(filename);

	/* create sid hash and check if already loaded */
	sid = coll.hash(realfilename.data(), realfilename.data()+realfilename.length());
	it = sounds.find(sid);
	if (it != sounds.end()) {
		it->second->refCnt++;
		return sid;
	}

	/* load non existing sound */
	lsnd.chunk = Mix_LoadWAV(realfilename.c_str());
	lsnd.refCnt = 1;
	if (!lsnd.chunk) {
		fprintf(stderr, "%s: Loading sound %s (%s) failed: %s \n", errormessage.c_str(),
			realfilename.c_str(), filename.c_str(), Mix_GetError());
		return 0;
	}

	/* instantiate and add sound to manager */
	Sound *psnd = new Sound;
	*psnd = lsnd;
	sounds.insert(pair<SoundID,Sound *>(sid, psnd));

	return sid;
}

void SoundManager::unload(SoundManager::SoundID sid) {

	SoundMapIterator it;
	it = sounds.find(sid);
	if (it == sounds.end())
		return;

	if (--it->second->refCnt == 0) {
		Mix_FreeChunk(it->second->chunk);
		delete it->second;
		sounds.erase(it);
	}
}



void SoundManager::play(SoundManager::SoundID sid, std::string channel, Point pos, bool loop) {

	SoundMapIterator it;
	VirtualChannelMapIterator vcit = channels.end();

	if (!sid || !AUDIO || !SOUND_VOLUME)
		return;

	it = sounds.find(sid);
	if (it == sounds.end())
		return;

	/* create playback object and start playback of sound chunk */
	Playback p;
	p.sound = it->second;
	p.location = pos;
	p.virtual_channel = channel;

	if (p.virtual_channel != GLOBAL_VIRTUAL_CHANNEL) {

		/* if playback exists, stop it befor playin next sound */
		vcit = channels.find(p.virtual_channel);
		if (vcit != channels.end())
			Mix_HaltChannel(vcit->second);

		vcit = channels.insert(pair<std::string, int>(p.virtual_channel, -1)).first;
	}

	Mix_ChannelFinished(&channel_finished);
	int c = Mix_PlayChannel(-1, p.sound->chunk, (loop ? -1 : 0));

	if (c == -1)
		fprintf(stderr,"Failed to play sound.\n");

	if(p.location.x != 0 || p.location.y != 0)
		Mix_SetPosition(c, 0, 255);

	if (vcit != channels.end())
		vcit->second = c;

	playback.insert(pair<int, Playback>(c, p));
}

void SoundManager::on_channel_finished(int channel)
{
	PlaybackMapIterator pit = playback.find(channel);
	if (pit == playback.end())
		return;

	/* find and erase virtual channel for playback if exists */
	VirtualChannelMapIterator vcit = channels.find(pit->second.virtual_channel);
	if (vcit != channels.end())
		channels.erase(vcit);

	Mix_SetPosition(channel, 0, 0);

	playback.erase(pit);
}

void SoundManager::channel_finished(int channel)
{
	snd->on_channel_finished(channel);
}
