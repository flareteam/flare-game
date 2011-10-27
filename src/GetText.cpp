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

#include "GetText.h"

GetText::GetText() {
	line = "";
	key = "";
	val = "";
}

bool GetText::open(string filename) {
	infile.open(filename.c_str(), ios::in);
	return infile.is_open();
}

void GetText::close() {
	if (infile.is_open())
		infile.close();
}

/**
 * Advance to the next key pair
 *
 * @return false if EOF, otherwise true
 */
bool GetText::next() {

    key = "";
    val = "";
	
	while (!infile.eof()) {
		line = getLine(infile);

		// this is a key
		if (line.find("msgid") == 0) {
            // grab only what's contained in the quotes
            key = line.substr(6);
            key = trim(key, '"');
            if (key != "")
    			continue;
		}

		// this is a value
		if (line.find("msgstr") == 0) {
            // grab only what's contained in the quotes
            val = line.substr(7);
            val = trim(val, '"');

	        // handle keypairs
            if (key != "" && val != "")
		        return true;
		}
		
	}

	// hit the end of file
	return false;
}

GetText::~GetText() {
	close();
}
