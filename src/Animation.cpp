/**
 * class Animation
 *
 * The Animation class handles the logic of advancing frames based on the animation type
 * and returning a renderable frame.
 * 
 * Currently an Animation optionally hold a reference to the sprite sheet. It can be instantiated
 * without specifying this and the getCurrentFrame() function will then return a renderable
 * without a sprite.
 *
 * The intention with the class is to keep it as flexible as possible so that the animations
 * can be used not only for character animations but any animated in-game objects.
 *
 * @author kitano
 * @license GPL
 */

#include "Animation.h"

Animation::Animation(int _frameSize, int _position, int _frames, int _duration, std::string _type) : sprites(NULL), frameSize(_frameSize), position(_position), frames(_frames), duration(_duration), type(_type), cur_frame(0), disp_frame(0), mid_frame(0), max_frame(0), timesPlayed(0) {

	mid_frame = frames * duration;

	max_frame = mid_frame + mid_frame;
}

Animation::Animation(SDL_Surface* _sprites, int _frameSize, int _position, int _frames, int _duration, std::string _type) : sprites(_sprites), frameSize(_frameSize), position(_position), frames(_frames), duration(_duration), type(_type), cur_frame(0), disp_frame(0), mid_frame(0), max_frame(0), timesPlayed(0) {

	mid_frame = frames * duration;

	max_frame = mid_frame + mid_frame;
}


void Animation::advanceFrame() {

	if (type == "play_once") {
		if (cur_frame < max_frame - 1) {
			cur_frame++;
		}
		else {
			timesPlayed = 1;
		}
		max_frame = frames * duration;
		disp_frame = (cur_frame / duration) + position;
	}
	else if (type == "looped") {
		cur_frame++;
		max_frame = frames * duration;
		disp_frame = (cur_frame / duration) + position;	
		if (cur_frame >= max_frame - 1) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}
	}
	else if (type == "back_forth") {
		cur_frame++;
		max_frame = mid_frame + mid_frame;
		if (cur_frame >= mid_frame) {
			disp_frame = (max_frame -1 - cur_frame) / duration + position;
		}
		else {
			disp_frame = cur_frame / duration + position;
		}
		if (cur_frame >= max_frame - 1) {
			cur_frame = 0;
			//animation has completed one loop
			timesPlayed++;
		}
	}
}

Renderable Animation::getCurrentFrame(int direction) {
	Renderable r;

	// if the animation contains the spritesheet
	if (sprites != NULL) {
		r.sprite = sprites;
	}

	r.src.x = 128 * disp_frame;
	r.src.y = 128 * direction;
	r.src.w = 128;
	r.src.h = 128;
	r.offset.x = 64;
	r.offset.y = 96; // 112
	r.object_layer = true;
	return r;
}

void Animation::reset() {
	cur_frame = 0;
	timesPlayed = 0;
}

