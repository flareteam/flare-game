/*
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


#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"

#include "ImageManager.h"
#include "FileParser.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsParsing.h"

#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

Animation *AnimationSet::getAnimation(const std::string &_name)
{
	if (!loaded)
		load();
	for (size_t i = 0; i < animations.size(); i++)
		if (animations[i]->getName() == _name)
			return new Animation(*animations[i]);
	return new Animation(*defaultAnimation);
}

Animation *AnimationSet::getAnimation()
{
	if (!loaded)
		load();
	return new Animation(*defaultAnimation);
}

AnimationSet::AnimationSet(const std::string &animationname)
 : name(animationname)
 , loaded(false)
 , animations()
 , sprite(NULL)
{
	defaultAnimation = new Animation("default", "play_once", NULL);
	defaultAnimation->setupUncompressed(Point(), Point(), 0, 1, 0);
}

void AnimationSet::load() {
	assert(!loaded);
	loaded = true;

	FileParser parser;
	const string filename = mods->locate(name);

	if (!parser.open(filename, "Error loading animation definition: " + name))
		return;

	string _name = "";
	int position = 0;
	int frames = 0;
	int duration = 0;
	Point render_size;
	Point render_offset;
	string type = "";
	string starting_animation = "";
	bool first_section=true;
	bool compressed_loading=false; // is reset every section to false, set by frame keyword
	Animation *newanim = NULL;
	vector<short> active_frames;

	// Parse the file and on each new section create an animation object from the data parsed previously
	parser.next();

	do {
		// create the animation if finished parsing a section
		if (parser.new_section) {
			if (!first_section && !compressed_loading) {
				Animation *a = new Animation(_name, type, sprite);
				a->setupUncompressed(render_size, render_offset, position, frames, duration);
				if (!active_frames.empty())
					a->setActiveFrames(active_frames);
				active_frames.clear();
				animations.push_back(a);
			}
			first_section = false;
			compressed_loading = false;
		}
		if (parser.key == "image") {
			if (sprite) {
				printf("multiple images specified in %s, dragons be here!\n", name.c_str());
				SDL_Quit();
				exit(128);
			}
			imagefile = parser.val;
			imag->increaseCount(imagefile);
			sprite = imag->getSurface(imagefile);
		}
		else if (parser.key == "position") {
			position = toInt(parser.val);
		}
		else if (parser.key == "frames") {
			frames = toInt(parser.val);
		}
		else if (parser.key == "duration") {
			duration = toInt(parser.val);

			// TEMP: if an animation is too fast, display one frame per fps anyway
			if (duration < 1) duration=1;
		}
		else if (parser.key == "type")
			type = parser.val;
		else if (parser.key == "render_size") {
			render_size.x = toInt(parser.nextValue());
			render_size.y = toInt(parser.nextValue());
		}
		else if (parser.key == "render_offset") {
			render_offset.x = toInt(parser.nextValue());
			render_offset.y = toInt(parser.nextValue());
		}
		else if (parser.key == "active_frame") {
			active_frames.clear();
			string nv = parser.nextValue();
			if (nv == "all") {
				active_frames.push_back(-1);
			}
			else {
				while (nv != "") {
					active_frames.push_back(toInt(nv));
					nv = parser.nextValue();
				}
				sort(active_frames.begin(), active_frames.end());
				active_frames.erase(unique(active_frames.begin(), active_frames.end()), active_frames.end());
			}
		}
		else if (parser.key == "frame") {
			if (compressed_loading == false) { // first frame statement in section
				newanim = new Animation(_name, type, sprite);
				newanim->setup(frames, duration);
				if (!active_frames.empty())
					newanim->setActiveFrames(active_frames);
				active_frames.clear();
				animations.push_back(newanim);
				compressed_loading = true;
			}
			// frame = index, direction, x, y, w, h, offsetx, offsety
			SDL_Rect r;
			Point offset;
			const int index = toInt(parser.nextValue());
			const int direction = toInt(parser.nextValue());
			r.x = toInt(parser.nextValue());
			r.y = toInt(parser.nextValue());
			r.w = toInt(parser.nextValue());
			r.h = toInt(parser.nextValue());
			offset.x = toInt(parser.nextValue());
			offset.y = toInt(parser.nextValue());
			newanim->addFrame(index, direction, r, offset);
		}
		else {
			fprintf(stderr, "animations definitions (%s): Key %s not supported!\n", filename.c_str(), parser.key.c_str());
		}

		if (_name == "") {
			// This is the first animation
			starting_animation = parser.section;
		}
		_name = parser.section;
	}
	while (parser.next());

	if (!compressed_loading) {
		// add final animation
		Animation *a = new Animation(_name, type, sprite);
		a->setupUncompressed(render_size, render_offset, position, frames, duration);
		if (!active_frames.empty())
			a->setActiveFrames(active_frames);
		active_frames.clear();
		animations.push_back(a);
	}

	if (starting_animation != "") {
		Animation *a = getAnimation(starting_animation);
		delete defaultAnimation;
		defaultAnimation = a;
	}
}

AnimationSet::~AnimationSet() {
	if (imagefile != "") imag->decreaseCount(imagefile);
	for (unsigned i = 0; i < animations.size(); ++i)
		delete animations[i];
	delete defaultAnimation;
}

