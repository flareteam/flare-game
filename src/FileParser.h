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

/**
 * FileParser
 *
 * Abstract the generic key-value pair ini-style file format
 */


#pragma once
#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <fstream>
#include <string>

class FileParser {
private:
	std::string filename;
	std::ifstream infile;
	std::string line;

public:
	FileParser();
	~FileParser();

	/**
	 * @brief open
	 * @param filename The file to be opened
	 * @param errormessage
	 * Optional parameter, will be printed to stderr together with the filename
	 * if an error occurs. If errormessage is empty, there will be no output to
	 * stderr in any case.
	 * @return true if file could be opened successfully for reading.
	 */
	bool open(const std::string& filename, const std::string &errormessage = "Could not open text file");
	void close();
	bool next();
	std::string nextValue(); // next value inside one line.
	std::string getRawLine();
	std::string getFileName();

	bool new_section;
	std::string section;
	std::string key;
	std::string val;
};

#endif
