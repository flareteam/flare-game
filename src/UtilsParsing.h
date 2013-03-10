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


#pragma once
#ifndef UTILS_PARSING_H
#define UTILS_PARSING_H

#include <string>
#include <typeinfo>
#include <iostream>

bool isInt(const std::string& s);
std::string trim(const std::string& s, char c);
int parse_duration(const std::string& s);
std::string parse_section_title(const std::string& s);
void parse_key_pair(const std::string& s, std::string& key, std::string& val);
int eatFirstInt(std::string& s, char separator);
std::string eatFirstString(std::string& s, char separator);
std::string getNextToken(const std::string& s, size_t& cursor, char separator);
std::string stripCarriageReturn(const std::string& line);
std::string getLine(std::ifstream& infile);
bool tryParseValue(const std::type_info & type, const char * value, void * output);
bool tryParseValue(const std::type_info & type, const std::string & value, void * output);
std::string toString(const std::type_info & type, void * value);
int toInt(const std::string& s, int default_value = 0);
bool toBool(std::string value);

#endif
