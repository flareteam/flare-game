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


#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"

#include <string>

using namespace std;

AnimationManager* _instance = 0;

AnimationSet *AnimationManager::getAnimationSet(const string& filename) {
    for (size_t i = 0; i < sets.size(); i++)
        if (sets[i]->getName() == filename)
            return sets[i];

    sets.push_back(new AnimationSet("animations/"+filename));
    return sets.back();
}

AnimationManager::AnimationManager()
{}

AnimationManager::~AnimationManager()
{}

AnimationManager *AnimationManager::instance()
{
    if (_instance == 0) _instance = new AnimationManager();
    return _instance;
}
