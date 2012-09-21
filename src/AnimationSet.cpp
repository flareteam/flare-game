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

#include "FileParser.h"
#include "SharedResources.h"
#include "Settings.h"
#include "UtilsParsing.h"

#include <string>

using namespace std;

Animation *AnimationSet::getAnimation(const std::string &_name)
{
    for (size_t i = 0; i < animations.size(); i++)
        if (animations[i]->getName() == _name)
            return new Animation(*animations[i]);
    return 0;
}

AnimationSet::AnimationSet(const string &filename)
 : animations(vector<Animation*>())
 , starting_animation("")
 , name(filename)
{
	FileParser parser;

	if (!parser.open(mods->locate(filename).c_str())) {
		cout << "Error loading animation definition file: " << filename << endl;
		SDL_Quit();
		exit(1);
	}

	string _name = "";
	int position = 0;
	int frames = 0;
	int duration = 0;
	Point render_size;
	Point render_offset;
	string type = "";
	bool first_section=true;
	bool compressed_loading=false; // is reset every section to false, set by frame keyword
	Animation *newanim;

	// Parse the file and on each new section create an animation object from the data parsed previously
	parser.next();

	do {
		// create the animation if finished parsing a section
		if (parser.new_section) {
			if (!first_section && !compressed_loading) {
				Animation *a = new Animation(_name, type);
				a->setupUncompressed(render_size, render_offset,  position, frames, duration);
				animations.push_back(a);
			}
			first_section = false;
			compressed_loading = false;
		}

		if (parser.key == "position") {
			position = toInt(parser.val);
		}
		else if (parser.key == "frames") {
			frames = toInt(parser.val);
		}
		else if (parser.key == "duration") {
			int ms_per_frame = toInt(parser.val);

			duration = (int)round((float)ms_per_frame / (1000.0 / (float)MAX_FRAMES_PER_SEC));
			// adjust duration according to the entity's animation speed
			//duration = (duration * 100) / stats.animationSpeed;

			// TEMP: if an animation is too fast, display one frame per fps anyway
			if (duration < 1) duration=1;
		}
		else if (parser.key == "type")
			type = parser.val;
		else if (parser.key == "render_size_x")
			render_size.x = toInt(parser.val);
		else if (parser.key == "render_size_y")
			render_size.y = toInt(parser.val);
		else if (parser.key == "render_offset_x")
			render_offset.x = toInt(parser.val);
		else if (parser.key == "render_offset_y")
			render_offset.y = toInt(parser.val);
		else if (parser.key == "active_frame")
			cout << "active frames in entities not supported" << endl;
		else if (parser.key == "frame") {
			if (compressed_loading == false) { // first frame statement in section
				newanim = new Animation(_name, type);
				newanim->setup(frames, duration);
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

		if (_name == "") {
			// This is the first animation
			starting_animation = parser.section;
		}
		_name = parser.section;
	}
	while (parser.next());

	if (!compressed_loading) {
		// add final animation
		Animation *a = new Animation(_name, type);
		a->setupUncompressed(render_size, render_offset, position, frames, duration);
		animations.push_back(a);
	}
}
