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
 * The animation set contains all animations of one entity, hence it
 * they are all using the same spritesheet.
 *
 * The animation set is responsible for the spritesheet to be freed.
 */
class AnimationSet {
private:
    const std::string name; //i.e. animations/goblin_runner.txt, matches the animations filename.
    std::string imagefile;

public:
    std::string starting_animation; // i.e. stance, matches the section in the animation file.
    std::vector<Animation*> animations;

    SDL_Surface *sprite;

    AnimationSet(const std::string &animationname);
    ~AnimationSet();

    // must be called before getAnimation is called.
    void load();

    /**
     * callee is responsible to free the returned animation
     */
    Animation *getAnimation(const std::string &name);

    const std::string &getName() { return name; }
};
