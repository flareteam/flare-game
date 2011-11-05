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
 * The MessageEngine class loads all of FLARE's internal messages from a configuration file
 * and returns them as human-readable strings.
 * 
 * This class is primarily used for making sure FLARE is flexible and translatable.
 */

#include "MessageEngine.h"
#include <sstream>
#include <iostream>

MessageEngine *msg;

MessageEngine::MessageEngine() {
	GetText infile;
	if (infile.open(PATH_DATA + "languages/engine." + LANGUAGE + ".po")) {
		while (infile.next()) {
			messages.insert(pair<string,string>(infile.key, infile.val));
		}
        infile.close();
	}
	if (infile.open(PATH_DATA + "languages/data." + LANGUAGE + ".po")) {
		while (infile.next()) {
			messages.insert(pair<string,string>(infile.key, infile.val));
		}
        infile.close();
	}
}
/*
 * Each of the get() functions returns the mapped value
 * They differ only on which variables they replace in the string - strings replace %s, integers replace %d
 */
string MessageEngine::get(string key) {
	string msg = messages[key];
	if (msg == "") msg = key;
	return msg;
}

string MessageEngine::get(string key, int i) {
	string msg = messages[key];
	if (msg == "") msg = key;
	int index = msg.find("%d");
	if (index != string::npos) msg = msg.replace(index, 2, str(i));
	return msg;
}

string MessageEngine::get(string key, string s) {
	string msg = messages[key];
	if (msg == "") msg = key;
	int index = msg.find("%s");
	if (index != string::npos) msg = msg.replace(index, 2, s);
	return msg;
}

string MessageEngine::get(string key, int i, string s) {
	string msg = messages[key];
	if (msg == "") msg = key;
	int index = msg.find("%d");
	if (index != string::npos) msg = msg.replace(index, 2, str(i));
	index = msg.find("%s");
	if (index != string::npos) msg = msg.replace(index, 2, s);
	return msg;
}

string MessageEngine::get(string key, int i, int j) {
	string msg = messages[key];
	if (msg == "") msg = key;
	int index = msg.find("%d");
	if (index != string::npos) msg = msg.replace(index, 2, str(i));
	index = msg.find("%d");
	if (index != string::npos) msg = msg.replace(index, 2, str(j));
	return msg;
}

// Changes an int into a string
string MessageEngine::str(int i){	
	stringstream ss;
	ss << i;
	return ss.str();
}
