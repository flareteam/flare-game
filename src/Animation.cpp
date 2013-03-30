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
	, max_kinds(0)
	, additional_data(0)
	, times_played(0)
	, gfx()
	, render_offset()
	, duration()
	, active_frames()
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
	, max_kinds(a.max_kinds)
	, additional_data(a.additional_data)
	, times_played(0)
	, gfx(std::vector<SDL_Rect>(a.gfx))
	, render_offset(std::vector<Point>(a.render_offset))
	, duration(std::vector<unsigned short>(a.duration))
	, active_frames(std::vector<short>(a.active_frames))
{}

void Animation::setupUncompressed(Point _render_size, Point _render_offset, int _position, int _frames, int _duration, unsigned short _maxkinds) {
	setup(_frames, _duration, _maxkinds);

	for (unsigned short i = 0 ; i < _frames; i++) {
		int base_index = max_kinds*i;
		for (unsigned short kind = 0 ; kind < max_kinds; kind++) {
			gfx[base_index + kind].x = _render_size.x * (_position + i);
			gfx[base_index + kind].y = _render_size.y * kind;
			gfx[base_index + kind].w = _render_size.x;
			gfx[base_index + kind].h = _render_size.y;
			render_offset[base_index + kind].x = _render_offset.x;
			render_offset[base_index + kind].y = _render_offset.y;
		}
	}
}

void Animation::setup(unsigned short _frames, unsigned short _duration, unsigned short _maxkinds) {
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
	max_kinds = _maxkinds;
	times_played = 0;

	active_frames.push_back(number_frames/2);

	gfx.resize(max_kinds*_frames);
	render_offset.resize(max_kinds*_frames);
	duration.resize(max_kinds*_frames);
	for (unsigned short i = 0; i < duration.size(); i++)
		duration[i] = _duration;
}

void Animation::addFrame(	unsigned short index,
							unsigned short kind,
							SDL_Rect sdl_rect,
							Point _render_offset) {

	if (index > gfx.size()/max_kinds) {
		fprintf(stderr, "WARNING: Animation(%s) adding rect(%d, %d, %d, %d) to frame index(%u) out of bounds. must be in [0, %d]\n",
		name.c_str(), sdl_rect.x, sdl_rect.y, sdl_rect.w, sdl_rect.h, index, (int)gfx.size()/max_kinds);
		return;
	}
	if (kind > max_kinds-1) {
		fprintf(stderr, "WARNING: Animation(%s) adding rect(%d, %d, %d, %d) to frame(%u) kind(%u) out of bounds. must be in [0, %d]\n",
		name.c_str(), sdl_rect.x, sdl_rect.y, sdl_rect.w, sdl_rect.h, index, kind, max_kinds-1);
		return;
	}
	gfx[max_kinds*index+kind] = sdl_rect;
	render_offset[max_kinds*index+kind] = _render_offset;
}

void Animation::advanceFrame() {

	if (!this)
		return;

	cur_frame_duration++;

	// Some entity state changes are triggered when the current frame is the last frame.
	// Even if those state changes are not handled properly, do not permit current frame to exceed last frame.
	if (cur_frame < number_frames-1) cur_frame++;

	if (cur_frame_duration >= duration[cur_frame_index]) {
		cur_frame_duration = 0;
		unsigned short last_base_index = (gfx.size()/max_kinds)-1;
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

Renderable Animation::getCurrentFrame(int kind) {
	Renderable r;
	if (this) {
		const int index = (max_kinds*cur_frame_index) + kind;
		r.src.x = gfx[index].x;
		r.src.y = gfx[index].y;
		r.src.w = gfx[index].w;
		r.src.h = gfx[index].h;
		r.offset.x = render_offset[index].x;
		r.offset.y = render_offset[index].y;
		r.sprite = sprite;
	}
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

void Animation::setActiveFrames(const std::vector<short> &_active_frames) {
	if (_active_frames.size() == 1 && _active_frames[0] == -1) {
		for (short i = 0; i < number_frames; ++i)
			active_frames.push_back(i);
	}
	else
		active_frames = std::vector<short>(_active_frames);
}
