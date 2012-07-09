#ifndef WIDGET_H
#define WIDGET_H

/**
 * Base interface all widget needs to implement
 */
#include <SDL.h>
class Widget {
public:
    Widget() {};
	virtual ~Widget() {};
	virtual void render(SDL_Surface *target = NULL) = 0;
	bool render_to_alpha;
private:
};

#endif

