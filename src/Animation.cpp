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

Animation::Animation(const std::string &_name, const std::string &_type, SDL_Surface *_sprite)
	: name(_name)
	, type(	_type == "play_once" ? PLAY_ONCE :
			_type == "back_forth" ? BACK_FORTH :
			_type == "looped" ? LOOPED :
			NONE)
	, sprite(_sprite)
	, number_frames(0)
	, cur_frame(0)
	, cur_frame_index(0)
	, cur_frame_duration(0)
	, additional_data(0)
	, times_played(0)
	, gfx(std::vector<SDL_Rect>())
	, render_offset(std::vector<Point>())
	, duration(std::vector<short>())
{
	if (type == NONE)
		cout << "Warning: animation type " << _type << " is unknown" << endl;
}

Animation::Animation(const Animation& a)
	: name(a.name)
	, type(a.type)
	, sprite(a.sprite)
	, number_frames(a.number_frames)
	, cur_frame(0)
	, cur_frame_index(a.cur_frame_index)
	, cur_frame_duration(a.cur_frame_duration)
	, additional_data(a.additional_data)
	, times_played(0)
	, gfx(std::vector<SDL_Rect>(a.gfx))
	, render_offset(std::vector<Point>(a.render_offset))
	, duration(std::vector<short>(a.duration))
{
}

void Animation::setupUncompressed(Point _render_size, Point _render_offset, int _position, int _frames, int _duration) {
	setup(_frames, _duration);


	for (unsigned short i = 0 ; i < _frames; i++) {
		int base_index = 8*i;
		for (unsigned short direction = 0 ; direction < 8; direction++) {
			gfx[base_index + direction].x = _render_size.x * (_position + i);
			gfx[base_index + direction].y = _render_size.y * direction;
			gfx[base_index + direction].w = _render_size.x;
			gfx[base_index + direction].h = _render_size.y;
			render_offset[base_index + direction].x = _render_offset.x;
			render_offset[base_index + direction].y = _render_offset.y;
		}
	}
}

void Animation::setup(unsigned short _frames, unsigned short _duration) {
	if (type == PLAY_ONCE) {
		number_frames = _frames * _duration;
		additional_data = 0;
	} else if (type == LOOPED) {
		number_frames = _frames * _duration;
		additional_data = 0;
	} else if (type == BACK_FORTH) {
		number_frames = 2 * _frames * _duration;
		additional_data = 1;
	}
	cur_frame = 0;
	cur_frame_index = 0;
	cur_frame_duration = 0;
	times_played = 0;

	gfx.resize(8*_frames);
	render_offset.resize(8*_frames);
	duration.resize(8*_frames);
	for (unsigned short i = 0; i < duration.size(); i++)
		duration[i] = _duration;
}

void Animation::addFrame(	unsigned short index,
							unsigned short direction,
							SDL_Rect sdl_rect,
							Point _render_offset) {

	if (index > gfx.size()/8) {
		fprintf(stderr, "WARNING: Animation(%s) adding rect(%d, %d, %d, %d) to frame index(%u) out of bounds. must be in [0, %d]\n",
		name.c_str(), sdl_rect.x, sdl_rect.y, sdl_rect.w, sdl_rect.h, index, (int)gfx.size()/8);
		return;
	}
	if (direction > 7) {
		fprintf(stderr, "WARNING: Animation(%s) adding rect(%d, %d, %d, %d) to frame(%u) direction(%u) out of bounds. must be in [0, 7]\n",
		name.c_str(), sdl_rect.x, sdl_rect.y, sdl_rect.w, sdl_rect.h, index, direction);
		return;
	}
	gfx[8*index+direction] = sdl_rect;
	render_offset[8*index+direction] = _render_offset;
}

void Animation::doneLoading() {
	// here we make sure
	gfx.reserve(gfx.size()); // position on the spritesheet to be used.
	render_offset.reserve(render_offset.size()); // "virtual point on the floor"
	duration.reserve(duration.size()); //duration of each individual image
}


void Animation::advanceFrame() {

	cur_frame_duration++;

	// Some entity state changes are triggered when the current frame is the last frame.
	// Even if those state changes are not handled properly, do not permit current frame to exceed last frame.
	if (cur_frame < number_frames-1) cur_frame++;

	if (cur_frame_duration >= duration[cur_frame_index]) {
		cur_frame_duration = 0;
		unsigned short last_base_index = (gfx.size()/8)-1;
		switch(type) {
		case PLAY_ONCE:

			if (cur_frame_index < last_base_index)
				cur_frame_index++;
			else
				times_played = 1;
			break;

		case LOOPED:
			if (cur_frame_index < last_base_index) {
				cur_frame_index++;
			}
			else {
				cur_frame_index = 0;
				cur_frame = 0;
				times_played++;
			}
			break;

		case BACK_FORTH:

			if (additional_data == 1) {
				if (cur_frame_index < last_base_index)
					cur_frame_index++;
				else
					additional_data = -1;
			}
			else if (additional_data == -1) {
				if (cur_frame_index > 0)
					cur_frame_index--;
				else {
					additional_data = 1;
					cur_frame = 0;
					times_played++;
				}
			}
			break;

		case NONE:
			break;
		}
	}
}

Renderable Animation::getCurrentFrame(int direction) {
	Renderable r;

	const int index = (8*cur_frame_index) + direction;
	r.src.x = gfx[index].x;
	r.src.y = gfx[index].y;
	r.src.w = gfx[index].w;
	r.src.h = gfx[index].h;
	r.offset.x = render_offset[index].x;
	r.offset.y = render_offset[index].y;
	r.sprite = sprite;
	return r;
}

void Animation::reset() {
	cur_frame = 0;
	cur_frame_duration = 0;
	cur_frame_index = 0;
	times_played = 0;
	additional_data = 1;
}

void Animation::syncTo(const Animation *other) {
	cur_frame = other->cur_frame;
	cur_frame_duration = other->cur_frame_duration;
	cur_frame_index = other->cur_frame_index;
	times_played = other->times_played;
	additional_data = other->additional_data;
}
