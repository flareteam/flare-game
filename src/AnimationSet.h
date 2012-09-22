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


#include <string>
#include <vector>

class Animation;

/**
 * The animation set contains all animations of one entity.
 */
class AnimationSet {
public:
    std::vector<Animation*> animations;
    std::string starting_animation; // i.e. stance, matches the section in the animation file.
    std::string name; //i.e. animations/goblin_runner.txt, matches the filename.

    AnimationSet(const std::string &name);

    /**
     * callee is responsible to free the returned animation
     */
    Animation *getAnimation(const std::string &name);
};
