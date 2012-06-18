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
 * UtilsFileSystem
 *
 * Various file system function wrappers. Abstracted here to hide OS-specific implementations
 */

#include "UtilsFileSystem.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

/**
 * Check to see if a directory/folder exists
 */
bool dirExists(std::string path) {
	struct stat st;
	return (stat(path.c_str(), &st) == 0);
}

/**
 * Create this folder if it doesn't already exist
 */
void createDir(std::string path) {

#ifndef _WIN32
	// *nix implementation
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif

#ifdef _WIN32
	// win implementation
	std::string syscmd = "mkdir " + path;
	system(syscmd.c_str());
#endif
}

/**
 * Check to see if a file exists
 * The filename parameter should include the entire path to this file
 */
bool fileExists(std::string filename) {
	bool exists;

	std::ifstream infile(filename.c_str());
	exists = infile.is_open();
	if (infile.is_open()) infile.close();
	
	return exists;
}

/**
 * Returns a vector containing all filenames in a given folder with the given extension
 */
int getFileList(std::string dir, std::string ext, std::vector<std::string> &files) {

    DIR *dp;
    struct dirent *dirp;
    
	if((dp  = opendir(dir.c_str())) == NULL) {
        //cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
	
	size_t extlen = ext.length();
    while ((dirp = readdir(dp)) != NULL) {
	//	if(dirp->d_type == 0x8) { //0x4 for directories, 0x8 for files
		std::string filename = std::string(dirp->d_name);
		if(filename.length() > extlen) {
			if(filename.substr(filename.length()-extlen,extlen) == ext) {
				files.push_back(filename);
			}
		}
    }
    closedir(dp);
    return 0;
}

/**
 * Returns a vector containing all directory names in a given directory
 */
int getDirList(std::string dir, std::vector<std::string> &dirs) {

	DIR *dp;
	struct dirent *dirp;
	struct stat st;

	if((dp  = opendir(dir.c_str())) == NULL) {
		//cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}
	
	while ((dirp = readdir(dp)) != NULL) {
	//	do not use dirp->d_type, it's not portable
		std::string directory = std::string(dirp->d_name);
		std::string mod_dir = dir + "/" + directory;
		if ((stat(mod_dir.c_str(), &st) != -1)
			&& S_ISDIR(st.st_mode)
			&& directory != "."
			&& directory != ".."
			)
		{
			dirs.push_back(directory);
		}
	}
	closedir(dp);
	return 0;
}
