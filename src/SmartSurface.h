/*
Copyright 2011 Clint Bellanger

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
 * class SmartSurface
 *
 * Wraps an SDL_Surface, calling SDL_Surface on it when it goes out of scope
 */
 
#ifndef SMART_SURFACE_H
#define SMART_SURFACE_H

#include <SDL.h>
#include <SDL_image.h>

class SmartSurface {
private:
	SDL_Surface* surface_;
	SmartSurface(SmartSurface const& surface);
	SmartSurface& operator=(SmartSurface const& surface);

public:
	SmartSurface();
	explicit SmartSurface(SDL_Surface* surface);
	~SmartSurface();

	SDL_Surface* release();
	// fails if SDL_Surface is invalid.
	SDL_Surface* get() const;
	bool is_null() const;
	void reset(SDL_Surface* surface = NULL);

	operator bool() const;
	bool operator!() const;

	SDL_Surface& operator*();
	SDL_Surface const& operator*() const;
	SDL_Surface* operator->();
	SDL_Surface const* operator->() const;
};

#endif
