/**
 * class InputState
 *
 * Handles keyboard and mouse states
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef INPUT_STATE_H
#define INPUT_STATE_H

#define JOY_MIN         -32768
#define JOY_MAX         32767
#define JOY_DEADZONE    100

#include <string>
#include <fstream>
#include "SDL.h"
#include "FileParser.h"
#include "Utils.h"
#include "UtilsParsing.h"

// Input commands enum
const int CANCEL = 0;
const int ACCEPT = 1;
const int UP = 2;
const int DOWN = 3;
const int LEFT = 4;
const int RIGHT = 5;
const int BAR_1 = 6;
const int BAR_2 = 7;
const int BAR_3 = 8;
const int BAR_4 = 9;
const int BAR_5 = 10;
const int BAR_6 = 11;
const int BAR_7 = 12;
const int BAR_8 = 13;
const int BAR_9 = 14;
const int BAR_0 = 15;
const int CHARACTER = 16;
const int INVENTORY = 17;
const int POWERS = 18;
const int LOG = 19;
const int MAIN1 = 20;
const int MAIN2 = 21;
const int CTRL = 22;
const int SHIFT = 23;
const int DELETE = 24;

class InputState {
private:
	static const int key_count = 25;
	int binding[key_count];
	int binding_alt[key_count];
public:
	InputState(void);
	~InputState();
	void loadKeyBindings();
	void saveKeyBindings();
	void handle();

	bool pressing[key_count];
	bool lock[key_count];
	
	bool done;
	Point mouse;
	string inkeys;
};

#endif
