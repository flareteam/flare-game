#include "FileParser.h"

FileParser::FileParser() {
	line = "";
	section = "";
	key = "";
	val = "";
}

bool FileParser::open(string filename) {
	
	infile.open(filename.c_str(), ios::in);
	return infile.is_open();
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
	
	while (!infile.eof()) {

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
	
	if (!infile.eof()) {
		line = getLine(infile);
	}
	return line;
}

string FileParser::nextValue() {
	if (val == "") {
		return ""; // not found
	}
	string s;
	unsigned int seppos = val.find_first_of(',');
	if (seppos == string::npos) {
		s = val;
		val = "";
	}
	else {
		s = val.substr(0, seppos);
		val = val.substr(seppos+1, val.length());
	}
	return s;
}

FileParser::~FileParser() {
	close();
}
