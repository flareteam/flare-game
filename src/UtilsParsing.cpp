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

#include "UtilsParsing.h"
#include <cstdlib>
#include <fstream>

using namespace std;

/**
 * Check to see if this string represents an integer
 * The first character can be a negative (-) sign.
 */
bool isInt(const string& s) {
	if (s.size() == 0) return false;

	int start=0;
	
	// allow a negative sign as the first char
	if (s.at(0) == '-') start=1;

	for (unsigned int i=start; i<s.length(); i++) {
		// if any character in this string is not a numeric digit, this string is not an integer
		if (s.at(i) < '0' || s.at(i) > '9') return false;
	}
	return true;
}

/**
 * Convert a single hex character (0123456789abcdef) into the equivalent integer
 */
unsigned short xtoi(char c) {
	if (c >= 'a') return c - 'a' + 10;
	else if (c >= 'A') return c - 'A' + 10;
	else return c - '0';
}

/**
 * Convert two-char hex string to int 0-255
 */
unsigned short xtoi(const string& hex) {

	char c0 = hex.at(0);
	char c1 = hex.at(1);
	unsigned short val;
	
	val = xtoi(c0) * 16;
	val = val + xtoi(c1);
	
	return val;
}

/**
 * Convert four booleans into a single hex character 0-f
 */
char btox(bool b1, bool b2, bool b3, bool b4) {
	char val = b1*1 + b2*2 + b3*4 + b4*8;
	if (val < 10)
		return '0' + val;
	else
		return 'a' + val - 10;
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
 * Given a string that starts with a number then a comma
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
	int num = atoi(s.substr(0, seppos).c_str());
	s = s.substr(seppos+1, s.length());
	return num;
}

unsigned short eatFirstHex(string &s, char separator) {
	size_t seppos = s.find_first_of(separator);
	if (seppos == string::npos) {
		s = "";
		return 0; // not found
	}
	unsigned short num = xtoi(s.substr(0, seppos));
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

string getLine(istream &infile) {
	string line;
	if (!getline(infile, line))
		return ""; // Read failed.
	line = stripCarriageReturn(line);
	return line; 
}

