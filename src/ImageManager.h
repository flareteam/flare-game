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

#include <SDL_image.h>

/**
 * The image manager class is a manager for images of any kind.
 * It makes sure there are no images loaded twice, but returns the appropriate
 * references to the already loaded image.
 *
 * The API is designed for lazy loading of images, so on map change events,
 * which renews most images, this manager tries not to reload the images
 * but rather keep them in memory.
 */

class ImageManager {
private:
    std::vector<SDL_Surface*> sprites;
    std::vector<std::string> names;
    std::vector<int> counts;

    ImageManager();
    ~ImageManager();

public:
    /** Get instance of the Singleton */
    static ImageManager *instance();

    /**
     * Returns the image specified by the filename as parameter.
     * The image must be in the pool already, i.e. the increaseCount
     * function with the same parameter must be called before.
     */
    SDL_Surface *getSurface(const std::string &name);

    /**
     * Decreases the count for this image.
     */
    void decreaseCount(const std::string &name);

    /**
     * Increases the count for this image.
     */
    void increaseCount(const std::string &name);

    /**
     * Removes all images with count equal zero.
     */
    void cleanUp();
};
