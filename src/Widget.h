#ifndef WIDGET_H
#define WIDGET_H

/**
 * Base interface all widget needs to implement
 */
#include <SDL.h>
class Widget {
public:
    virtual void    render  (SDL_Surface *target = NULL) = 0;
private:
};

#endif

