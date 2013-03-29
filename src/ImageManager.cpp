/*
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

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

#include <assert.h>


#include "ImageManager.h"
#include "Settings.h"
#include "SharedResources.h"

using namespace std;

ImageManager* ImageManager_instance = 0;

ImageManager::ImageManager()
{}

ImageManager::~ImageManager()
{
	cleanUp();
// NDEBUG is used by posix to disable assertions, so use the same MACRO.
#ifndef NDEBUG
	if (!sprites.empty()) {
		cout << "ImageManager still holding these images:" << endl;
		for (unsigned i = 0; i < sprites.size(); ++i)
			 fprintf(stderr, "%s %d\n", names[i].c_str(), counts[i]);
	}
	assert(sprites.size() == 0);
#endif
}

SDL_Surface *ImageManager::getSurface(const std::string &name) {
	vector<string>::iterator found = find(names.begin(), names.end(), name);
	if (found != names.end()) {
		int index = distance(names.begin(), found);
		if (!sprites[index]) {
			if (!TEXTURE_QUALITY) {
				string newname = string(name);
				newname.replace(name.rfind("/"), 0, "/noalpha");
				sprites[index] = loadGraphicSurface(newname, "Falling back to alpha version", false, true);
			}
			if (!sprites[index])
				sprites[index] = loadGraphicSurface(name);
		}
		return sprites[index];
	}
	return 0;
}

void ImageManager::increaseCount(const std::string &name) {
	vector<string>::iterator found = find(names.begin(), names.end(), name);
	if (found != names.end()) {
		int index = distance(names.begin(), found);
		counts[index]++;
	} else {
		sprites.push_back(0);
		names.push_back(name);
		counts.push_back(1);
	}
}

void ImageManager::decreaseCount(const std::string &name) {
	vector<string>::iterator found = find(names.begin(), names.end(), name);
	if (found != names.end()) {
		int index = distance(names.begin(), found);
		counts[index]--;
	} else {
		fprintf(stderr, "ImageManager::decreaseCount: Couldn't decrease image count: %s\n", name.c_str());
	}
}

void ImageManager::cleanUp() {
	int i = sprites.size() - 1;
	while (i >= 0) {
		if (counts[i] <= 0) {
			SDL_FreeSurface(sprites[i]);
			counts.erase(counts.begin()+i);
			sprites.erase(sprites.begin()+i);
			names.erase(names.begin()+i);
		}
		--i;
	}
}
