/*
Copyright © 2011-2012 Thane Brimhall & Clint Bellanger

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
 * GetText
 *
 * Abstract the generic key-value pair handling from the gettext .po file format.
 * We don't need full gettext functionality (yet) so this is a simple solution.
 */


#pragma once
#ifndef GET_TEXT_H
#define GET_TEXT_H

#include <fstream>
#include <string>

class GetText {
private:
	std::ifstream infile;
	std::string line;
	std::string sanitize(std::string input);

public:
	GetText();
	~GetText();

	bool open(const std::string& filename);
	void close();
	bool next();

	std::string key;
	std::string val;
	bool fuzzy;

};

#endif
