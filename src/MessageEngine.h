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

#include "GetText.h"
#include "Settings.h"
#include <map>

class MessageEngine {

private:
	std::map<std::string,std::string> messages;
	std::string str(int i);

public:
	MessageEngine();
	std::string get(const std::string& key);
	std::string get(const std::string& key, int i);
	std::string get(const std::string& key, const std::string& s);
	std::string get(const std::string& key, int i, const std::string& s);
	std::string get(const std::string& key, int i, int j);
};

#endif
