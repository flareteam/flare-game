/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller
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

#include "UtilsParsing.h"
#include "Settings.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

/**
 * Check to see if this string represents an integer
 * The first character can be a negative (-) sign.
 */
bool isInt(const string& s) {
	if (s == "") return false;

	int start=0;

	// allow a negative sign as the first char
	if (s.at(0) == '-') start=1;

	for (unsigned int i=start; i<s.length(); i++) {
		// if any character in this string is not a numeric digit, this string is not an integer
		if (s.at(i) < 48 || s.at(i) > 57) return false;
	}
	return true;
}

/**
 * trim: remove leading and trailing c from s
 */
string trim(const string& s, char c) {
	if (s.length() == 0) return "";

	unsigned int first = 0;
	unsigned int last = s.length()-1;

	while (s.at(first) == c && first < s.length()-1) {
		first++;
	}
	while (s.at(last) == c && last >= first && last > 0) {
		last--;
	}
	if (first <= last) return s.substr(first,last-first+1);
	return "";
}

/**
 * Parse a duration string and return duration in frames.
 */
int parse_duration(const std::string& s) {
	int val = 0;
	std::string suffix = "";
	std::stringstream ss;
	ss.str(s);
	ss >> val;
	ss >> suffix;

	if (suffix == "s")
		val *= MAX_FRAMES_PER_SEC;
	else if (suffix == "ms")
		val = (val/1000.0f)*MAX_FRAMES_PER_SEC;

	return val;
}

string parse_section_title(const string& s) {
	size_t bracket = s.find_first_of(']');
	if (bracket == string::npos) return ""; // not found
	return s.substr(1, bracket-1);
}

void parse_key_pair(const string& s, string &key, string &val) {
	size_t separator = s.find_first_of('=');
	if (separator == string::npos) {
		key = "";
		val = "";
		return; // not found
	}
	key = s.substr(0, separator);
	val = s.substr(separator+1, s.length());
	key = trim(key, ' ');
	val = trim(val, ' ');
}

/**
 * Given a string that starts with a decimal number then a comma
 * Return that int, and modify the string to remove the num and comma
 *
 * This is basically a really lazy "split" replacement
 */
int eatFirstInt(string &s, char separator) {
	size_t seppos = s.find_first_of(separator);
	if (seppos == string::npos) {
		s = "";
		return 0; // not found
	}
	int num = toInt(s.substr(0, seppos));
	s = s.substr(seppos+1, s.length());
	return num;
}

string eatFirstString(string &s, char separator) {
	size_t seppos = s.find_first_of(separator);
	if (seppos == string::npos) return ""; // not found
	string outs = s.substr(0, seppos);
	s = s.substr(seppos+1, s.length());
	return outs;
}

// similar to eatFirstString but does not alter the input string
string getNextToken(const string& s, size_t &cursor, char separator) {
	size_t seppos = s.find_first_of(separator, cursor);
	if (seppos == string::npos) { // not found
		cursor = string::npos;
		return "";
	}
	string outs = s.substr(cursor, seppos-cursor);
	cursor = seppos+1;
	return outs;
}

// strip carriage return if exists
string stripCarriageReturn(const string& line) {
	if (line.length() > 0) {
		if ('\r' == line.at(line.length()-1)) {
			return line.substr(0, line.length()-1);
		}
	}
	return line;
}

string getLine(ifstream &infile) {
	string line;
	// This is the standard way to check whether a read failed.
	if (!getline(infile, line))
		return "";
	line = stripCarriageReturn(line);
	return line;
}

bool tryParseValue(const type_info & type, const char * value, void * output) {
	return tryParseValue(type, string(value), output);
}

bool tryParseValue(const type_info & type, const std::string & value, void * output) {

	stringstream stream(value);

	// TODO: add additional type parsing
	if (type == typeid(bool)) {
		stream>>(bool&)*((bool*)output);
	} else if (type == typeid(int)) {
		stream>>(int&)*((int*)output);
	} else if (type == typeid(unsigned int)) {
		stream>>(unsigned int&)*((unsigned int*)output);
	} else if (type == typeid(short)) {
		stream>>(short&)*((short*)output);
	} else if (type == typeid(unsigned short)) {
		stream>>(unsigned short&)*((unsigned short*)output);
	} else if (type == typeid(char)) {
		stream>>(char&)*((char*)output);
	} else if (type == typeid(unsigned char)) {
		stream>>(unsigned char&)*((unsigned char*)output);
	} else if (type == typeid(float)) {
		stream>>(float&)*((float*)output);
	} else if (type == typeid(std::string)) {
		*((string *)output) = value;
	} else {
		cout << __FUNCTION__ << ": a required type is not defined!" << endl;
		return false;
	}

	return !stream.fail();
}

std::string toString(const type_info & type, void * value) {

	stringstream stream;

	// TODO: add additional type parsing
	if (type == typeid(bool)) {
		stream<<*((bool*)value);
	} else if (type == typeid(int)) {
		stream<<*((int*)value);
	} else if (type == typeid(unsigned int)) {
		stream<<*((unsigned int*)value);
	} else if (type == typeid(short)) {
		stream<<*((short*)value);
	} else if (type == typeid(unsigned short)) {
		stream<<*((unsigned short*)value);
	} else if (type == typeid(char)) {
		stream<<*((char*)value);
	} else if (type == typeid(unsigned char)) {
		stream<<*((unsigned char*)value);
	} else if (type == typeid(float)) {
		stream<<*((float*)value);
	} else if (type == typeid(std::string)) {
		return (string &)*((string *)value);
	} else {
		cout << __FUNCTION__ << ": a required type is not defined!" << endl;
		return "";
	}

	return stream.str();
}

int toInt(const string& s, int default_value) {
	int result;
	if (!(stringstream(s) >> result))
		result = default_value;
	return result;
}

string &trim_right_inplace(string &s, const string& delimiters = " \f\n\r\t\v") {
	return s.erase(s.find_last_not_of(delimiters) + 1);
}

string& trim_left_inplace(string &s, const string& delimiters = " \f\n\r\t\v" ) {
	return s.erase(0, s.find_first_not_of(delimiters));
}

string &trim(string &s, const string& delimiters = " \f\n\r\t\v") {
	return trim_left_inplace(trim_right_inplace(s, delimiters), delimiters);
}

bool toBool(std::string value) {
	trim(value);

	std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	if (value == "true") return true;
	if (value == "yes") return true;
	if (value == "false") return false;
	if (value == "no") return false;

	fprintf(stderr, "%s %s doesn't know how to handle %s\n", __FILE__, __FUNCTION__, value.c_str());
	return false;
}


