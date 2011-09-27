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
using namespace std;

/**
 * Check to see if this string represents an integer
 * The first character can be a negative (-) sign.
 */
bool isInt(string s) {
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
 * Convert a single hex character (0123456789abcdef) into the equivalent integer
 */
unsigned short xtoi(char c) {
	if (c >= 97) return c - 87;
	else if (c >= 65) return c - 55;
	else return c - 48;
}

/**
 * Convert two-char hex string to int 0-255
 */
unsigned short xtoi(string hex) {

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
	if (b4)
		if (b3)
			if (b2)
				if (b1) return 'f';
				else return 'e';
			else
				if (b1) return 'd';
				else return 'c';
		else
			if (b2)
				if (b1) return 'b';
				else return 'a';
			else
				if (b1) return '9';
				else return '8';
	else
		if (b3)
			if (b2)
				if (b1) return '7';
				else return '6';
			else
				if (b1) return '5';
				else return '4';
		else
			if (b2)
				if (b1) return '3';
				else return '2';
			else
				if (b1) return '1';
				else return '0';
}

/**
 * trim: remove leading and trailing c from s
 */
string trim(string s, char c) {
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

string parse_section_title(string s) {
	unsigned int bracket = s.find_first_of(']');
	if (bracket == string::npos) return ""; // not found
	return s.substr(1, bracket-1);
}

void parse_key_pair(string s, string &key, string &val) {
	unsigned int separator = s.find_first_of('=');
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
	int seppos = s.find_first_of(separator);
	if (seppos == string::npos) {
		s = "";
		return 0; // not found
	}
	int num = atoi(s.substr(0, seppos).c_str());
	s = s.substr(seppos+1, s.length());
	return num;
}

unsigned short eatFirstHex(string &s, char separator) {
	int seppos = s.find_first_of(separator);
	if (seppos == string::npos) {
		s = "";
		return 0; // not found
	}
	unsigned short num = xtoi(s.substr(0, seppos));
	s = s.substr(seppos+1, s.length());
	return num;
}

string eatFirstString(string &s, char separator) {
	int seppos = s.find_first_of(separator);
	if (seppos == string::npos) return ""; // not found
	string outs = s.substr(0, seppos);
	s = s.substr(seppos+1, s.length());
	return outs;
}

// strip carriage return if exists
string stripCarriageReturn(string line) {
	if (line.length() > 0) {
		if ('\r' == line.at(line.length()-1)) {
			return line.substr(0, line.length()-1);
		}
	}
	return line;
}

string getLine(ifstream &infile) {
	string line;
	getline(infile, line);
	line = stripCarriageReturn(line);
	return line; 
}

