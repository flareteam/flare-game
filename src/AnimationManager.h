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
class AnimationSet;

class AnimationManager {
private:
    std::vector<AnimationSet*> sets;
    std::vector<std::string> names;
    std::vector<int> counts;

    AnimationManager();
    ~AnimationManager();

public:
    /** Get instance of the Singleton */
    static AnimationManager *instance();

    /**
     * @param name: the filename of what to load starting below the animations folder.
     */
    AnimationSet *getAnimationSet(const std::string &name);

    void decreaseCount(const std::string &name);
    void increaseCount(const std::string &name);
    void cleanUp();
};

