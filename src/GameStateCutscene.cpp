/*
Copyright © 2012-2013 Henrik Andersson

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

#include "GameStateCutscene.h"
#include "GameStatePlay.h"
#include "FileParser.h"

#include <iostream>
using namespace std;

Scene::Scene() : frame_counter(0)
	       , pause_frames(0)
	       , caption("")
	       , caption_size(0,0)
	       , art(NULL)
	       , sid(-1)
{
}

Scene::~Scene() {

	while(!components.empty())
		components.pop();

}

bool Scene::logic() {

	/* TODO: handle cancel input to skip scene */
	bool skip = false;
	if (inpt->pressing[MAIN1] && !inpt->lock[MAIN1]) {
		inpt->lock[MAIN1] = true;
		skip = true;
	}

	/* Pause until specified frame */
	if (!skip && pause_frames != 0 && frame_counter < pause_frames) {
		++frame_counter;
		return true;
	}

	/* parse scene components until next pause */
	while (!components.empty() && components.front().type != "pause") {

		if (components.front().type == "caption") {

			font->setFont("font_captions");
			caption = components.front().s;
			caption_size = font->calc_size(caption, VIEW_W * 0.8f);

		} else if (components.front().type == "image") {

			if (art)
				SDL_FreeSurface(art);

			art = components.front().i;

			art_dest.x = (VIEW_W/2) - (art->w/2);
			art_dest.y = (VIEW_H/2) - (art->h/2);
			art_dest.w = art->w;
			art_dest.h = art->h;

		} else if (components.front().type == "soundfx") {
			if (sid != 0)
				snd->unload(sid);

			sid = snd->load(components.front().s, "Cutscenes");
			snd->play(sid);
		}

		components.pop();
	}

	/* check if current scene has reached the end */
	if (components.empty())
		return false;

	/* setup frame pausing */
	frame_counter = 0;
	pause_frames = components.front().x;
	components.pop();

	return true;
}

void Scene::render() {
	SDL_Rect r = art_dest;
	if (art != NULL)
		SDL_BlitSurface(art, NULL, screen, &r);

	if (caption != "") {
		font->setFont("font_captions");
		font->renderShadowed(caption, screen->w / 2, screen->h - (caption_size.y*2),
				     JUSTIFY_CENTER,
				     screen, FONT_WHITE);
	}
}

GameStateCutscene::GameStateCutscene(GameState *game_state) : previous_gamestate(game_state)
	, game_slot(-1)
{
	scale_graphics = false;
}

GameStateCutscene::~GameStateCutscene() {
}

void GameStateCutscene::logic() {

	if (scenes.empty())
	{
		if (game_slot != -1) {
			GameStatePlay *gsp = new GameStatePlay();
			gsp->resetGame();
			gsp->game_slot = game_slot;
			gsp->loadGame();

			previous_gamestate = gsp;
		}

		/* return to previous gamestate */
		delete requestedGameState;
		requestedGameState = previous_gamestate;
		return;
	}

	while (!scenes.empty() && !scenes.front().logic())
		scenes.pop();
}

void GameStateCutscene::render() {
	if (!scenes.empty())
		scenes.front().render();
}

bool GameStateCutscene::load(std::string filename) {
	FileParser infile;

	if (!infile.open(mods->locate("cutscenes/" + filename))) {
		cerr << "Unable to open cutscenes/" << filename << endl;
		return false;
	}

	// parse the cutscene file
	while (infile.next()) {

		if (infile.new_section) {
			if (infile.section == "scene")
				scenes.push(Scene());
		}

		if (infile.section == "scene") {
			SceneComponent sc;
			sc.type = "";

			if (infile.key == "caption") {
				sc.type = infile.key;
				sc.s = msg->get(infile.val);
			}
			else if (infile.key == "image") {
				sc.type = infile.key;
				sc.i = loadImage(infile.val);
				if (sc.i == NULL)
					sc.type = "";
			}
			else if (infile.key == "pause") {
				sc.type = infile.key;
				sc.x = toInt(infile.val);
			}
			else if (infile.key == "soundfx") {
				sc.type = infile.key;
				sc.s = infile.val;
			}

			if (sc.type != "")
				scenes.back().components.push(sc);

		} else {

			if (infile.key == "scale_gfx") {
				scale_graphics = toBool(infile.val);
			}

		}
	}

	if (scenes.empty())
	{
		cerr << "No scenes defined in cutscene file " << filename << endl;
		return false;
	}

	return true;
}

SDL_Surface *GameStateCutscene::loadImage(std::string filename) {

	std::string image_file = (mods->locate("images/"+ filename));
	SDL_Surface *image = IMG_Load(image_file.c_str());
	if (!image) {
		std::cerr << "Missing cutscene art reference: " << image_file << std::endl;
		return NULL;
	}

	/* scale image to fit height */
	if (scale_graphics) {
		float ratio = image->h/(float)image->w;
		SDL_Surface *art = scaleSurface(image, VIEW_W, VIEW_W*ratio);
		if (art == NULL)
			return image;

		SDL_FreeSurface(image);
		image = art;
	}

	return image;
}

