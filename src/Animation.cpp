/*
Copyright © 2011-2012 kitano
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

/**
 * class Animation
 *
 * The Animation class handles the logic of advancing frames based on the animation type
 * and returning a renderable frame.
 *
 * The intention with the class is to keep it as flexible as possible so that the animations
 * can be used not only for character animations but any animated in-game objects.
 */

#include "Animation.h"

#include <iostream>
using namespace std;

Animation::Animation()
	: name("")
	, sprites(NULL)
	, render_size(Point())
	, render_offset(Point())
	, position(0)
	, frames(0)
	, duration(0)
	, type(NONE)
	, cur_frame(0)
	, disp_frame(0)
	, mid_frame(0)
	, max_frame(0)
	, timesPlayed(0)
	, active_frame(0)
{
}

void Animation::init(std::string _name,
				Point _render_size,
				Point _render_offset,
				int _position,
				int _frames,
				int _duration,
				std::string _type,
				int /*_active_frame*/)
{
	name = _name;
	render_size = _render_size;
	render_offset = _render_offset;
	position = _position;
	frames = _frames;
	duration = _duration;

	if (_type == "play_once")
		type = PLAY_ONCE;
	else if (_type == "back_forth")
		type = BACK_FORTH;
	else if (_type == "looped")
		type = LOOPED;
	else
		cout << "Warning: animation type " << _type << " is unknown" << endl;

	if (type == PLAY_ONCE || type == LOOPED) {
		max_frame = frames * duration;
	}
	else if (type == BACK_FORTH) {
		mid_frame = frames * duration;
		max_frame = mid_frame + mid_frame;
	}
}

void Animation::advanceFrame() {

	if (type == PLAY_ONCE) {
		if (cur_frame < max_frame - 1) {
			cur_frame++;
		}
		else {
			timesPlayed = 1;
		}
		disp_frame = (cur_frame / duration) + position;
	}
	else if (type == LOOPED) {
		cur_frame++;
		if (cur_frame == max_frame) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}
		disp_frame = (cur_frame / duration) + position;

	}
	else if (type == BACK_FORTH) {
		cur_frame++;

		if (cur_frame == max_frame) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}
		if (cur_frame >= mid_frame) {
			disp_frame = (max_frame -1 - cur_frame) / duration + position;
		}
		else {
			disp_frame = cur_frame / duration + position;
		}
	}
}

Renderable Animation::getCurrentFrame(int direction) {
	Renderable r;

	// if the animation contains the spritesheet
	if (sprites != NULL) {
		r.sprite = sprites;
	}

	r.src.x = render_size.x * disp_frame;
	r.src.y = render_size.y * direction;
	r.src.w = render_size.x;
	r.src.h = render_size.y;
	r.offset.x = render_offset.x;
	r.offset.y = render_offset.y; // 112
	return r;
}

void Animation::reset() {
	cur_frame = 0;
	disp_frame = (cur_frame / duration) + position;
	timesPlayed = 0;
}

