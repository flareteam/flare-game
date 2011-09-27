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

#ifndef UTILS_PARSING_H
#define UTILS_PARSING_H

#include <string>
#include <stdlib.h>
#include <fstream>
using namespace std;

bool isInt(string s);
unsigned short xtoi(char c);
unsigned short xtoi(string hex);
char btox(bool b1, bool b2, bool b3, bool b4);
string trim(string s, char c);
string parse_section_title(string s);
void parse_key_pair(string s, string &key, string &val);
int eatFirstInt(string &s, char separator);
unsigned short eatFirstHex(string &s, char separator);
string eatFirstString(string &s, char separator);
string stripCarriageReturn(string line);
string getLine(ifstream &infile);

#endif
