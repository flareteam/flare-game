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

#include "SmartSurface.h"
#include <cassert>

SmartSurface::SmartSurface() : surface_(NULL) {}

SmartSurface::SmartSurface(SDL_Surface* surface) : surface_(surface) {}

SmartSurface::~SmartSurface() {
	reset();
}

SDL_Surface* SmartSurface::release() {
	SDL_Surface* retval = surface_;
	surface_ = NULL;
	return retval;
}

SDL_Surface* SmartSurface::get() const {
	assert(surface_ && "Invalid surface requested");
	return surface_;
}

bool SmartSurface::is_null() const {
	return surface_ == NULL;
}

void SmartSurface::reset(SDL_Surface* surface) {
	if (surface_)
		SDL_FreeSurface(surface_);
	surface_ = surface;
}

SmartSurface::operator bool() const {
	return surface_ == NULL;
}

bool SmartSurface::operator!() const{
	return !surface_;
}

