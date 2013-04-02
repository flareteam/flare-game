/*
Copyright © 2011-2012 Clint Bellanger

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

#include "FileParser.h"
#include "UtilsParsing.h"


using namespace std;
	bool new_section;
	std::string section;
	std::string key;
	std::string val;

FileParser::FileParser()
	: line("")
	, new_section(false)
	, section("")
	, key("")
	, val("")
{}

bool FileParser::open(const string& _filename, const string &errormessage) {
	this->filename = _filename;
	infile.open(filename.c_str(), ios::in);
	bool ret = infile.is_open();
	if (!ret && !errormessage.empty())
		fprintf(stderr, "%s: %s\n", errormessage.c_str(), filename.c_str());
	return ret;
}

void FileParser::close() {
	if (infile.is_open())
		infile.close();
}

/**
 * Advance to the next key pair
 * Take note if a new section header is encountered
 *
 * @return false if EOF, otherwise true
 */
bool FileParser::next() {

	string starts_with;
	new_section = false;

	while (infile.good()) {

		line = getLine(infile);

		// skip ahead if this line is empty
		if (line.length() == 0) continue;

		starts_with = line.at(0);

		// skip ahead if this line is a comment
		if (starts_with == "#") continue;

		// set new section if this line is a section declaration
		if (starts_with == "[") {
			new_section = true;
			section = parse_section_title(line);

			// keep searching for a key-pair
			continue;
		}

		// this is a keypair. Perform basic parsing and return
		parse_key_pair(line, key, val);
		return true;

	}

	// hit the end of file
	return false;
}

/**
 * Get an unparsed, unfiltered line from the input file
 */
string FileParser::getRawLine() {
	line = "";

	if (infile.good()) {
		line = getLine(infile);
	}
	return line;
}

string FileParser::nextValue() {
	if (val == "") {
		return ""; // not found
	}
	string s;
	size_t seppos = val.find_first_of(',');
	size_t alt_seppos = val.find_first_of(';');
	if (alt_seppos != string::npos && alt_seppos < seppos)
		seppos = alt_seppos; // return the first ',' or ';'

	if (seppos == string::npos) {
		s = val;
		val = "";
	}
	else {
		s = val.substr(0, seppos);
		val = val.substr(seppos+1);
	}
	return s;
}

std::string FileParser::getFileName()
{
	return filename;
}

FileParser::~FileParser() {
	close();
}
