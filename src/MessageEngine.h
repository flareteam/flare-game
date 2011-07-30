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

#ifndef MESSAGE_ENGINE_H
#define MESSAGE_ENGINE_H

using namespace std;

#include "FileParser.h"
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

#endif
