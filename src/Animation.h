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

#include "Utils.h"

#include <SDL_image.h>

#include <string>
#include <vector>
#include <iostream>

enum animation_type {
	NONE       = 0,
	PLAY_ONCE  = 1, // just iterates over the images one time. it holds the final image when finished.
	LOOPED     = 2, // going over the images again and again.
	BACK_FORTH = 3  // iterate from index=0 to maxframe and back again. keeps holding the first image afterwards.
};

class Animation {
protected:
	std::string name;
	animation_type type;

	SDL_Surface* sprites;


	unsigned short number_frames; // how many ticks this animation lasts.
	unsigned short cur_frame;     // counts up until reaching number_frames.

	unsigned short cur_frame_index; // which frame in this animation is currently being displayed? range: 0..gfx.size()-1
	unsigned short cur_frame_duration;  // how many ticks is the current image being displayed yet? range: 0..duration[cur_frame]-1


	short additional_data;  // additional state depending on type:
							// if type == BACK_FORTH then it is 1 for advancing, and -1 for going back, 0 at the end
							// if type == LOOPED, then it is the number of loops to be played.
							// if type == PLAY_ONCE or NONE, this has no meaning.

	short times_played; // how often this animation was played (loop counter for type LOOPED)

	// Frame data, all vectors must have the same length:
	// These are indexed as 8*cur_frame_index + direction.
	std::vector<SDL_Rect> gfx; // position on the spritesheet to be used.
	std::vector<Point> render_offset; // "virtual point on the floor"
	std::vector<short> duration; //duration of each individual image

public:
	Animation(std::string _name, std::string _type);

	// returns a copy of this:
	Animation(const Animation&);

	// Traditional way to create an animation.
	// The frames are stored in a grid like fashion, so the individual frame
	// position can be calculated based on a few things.
	// The spritesheet has 8 rows, each containing the data of one direction.
	// Within a row starting at (_position) there will be (_frames) frames,
	// which all belong to this animation.
	// The render_offset is constant for all frames. The render_size is also
	// the grid size.
	void setupUncompressed(Point render_size, Point render_offset, int _position, int _frames, int _duration);

	void setup(unsigned short _frames, unsigned short _duration);
	void addFrame(unsigned short index, unsigned short direction, SDL_Rect sdl_rect, Point _render_offset);
	void doneLoading();

	// advance the animation one frame
	void advanceFrame();

	// return the Renderable of the current frame
	Renderable getCurrentFrame(int direction);

	bool isFirstFrame() { return cur_frame == 0; }
	bool isLastFrame() { return cur_frame == number_frames - 1; }
	bool isActiveFrame() { return cur_frame == number_frames /2; }

	// in a looped animation returns how many times it's been played
	// in a play once animation returns 1 when the animation is finished
	int getTimesPlayed() { return times_played; }

	// resets to beginning of the animation
	void reset();

	std::string getName() { return name; }
};

#endif

