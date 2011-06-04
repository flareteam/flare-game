/**
 * FileParser
 *
 * Abstract the generic key-value pair ini-style file format
 */

#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <fstream>
#include <string>
#include "UtilsParsing.h"

class FileParser {
private:
	ifstream infile;
	string line;
	
public:
	FileParser();
	~FileParser();
	
	bool open(string filename);
	void close();
	bool next();
	string getRawLine();

	bool new_section;
	string section;
	string key;
	string val;
	
};

#endif
