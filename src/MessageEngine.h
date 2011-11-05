/*
Copyright 2011 Thane Brimhall

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
 * class MessageEngine
 *
 * The MessageEngine class allows translation of messages in FLARE by comparing them to
 * .po files in a format similar to gettext.
 * 
 * This class is primarily used for making sure FLARE is flexible and translatable.
 */

#ifndef MESSAGE_ENGINE_H
#define MESSAGE_ENGINE_H

using namespace std;

#include "GetText.h"
#include "Settings.h"
#include <map>

class MessageEngine {

private:
	map<string,string> messages;
	string str(int i);

public:
	MessageEngine();
	string get(string key);
	string get(string key, int i);
	string get(string key, string s);
	string get(string key, int i, string s);
	string get(string key, int i, int j);

};

// The MessageEngine is initialized once,
// never changed, and used everywhere, so
// it works nicely as a global variable.
extern MessageEngine *msg;

#endif
