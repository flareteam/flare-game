/**
 * class MessageEngine
 *
 * The MessageEngine class loads all of FLARE's internal messages from a configuration file
 * and returns them as human-readable strings.
 * 
 * This class is primarily used for making sure FLARE is flexible and translatable.
 *
 * @author Thane "pennomi" Brimhall
 * @license GPL
 */

#include "MessageEngine.h"
#include <sstream>
#include <iostream>

MessageEngine::MessageEngine() {
	FileParser infile;
	if (infile.open(PATH_DATA + "engine/messages.txt")) {
		while (infile.next()) {
				messages.insert(pair<string,string>(infile.key, infile.val));
		}
	}
}
/*
 * Each of the get() functions returns the mapped value
 * They differ only on which variables they replace in the string - strings replace %s, integers replace %d
 */
string MessageEngine::get(string key) {
	string msg = messages[key];
	if (msg == "") cout << "Warning! Message '" << key << "' not found in /engine/messages.txt" << endl;
	return msg;
}

string MessageEngine::get(string key, int i) {
	string msg = messages[key];
	if (msg == "") cout << "Warning! Message '" << key << "' not found in /engine/messages.txt" << endl;
	int index = msg.find("%d");
	if (index != string::npos) msg = msg.replace(index, 2, str(i));
	return msg;
}

string MessageEngine::get(string key, string s) {
	string msg = messages[key];
	if (msg == "") cout << "Warning! Message '" << key << "' not found in /engine/messages.txt" << endl;
	int index = msg.find("%s");
	if (index != string::npos) msg = msg.replace(index, 2, s);
	return msg;
}

string MessageEngine::get(string key, int i, string s) {
	string msg = messages[key];
	if (msg == "") cout << "Warning! Message '" << key << "' not found in /engine/messages.txt" << endl;
	int index = msg.find("%d");
	if (index != string::npos) msg = msg.replace(index, 2, str(i));
	index = msg.find("%s");
	if (index != string::npos) msg = msg.replace(index, 2, s);
	return msg;
}

string MessageEngine::get(string key, int i, int j) {
	string msg = messages[key];
	if (msg == "") cout << "Warning! Message '" << key << "' not found in /engine/messages.txt" << endl;
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
