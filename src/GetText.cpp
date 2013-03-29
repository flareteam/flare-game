/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2013 Henrik Andersson

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
#include "UtilsParsing.h"


using namespace std;


GetText::GetText() {
	line = "";
	key = "";
	val = "";
}

bool GetText::open(const string& filename) {
	infile.open(filename.c_str(), ios::in);
	return infile.is_open();
}

void GetText::close() {
	if (infile.is_open())
		infile.close();
}

// Turns all \" into just "
string GetText::sanitize(string message) {
	signed int pos = 0;
	while ((pos = message.find("\\\"")) != -1) {
		message = message.substr(0, pos) + message.substr(pos+1);
	}
	return message;
}

/**
 * Advance to the next key pair
 *
 * @return false if EOF, otherwise true
 */
bool GetText::next() {

	key = "";
	val = "";

	fuzzy = false;

	while (infile.good()) {
		line = getLine(infile);

		// check if comment and if fuzzy
		if (line.compare(0,2,"#,") && line.find("fuzzy") == 0)
		        fuzzy = true;

		// this is a key
		if (line.find("msgid") == 0) {
			// grab only what's contained in the quotes
			key = line.substr(6);
			key = key.substr(1, key.length()-2); //strips off "s
			key = sanitize(key);

			if (key != "")
				continue;
			else {
				// It is a multi-line value, unless it is the first msgid, in which case it will be empty
				// and it will be ignored when finding the matching msgstr, so no big deal.
				line = getLine(infile);
				while(line.find("\"") == 0)
				{
					// We remove the double quotes.
					key += line.substr(1, line.length()-2);
					line = getLine(infile);
				}
			}
		}

		// this is a value
		if (line.find("msgstr") == 0) {
			// grab only what's contained in the quotes
			val = line.substr(7);
			val = val.substr(1, val.length()-2); //strips off "s
			val = sanitize(val);

			// handle keypairs
			if (key != "")
			{
				if(val != "") // One-line value found.
				{
					return true;
				}
				else  // Might be a multi-line value.
				{
					line = getLine(infile);
					while(line.find("\"") == 0)
					{
						// We remove the double quotes.
						val += line.substr(1, line.length()-2);
						line = getLine(infile);
					}
					if(val != "") // It was a multi-line value indeed.
					{
						return true;
					}
				}
			}
		}
	}

	// hit the end of file
	return false;
}

GetText::~GetText() {
	close();
}
