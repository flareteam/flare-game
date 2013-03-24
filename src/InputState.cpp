/*
Copyright © 2011-2012 Clint Bellanger
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
 * class InputState
 *
 * Handles keyboard and mouse states
 */

#include <iostream>

#include "FileParser.h"
#include "InputState.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsDebug.h"
#include "UtilsParsing.h"

using namespace std;

InputState::InputState(void)
	: mx_vel(0)
	, my_vel(0)
	, done(false)
	, mouse()
	, last_key(0)
	, last_button(0)
	, scroll_up(false)
	, scroll_down(false)
	, mouse_emulation(false)
{
	SDL_EnableUNICODE(true);

	defaultQwertyKeyBindings();
	defaultJoystickBindings();

	for (int key=0; key<key_count; key++) {
		pressing[key] = false;
		lock[key] = false;
	}

	loadKeyBindings();
	setKeybindNames();
}


void InputState::defaultQwertyKeyBindings ()
{
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
}

void InputState::defaultJoystickBindings ()
{
	// most joystick buttons are unbound by default
	for (int key=0; key<key_count; key++) {
		binding_joy[key] = -1;
	}

	binding_joy[MAIN1] = 0;
	binding_joy[MAIN2] = 1;
	binding_joy[ACCEPT] = 2;
	binding_joy[CANCEL] = 3;
	binding_joy[CHARACTER] = 4;
	binding_joy[INVENTORY] = 5;
	binding_joy[LOG] = 6;
	binding_joy[POWERS] = 7;
}

/**
 * Key bindings are found in config/keybindings.txt
 */
void InputState::loadKeyBindings() {

	FileParser infile;

	if (!infile.open(PATH_CONF + FILE_KEYBINDINGS)) {
		if (!infile.open(mods->locate("engine/default_keybindings.txt").c_str())) {
			saveKeyBindings();
			return;
		} else saveKeyBindings();
	}

	while (infile.next()) {

		int key1 = eatFirstInt(infile.val, ',');
		int key2 = toInt(infile.val);

		int cursor = -1;

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
	outfile.open((PATH_CONF + FILE_KEYBINDINGS).c_str(), ios::out);

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
		outfile << "character=" << binding[CHARACTER] << "," << binding_alt[CHARACTER] << "\n";
		outfile << "inventory=" << binding[INVENTORY] << "," << binding_alt[INVENTORY] << "\n";
		outfile << "powers=" << binding[POWERS] << "," << binding_alt[POWERS] << "\n";
		outfile << "log=" << binding[LOG] << "," << binding_alt[LOG] << "\n";
		outfile << "ctrl=" << binding[CTRL] << "," << binding_alt[CTRL] << "\n";
		outfile << "shift=" << binding[SHIFT] << "," << binding_alt[SHIFT] << "\n";
		outfile << "delete=" << binding[DEL] << "," << binding_alt[DEL] << "\n";

		if (outfile.bad()) fprintf(stderr, "Unable to write keybindings config file. No write access or disk is full!\n");
		outfile.close();
		outfile.clear();
	}

}

void InputState::handle(bool dump_event) {
	SDL_Event event;

	SDL_GetMouseState(&mouse.x, &mouse.y);

	static bool joyReverseAxisX;
	static bool joyReverseAxisY;
	static bool joyHasMovedX;
	static bool joyHasMovedY;
	static int joyLastPosX;
	static int joyLastPosY;
	int joyAxisXval;
	int joyAxisYval;

	inkeys = "";

	/* Check for events */
	while (SDL_PollEvent (&event)) {

		if (dump_event) {
			cout << event << endl;
		}

		// grab symbol keys
		if (event.type == SDL_KEYDOWN) {
			int ch = event.key.keysym.unicode;
			// if it is printable char then write its utf-8 representation
			if (ch >= 0x800) {
				inkeys += (char) ((ch >> 12) | 0xe0);
				inkeys += (char) (((ch >> 6) & 0x3f) | 0x80);
				inkeys += (char) ((ch & 0x3f) | 0x80);
			} else if (ch >= 0x80) {
				inkeys += (char) ((ch >> 6) | 0xc0);
				inkeys += (char) ((ch & 0x3f) | 0x80);
			} else if (ch >= 32 && ch != 127) {
				inkeys += (char)ch;
			}
		}

		switch (event.type) {

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_WHEELUP) {
					scroll_up = true;
				} else if (event.button.button == SDL_BUTTON_WHEELDOWN) {
					scroll_down = true;
				}else {
					for (int key=0; key<key_count; key++) {
						if (event.button.button == binding[key] || event.button.button == binding_alt[key]) {
							pressing[key] = true;
						}
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
				last_button = event.button.button;
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
				last_key = event.key.keysym.sym;
				break;
			/*
			case SDL_JOYAXISMOTION:
				// Reading joystick from SDL_JOYAXISMOTION is slow. Joystick analog input is handled by SDL_JoystickGetAxis() now.
				break;
			*/
			case SDL_JOYHATMOTION:
				if(JOYSTICK_DEVICE == event.jhat.which && ENABLE_JOYSTICK)
				{
					switch (event.jhat.value) {
						case SDL_HAT_CENTERED:
							pressing[UP] = false;
							lock[UP] = false;
							pressing[DOWN] = false;
							lock[DOWN] = false;
							pressing[LEFT] = false;
							lock[LEFT] = false;
							pressing[RIGHT] = false;
							lock[RIGHT] = false;
							break;
						case SDL_HAT_UP:
							pressing[UP] = true;
							pressing[DOWN] = false;
							lock[DOWN] = false;
							pressing[LEFT] = false;
							lock[LEFT] = false;
							pressing[RIGHT] = false;
							lock[RIGHT] = false;
							break;
						case SDL_HAT_DOWN:
							pressing[UP] = false;
							lock[UP] = false;
							pressing[DOWN] = true;
							pressing[LEFT] = false;
							lock[LEFT] = false;
							pressing[RIGHT] = false;
							lock[RIGHT] = false;
							break;
						case SDL_HAT_LEFT:
							pressing[UP] = false;
							lock[UP] = false;
							pressing[DOWN] = false;
							lock[DOWN] = false;
							pressing[LEFT] = true;
							pressing[RIGHT] = false;
							lock[RIGHT] = false;
							break;
						case SDL_HAT_RIGHT:
							pressing[UP] = false;
							lock[UP] = false;
							pressing[DOWN] = false;
							lock[DOWN] = false;
							pressing[LEFT] = false;
							lock[LEFT] = false;
							pressing[RIGHT] = true;
							break;
						case SDL_HAT_LEFTUP:
							pressing[UP] = true;
							pressing[DOWN] = false;
							lock[DOWN] = false;
							pressing[LEFT] = true;
							pressing[RIGHT] = false;
							lock[RIGHT] = false;
							break;
						case SDL_HAT_LEFTDOWN:
							pressing[UP] = false;
							lock[UP] = false;
							pressing[DOWN] = true;
							pressing[LEFT] = true;
							pressing[RIGHT] = false;
							lock[RIGHT] = false;
							break;
						case SDL_HAT_RIGHTUP:
							pressing[UP] = true;
							pressing[DOWN] = false;
							lock[DOWN] = false;
							pressing[LEFT] = false;
							lock[LEFT] = false;
							pressing[RIGHT] = true;
							break;
						case SDL_HAT_RIGHTDOWN:
							pressing[UP] = false;
							lock[UP] = false;
							pressing[DOWN] = true;
							pressing[LEFT] = false;
							lock[LEFT] = false;
							pressing[RIGHT] = true;
							break;
					}
				}
				break;
			case SDL_JOYBUTTONDOWN:
				if(JOYSTICK_DEVICE == event.jbutton.which && ENABLE_JOYSTICK)
				{
					for (int key=0; key<key_count; key++) {
						if (event.jbutton.button == binding_joy[key]) {
							pressing[key] = true;
						}
					}
				}
				break;
			case SDL_JOYBUTTONUP:
				if(JOYSTICK_DEVICE == event.jbutton.which && ENABLE_JOYSTICK)
				{
					for (int key=0; key<key_count; key++) {
						if (event.jbutton.button == binding_joy[key]) {
							pressing[key] = false;
							lock[key] = false;
						}
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

	// joystick analog input
	if(ENABLE_JOYSTICK)
	{
		joyAxisXval = SDL_JoystickGetAxis(joy, 0);
		joyAxisYval = SDL_JoystickGetAxis(joy, 1);

		// axis 0
		if(joyAxisXval < -JOY_DEADZONE)
		{
			if(!joyReverseAxisX)
			{
				if(joyLastPosX == JOY_POS_RIGHT)
				{
					joyHasMovedX = 0;
				}
			}
			else
			{
				if(joyLastPosX == JOY_POS_LEFT)
				{
					joyHasMovedX = 0;
				}
			}
			if(joyHasMovedX == 0)
			{
				if(!joyReverseAxisX)
				{
					pressing[LEFT] = true;
					pressing[RIGHT] = false;
					lock[RIGHT] = false;
					joyLastPosX = JOY_POS_LEFT;
				}
				else
				{
					pressing[RIGHT] = true;
					pressing[LEFT] = false;
					lock[LEFT] = false;
					joyLastPosX = JOY_POS_RIGHT;
				}
				joyHasMovedX = 1;
			}
		}
		if(joyAxisXval > JOY_DEADZONE)
		{
			if(!joyReverseAxisX)
			{
				if(joyLastPosX == JOY_POS_LEFT)
				{
					joyHasMovedX = 0;
				}
			}
			else
			{
				if(joyLastPosX == JOY_POS_RIGHT)
				{
					joyHasMovedX = 0;
				}
			}
			if(joyHasMovedX == 0)
			{
				if(!joyReverseAxisX)
				{
					pressing[RIGHT] = true;
					pressing[LEFT] = false;
					lock[LEFT] = false;
					joyLastPosX = JOY_POS_RIGHT;
				}
				else
				{
					pressing[LEFT] = true;
					pressing[RIGHT] = false;
					lock[RIGHT] = false;
					joyLastPosX = JOY_POS_LEFT;
				}
				joyHasMovedX = 1;
			}
		}
		if((joyAxisXval >= -JOY_DEADZONE) && (joyAxisXval < JOY_DEADZONE))
		{
			pressing[LEFT] = false;
			lock[LEFT] = false;
			pressing[RIGHT] = false;
			lock[RIGHT] = false;
			joyHasMovedX = 0;
			joyLastPosX = JOY_POS_CENTER;
		}

		// axis 1
		if(joyAxisYval < -JOY_DEADZONE)
		{
			if(!joyReverseAxisY)
			{
				if(joyLastPosY == JOY_POS_DOWN)
				{
					joyHasMovedY = 0;
				}
			}
			else
			{
				if(joyLastPosY == JOY_POS_UP)
				{
					joyHasMovedY = 0;
				}
			}
			if(joyHasMovedY == 0)
			{
				if(!joyReverseAxisY)
				{
					pressing[UP] = true;
					pressing[DOWN] = false;
					lock[DOWN] = false;
					joyLastPosY = JOY_POS_UP;
				}
				else
				{
					pressing[DOWN] = true;
					pressing[UP] = false;
					lock[UP] = false;
					joyLastPosY = JOY_POS_DOWN;
				}
				joyHasMovedY = 1;
			}
		}
		if(joyAxisYval > JOY_DEADZONE)
		{
			if(!joyReverseAxisY)
			{
				if(joyLastPosY == JOY_POS_UP)
				{
					joyHasMovedY = 0;
				}
			}
			else
			{
				if(joyLastPosY == JOY_POS_DOWN)
				{
					joyHasMovedY = 0;
				}
			}
			if(joyHasMovedY == 0)
			{
				if(!joyReverseAxisY)
				{
					pressing[DOWN] = true;
					pressing[UP] = false;
					lock[UP] = false;
					joyLastPosY = JOY_POS_DOWN;
				}
				else
				{
					pressing[UP] = true;
					pressing[DOWN] = false;
					lock[DOWN] = false;
					joyLastPosY = JOY_POS_UP;
				}
				joyHasMovedY = 1;
			}
		}
		if((joyAxisYval >= -JOY_DEADZONE) && (joyAxisYval < JOY_DEADZONE))
		{
			pressing[UP] = false;
			lock[UP] = false;
			pressing[DOWN] = false;
			lock[DOWN] = false;
			joyHasMovedY = 0;
			joyLastPosY = JOY_POS_CENTER;
		}

		mouseEmulation();
	}
}

void InputState::resetScroll() {
	scroll_up = false;
	scroll_down = false;
}

void InputState::enableMouseEmulation() {
	if (ENABLE_JOYSTICK && !mouse_emulation) {
		mouse_emulation = true;
		SDL_WarpMouse(VIEW_W_HALF,VIEW_H_HALF);
	}
}

void InputState::disableMouseEmulation() {
	if (ENABLE_JOYSTICK && mouse_emulation) {
		mouse_emulation = false;
		SDL_WarpMouse(VIEW_W-1,VIEW_H-1);
	}
}

void InputState::mouseEmulation() {
	if (!mouse_emulation) return;

	if (pressing[UP] && my_vel > -MOUSE_EMU_VEL) my_vel--;
	else if (!pressing[UP] && my_vel < 0) my_vel = 0;

	if (pressing[DOWN] && my_vel < MOUSE_EMU_VEL) my_vel++;
	else if (!pressing[DOWN] && my_vel > 0) my_vel = 0;

	if (pressing[LEFT] && mx_vel > -MOUSE_EMU_VEL) mx_vel--;
	else if (!pressing[LEFT] && mx_vel < 0) mx_vel = 0;

	if (pressing[RIGHT] && mx_vel < MOUSE_EMU_VEL) mx_vel++;
	else if (!pressing[RIGHT] && mx_vel > 0) mx_vel = 0;

	if (mx_vel != 0 || my_vel != 0) SDL_WarpMouse(mouse.x+mx_vel,mouse.y+my_vel);
}

void InputState::lockActionBar() {
	pressing[BAR_1] = false;
	pressing[BAR_2] = false;
	pressing[BAR_3] = false;
	pressing[BAR_4] = false;
	pressing[BAR_5] = false;
	pressing[BAR_6] = false;
	pressing[BAR_7] = false;
	pressing[BAR_8] = false;
	pressing[BAR_9] = false;
	pressing[BAR_0] = false;
	pressing[MAIN1] = false;
	pressing[MAIN2] = false;
	lock[BAR_1] = true;
	lock[BAR_2] = true;
	lock[BAR_3] = true;
	lock[BAR_4] = true;
	lock[BAR_5] = true;
	lock[BAR_6] = true;
	lock[BAR_7] = true;
	lock[BAR_8] = true;
	lock[BAR_9] = true;
	lock[BAR_0] = true;
	lock[MAIN1] = true;
	lock[MAIN2] = true;
}

void InputState::unlockActionBar() {
	lock[BAR_1] = false;
	lock[BAR_2] = false;
	lock[BAR_3] = false;
	lock[BAR_4] = false;
	lock[BAR_5] = false;
	lock[BAR_6] = false;
	lock[BAR_7] = false;
	lock[BAR_8] = false;
	lock[BAR_9] = false;
	lock[BAR_0] = false;
	lock[MAIN1] = false;
	lock[MAIN2] = false;
}

void InputState::setKeybindNames() {
	binding_name[0] = msg->get("Cancel");
	binding_name[1] = msg->get("Accept");
	binding_name[2] = msg->get("Up");
	binding_name[3] = msg->get("Down");
	binding_name[4] = msg->get("Left");
	binding_name[5] = msg->get("Right");
	binding_name[6] = msg->get("Bar1");
	binding_name[7] = msg->get("Bar2");
	binding_name[8] = msg->get("Bar3");
	binding_name[9] = msg->get("Bar4");
	binding_name[10] = msg->get("Bar5");
	binding_name[11] = msg->get("Bar6");
	binding_name[12] = msg->get("Bar7");
	binding_name[13] = msg->get("Bar8");
	binding_name[14] = msg->get("Bar9");
	binding_name[15] = msg->get("Bar0");
	binding_name[16] = msg->get("Character");
	binding_name[17] = msg->get("Inventory");
	binding_name[18] = msg->get("Powers");
	binding_name[19] = msg->get("Log");
	binding_name[20] = msg->get("Main1");
	binding_name[21] = msg->get("Main2");
	binding_name[22] = msg->get("Ctrl");
	binding_name[23] = msg->get("Shift");
	binding_name[24] = msg->get("Delete");

	mouse_button[0] = msg->get("lmb");
	mouse_button[1] = msg->get("mmb");
	mouse_button[2] = msg->get("rmb");
	mouse_button[3] = msg->get("wheel up");
	mouse_button[4] = msg->get("wheel down");
	mouse_button[5] = msg->get("mbx1");
	mouse_button[6] = msg->get("mbx2");
}

InputState::~InputState() {
}
