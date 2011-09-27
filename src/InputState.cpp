/*
Copyright 2011 Clint Bellanger

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
 * class InputState
 *
 * Handles keyboard and mouse states
 */

#include "InputState.h"

using namespace std;

InputState::InputState(void) {

	SDL_EnableUNICODE(true);

	binding[CANCEL] = SDLK_ESCAPE;
	binding[ACCEPT] = SDLK_RETURN;
	binding[UP] = SDLK_w;
	binding[DOWN] = SDLK_s;
	binding[LEFT] = SDLK_a;
	binding[RIGHT] = SDLK_d;
	
	binding_alt[CANCEL] = SDLK_ESCAPE;
	binding_alt[ACCEPT] = SDLK_SPACE;
	binding_alt[UP] = SDLK_UP;
	binding_alt[DOWN] = SDLK_DOWN;
	binding_alt[LEFT] = SDLK_LEFT;
	binding_alt[RIGHT] = SDLK_RIGHT;
	
	binding[BAR_1] = binding_alt[BAR_1] = SDLK_1;
	binding[BAR_2] = binding_alt[BAR_2] = SDLK_2;
	binding[BAR_3] = binding_alt[BAR_3] = SDLK_3;
	binding[BAR_4] = binding_alt[BAR_4] = SDLK_4;
	binding[BAR_5] = binding_alt[BAR_5] = SDLK_5;
	binding[BAR_6] = binding_alt[BAR_6] = SDLK_6;
	binding[BAR_7] = binding_alt[BAR_7] = SDLK_7;
	binding[BAR_8] = binding_alt[BAR_8] = SDLK_8;
	binding[BAR_9] = binding_alt[BAR_9] = SDLK_9;
	binding[BAR_0] = binding_alt[BAR_0] = SDLK_0;
	
	binding[CHARACTER] = binding_alt[CHARACTER] = SDLK_c;
	binding[INVENTORY] = binding_alt[INVENTORY] = SDLK_i;
	binding[POWERS] = binding_alt[POWERS] = SDLK_p;
	binding[LOG] = binding_alt[LOG] = SDLK_l;
	
	binding[MAIN1] = binding_alt[MAIN1] = SDL_BUTTON_LEFT;
	binding[MAIN2] = binding_alt[MAIN2] = SDL_BUTTON_RIGHT;
	
	binding[CTRL] = SDLK_LCTRL;
	binding_alt[CTRL] = SDLK_RCTRL;
	binding[SHIFT] = SDLK_LSHIFT;
	binding_alt[SHIFT] = SDLK_RSHIFT;
	binding[DEL] = SDLK_DELETE;
	binding_alt[DEL] = SDLK_BACKSPACE;
	
	for (int key=0; key<key_count; key++) {
		pressing[key] = false;
		lock[key] = false;
	}
	done = false;
	
	loadKeyBindings();
	
	// Optionally ignore the Joystick subsystem
	if (!ENABLE_JOYSTICK) {
		SDL_JoystickEventState(SDL_IGNORE);
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}
}

/**
 * Key bindings are found in config/keybindings.txt
 */
void InputState::loadKeyBindings() {

	FileParser infile;	
	int key1;
	int key2;
	int cursor;

	if (!infile.open(PATH_CONF + "keybindings.txt")) {
		saveKeyBindings();
		return;
	}
	
	while (infile.next()) {

		key1 = eatFirstInt(infile.val, ',');
		key2 = atoi(infile.val.c_str());
		
		cursor = -1;
		
		if (infile.key == "cancel") cursor = CANCEL;
		else if (infile.key == "accept") cursor = ACCEPT;
		else if (infile.key == "up") cursor = UP;
		else if (infile.key == "down") cursor = DOWN;
		else if (infile.key == "left") cursor = LEFT;
		else if (infile.key == "right") cursor = RIGHT;
		else if (infile.key == "bar1") cursor = BAR_1;
		else if (infile.key == "bar2") cursor = BAR_2;
		else if (infile.key == "bar3") cursor = BAR_3;
		else if (infile.key == "bar4") cursor = BAR_4;
		else if (infile.key == "bar5") cursor = BAR_5;
		else if (infile.key == "bar6") cursor = BAR_6;
		else if (infile.key == "bar7") cursor = BAR_7;
		else if (infile.key == "bar8") cursor = BAR_8;
		else if (infile.key == "bar9") cursor = BAR_9;
		else if (infile.key == "bar0") cursor = BAR_0;
		else if (infile.key == "main1") cursor = MAIN1;
		else if (infile.key == "main2") cursor = MAIN2;
		else if (infile.key == "character") cursor = CHARACTER;
		else if (infile.key == "inventory") cursor = INVENTORY;
		else if (infile.key == "powers") cursor = POWERS;
		else if (infile.key == "log") cursor = LOG;
		else if (infile.key == "ctrl") cursor = CTRL;
		else if (infile.key == "shift") cursor = SHIFT;
		else if (infile.key == "delete") cursor = DEL;
		
		if (cursor != -1) {
			binding[cursor] = key1;
			binding_alt[cursor] = key2;
		}

	}
	infile.close();
}

/**
 * Write current key bindings to config file
 */
void InputState::saveKeyBindings() {
	ofstream outfile;
	outfile.open((PATH_CONF + "keybindings.txt").c_str(), ios::out);

	if (outfile.is_open()) {
	
		outfile << "cancel=" << binding[CANCEL] << "," << binding_alt[CANCEL] << "\n";
		outfile << "accept=" << binding[ACCEPT] << "," << binding_alt[ACCEPT] << "\n";
		outfile << "up=" << binding[UP] << "," << binding_alt[UP] << "\n";
		outfile << "down=" << binding[DOWN] << "," << binding_alt[DOWN] << "\n";
		outfile << "left=" << binding[LEFT] << "," << binding_alt[LEFT] << "\n";
		outfile << "right=" << binding[RIGHT] << "," << binding_alt[RIGHT] << "\n";
		outfile << "bar1=" << binding[BAR_1] << "," << binding_alt[BAR_1] << "\n";
		outfile << "bar2=" << binding[BAR_2] << "," << binding_alt[BAR_2] << "\n";
		outfile << "bar3=" << binding[BAR_3] << "," << binding_alt[BAR_3] << "\n";
		outfile << "bar4=" << binding[BAR_4] << "," << binding_alt[BAR_4] << "\n";
		outfile << "bar5=" << binding[BAR_5] << "," << binding_alt[BAR_5] << "\n";
		outfile << "bar6=" << binding[BAR_6] << "," << binding_alt[BAR_6] << "\n";
		outfile << "bar7=" << binding[BAR_7] << "," << binding_alt[BAR_7] << "\n";
		outfile << "bar8=" << binding[BAR_8] << "," << binding_alt[BAR_8] << "\n";
		outfile << "bar9=" << binding[BAR_9] << "," << binding_alt[BAR_9] << "\n";
		outfile << "bar0=" << binding[BAR_0] << "," << binding_alt[BAR_0] << "\n";
		outfile << "main1=" << binding[MAIN1] << "," << binding_alt[MAIN1] << "\n";
		outfile << "main2=" << binding[MAIN2] << "," << binding_alt[MAIN2] << "\n";
		outfile << "charater=" << binding[CHARACTER] << "," << binding_alt[CHARACTER] << "\n";
		outfile << "inventory=" << binding[INVENTORY] << "," << binding_alt[INVENTORY] << "\n";
		outfile << "powers=" << binding[POWERS] << "," << binding_alt[POWERS] << "\n";
		outfile << "log=" << binding[LOG] << "," << binding_alt[LOG] << "\n";
		outfile << "ctrl=" << binding[CTRL] << "," << binding_alt[CTRL] << "\n";
		outfile << "shift=" << binding[SHIFT] << "," << binding_alt[SHIFT] << "\n";
		outfile << "delete=" << binding[DEL] << "," << binding_alt[DEL] << "\n";
		
		outfile.close();
	}

}
	
void InputState::handle() {
	SDL_Event event;
	
	SDL_GetMouseState(&mouse.x, &mouse.y);
	
	static bool joyHasMovedX;
	static bool joyHasMovedY;
	static int fakeKeyX;
	static int fakeKeyY;

	inkeys = "";

	/* Check for events */
	while (SDL_PollEvent (&event)) {
	
		// grab ASCII keys
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.unicode >= 32 && event.key.keysym.unicode < 127)
				inkeys = inkeys + (char)event.key.keysym.unicode;
		}
	
		switch (event.type) {
				
			case SDL_MOUSEBUTTONDOWN:
				for (int key=0; key<key_count; key++) {
					if (event.button.button == binding[key] || event.button.button == binding_alt[key]) {
						pressing[key] = true;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				for (int key=0; key<key_count; key++) {
					if (event.button.button == binding[key] || event.button.button == binding_alt[key]) {
						pressing[key] = false;
						lock[key] = false;
					}
				}
				break;
			case SDL_KEYDOWN:
				for (int key=0; key<key_count; key++) {
					if (event.key.keysym.sym == binding[key] || event.key.keysym.sym == binding_alt[key]) {
						pressing[key] = true;
					}
				}
				break;
			case SDL_KEYUP:
				for (int key=0; key<key_count; key++) {
					if (event.key.keysym.sym == binding[key] || event.key.keysym.sym == binding_alt[key]) {
						pressing[key] = false;
						lock[key] = false;
					}
				}
				break;
			case SDL_JOYAXISMOTION:
				switch(event.jaxis.axis) {
					/* first analog */
					case 0:
						/* left */
						if ((joyHasMovedX == false) && (event.jaxis.value < -JOY_DEADZONE) && (event.jaxis.value > JOY_MIN)) {
							fakeKeyX = SDLK_LEFT;
							joyHasMovedX = true;
						}
						/* right */
						if ((joyHasMovedX == false) && (event.jaxis.value > JOY_DEADZONE) && (event.jaxis.value < JOY_MAX)) {
							fakeKeyX = SDLK_RIGHT;
							joyHasMovedX = true;
						}
						/* centered */
						if ((event.jaxis.value > -JOY_DEADZONE) && (event.jaxis.value < JOY_DEADZONE)) {
							joyHasMovedX = false;
						}
						break;
					case 1:
						/* up */
						if ((joyHasMovedY == false) && (event.jaxis.value < -JOY_DEADZONE) && (event.jaxis.value > JOY_MIN)) {
							fakeKeyY = SDLK_UP;
							joyHasMovedY = true;
						}
						/* down */
						if ((joyHasMovedY == false) && (event.jaxis.value > JOY_DEADZONE) && (event.jaxis.value < JOY_MAX)) {
							fakeKeyY = SDLK_DOWN;
							joyHasMovedY = true;
						}
						/* centered */
						if ((event.jaxis.value > -JOY_DEADZONE) && (event.jaxis.value < JOY_DEADZONE)) {
							joyHasMovedY = false;
						}
						break;
					/* second analog */
					case 2:
						break;
					case 4:
						break;
				}
				for (int key=0; key<key_count; key++) {
					if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
						if (joyHasMovedX) {
							pressing[key] = true;
						}
						else {
							pressing[key] = false;
							lock[key] = false;
						}
					}
					if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
						if (joyHasMovedY) {
							pressing[key] = true;
						}
						else {
							pressing[key] = false;
							lock[key] = false;
						}
					}
				}
				break;

			case SDL_JOYHATMOTION:
				switch (event.jhat.value) {
					case SDL_HAT_CENTERED:
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = false;
								lock[key] = false;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = false;
								lock[key] = false;
							}
						}
						break;
					case SDL_HAT_UP:
						fakeKeyY = SDLK_UP;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = false;
								lock[key] = false;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = true;
							}
						}
						break;
					case SDL_HAT_DOWN:
						fakeKeyY = SDLK_DOWN;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = false;
								lock[key] = false;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = true;
							}
						}
						break;
					case SDL_HAT_LEFT:
						fakeKeyX = SDLK_LEFT;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = true;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = false;
								lock[key] = false;
							}
						}
						break;
					case SDL_HAT_RIGHT:
						fakeKeyX = SDLK_RIGHT;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = true;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = false;
								lock[key] = false;
							}
						}
						break;
					case SDL_HAT_LEFTUP:
						fakeKeyX = SDLK_LEFT;
						fakeKeyY = SDLK_UP;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = true;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = true;
							}
						}
						break;
					case SDL_HAT_LEFTDOWN:
						fakeKeyX = SDLK_LEFT;
						fakeKeyY = SDLK_DOWN;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = true;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = true;
							}
						}
						break;
					case SDL_HAT_RIGHTUP:
						fakeKeyX = SDLK_RIGHT;
						fakeKeyY = SDLK_UP;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = true;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = true;
							}
						}
						break;
					case SDL_HAT_RIGHTDOWN:
						fakeKeyX = SDLK_RIGHT;
						fakeKeyY = SDLK_DOWN;
						for (int key = 0; key<key_count; key++) {
							if (fakeKeyX == binding[key] || fakeKeyX == binding_alt[key]) {
								pressing[key] = true;
							}
							if (fakeKeyY == binding[key] || fakeKeyY == binding_alt[key]) {
								pressing[key] = true;
							}
						}
						break;
				}
				break;
			case SDL_JOYBUTTONDOWN:
				for (int key=0; key<key_count; key++) {
					if (event.jbutton.button == binding[key] || event.jbutton.button == binding_alt[key]) {
						pressing[key] = true;
					}
				}
				break;
			case SDL_JOYBUTTONUP:
				for (int key=0; key<key_count; key++) {
					if (event.jbutton.button == binding[key] || event.jbutton.button == binding_alt[key]) {
						pressing[key] = false;
						lock[key] = false;
					}
				}
				break;
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
		}
	}

		
}

InputState::~InputState() {
}
