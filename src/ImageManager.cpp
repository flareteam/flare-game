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

#include <assert.h>


#include "ImageManager.h"
#include "Settings.h"
#include "SharedResources.h"

using namespace std;

ImageManager* ImageManager_instance = 0;

ImageManager *ImageManager::instance()
{
    if (ImageManager_instance == 0) ImageManager_instance = new ImageManager();
    return ImageManager_instance;
}

ImageManager::ImageManager()
{}

ImageManager::~ImageManager()
{
    assert(sprites.size() == 0);
}

SDL_Surface *ImageManager::getSurface(const std::string &name) {
    vector<string>::iterator found = find(names.begin(), names.end(), name);
    if (found != names.end()) {
        int index = distance(names.begin(), found);
        if (sprites[index] == 0) {
            SDL_Surface *cleanup = NULL;

            if (TEXTURE_QUALITY == false) {
                string path = mods->locate(name);
                const char * pch = strrchr(path.c_str(), '/' );
                path.insert(pch-path.c_str(), "/noalpha");
                cleanup = IMG_Load(path.c_str());
                if (!cleanup)
                    printf("failed to load %s\n", path.c_str());
                else
                    SDL_SetColorKey(cleanup, SDL_SRCCOLORKEY, SDL_MapRGB(cleanup->format, 255, 0, 255));
            }

            if (!cleanup) {
                cleanup = IMG_Load(mods->locate(name).c_str());
            }

            if (!cleanup) {
                fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
                SDL_Quit();
                exit(17);
            }

            SDL_Surface *sprite = SDL_DisplayFormatAlpha(cleanup);
            SDL_FreeSurface(cleanup);
            sprites[index] = sprite;
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
        //assert(1 == 0); // This should never happen!
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
