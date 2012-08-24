/*
Copyright © 2011-2012 kitano

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
 * class Animation
 *
 * The Animation class handles the logic of advancing frames based on the animation type
 * and returning a renderable frame.
 *
 * The intention with the class is to keep it as flexible as possible so that the animations
 * can be used not only for character animations but any animated in-game objects.
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL_image.h>
#include "Utils.h"
#include <string>

enum animation_type {
	NONE,
	PLAY_ONCE, // just iterates over the images one time. it holds the final image when finished.
	LOOPED,    // going over the images again and again.
	BACK_FORTH // iterate from index=0 to maxframe and back again. keeps holding the first image afterwards.
};

class Animation {
protected:
	std::string name;

	SDL_Surface* sprites;
	animation_type type;

	short cur_frame;        // which frame in this animation is currently being displayed? range: 0..gfx.size()-1
	short duration;         // how many ticks is the current image being displayed yet? range: 0..duration[cur_frame]-1
	short additional_data;  // additional state depending on type:
							// if type == BACK_FORTH then it is 1 for advancing, and -1 for going back, 0 at the end
							// if type == LOOPED, then it is the number of loops still to be completed, i.e. from n to 0.
							// if type == PLAY_ONCE or NONE, this has no meaning.

	// Frame data:
	std::vector<SDL_Rect> gfx; // position on the spritesheet to be used.
	std::vector<Point> render_offset; // "virtual point on the floor"
	std::vector<int> duration; //duration of each individual image

public:
	Animation();

	void init(std::string, Point _render_size, Point _render_offset, int position, int frames, int duration, std::string, int _active_frame);

	// advance the animation one frame
	void advanceFrame();
	// return the Renderable of the current frame
	Renderable getCurrentFrame(int direction);

	int getCurFrame() { return cur_frame; }
	int getMaxFrame() { return max_frame; }

	bool isFirstFrame() { return cur_frame == 1; }
	bool isLastFrame() { return cur_frame == (max_frame -1); }

	//bool isActiveFrame() { return cur_frame == active_frame; }
	bool isActiveFrame() { return cur_frame == max_frame /2; }


	// in a looped animation returns how many times it's been played
	// in a play once animation returns 1 when the animation is finished
	int getTimesPlayed() { return timesPlayed; }

	// resets to beginning of the animation
	void reset();

	std::string getName() { return name; }
};

#endif

