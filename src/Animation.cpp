/**
 * class Animation
 *
 * The Animation class handles the logic of advancing frames based on the animation type
 * and returning a renderable frame.
 * 
 * The intention with the class is to keep it as flexible as possible so that the animations
 * can be used not only for character animations but any animated in-game objects.
 *
 * @author kitano
 * @license GPL
 */

#include "Animation.h"

Animation::Animation(std::string _name, Point _render_size, Point _render_offset, int _position, int _frames, int _duration, std::string _type)
	: name(_name), sprites(NULL),
	  render_size(_render_size), render_offset(_render_offset),
	  position(_position), frames(_frames), duration(_duration), type(_type),
	  cur_frame(0), disp_frame(0), mid_frame(0), max_frame(0), timesPlayed(0) {

	if (type == "play_once" || type == "looped") {
		max_frame = frames * duration;
	}
	else if (type == "back_forth") {
		mid_frame = frames * duration;
		max_frame = mid_frame + mid_frame;
	
	}
}

void Animation::advanceFrame() {

	if (type == "play_once") {
		if (cur_frame < max_frame - 1) {
			cur_frame++;
		}
		else {
			timesPlayed = 1;
		}
		disp_frame = (cur_frame / duration) + position;
	}
	else if (type == "looped") {
		cur_frame++;
		if (cur_frame == max_frame) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}
		disp_frame = (cur_frame / duration) + position;	

	}
	else if (type == "back_forth") {
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
	r.object_layer = true;
	return r;
}

void Animation::reset() {
	cur_frame = 0;
	disp_frame = (cur_frame / duration) + position;
	timesPlayed = 0;
}

