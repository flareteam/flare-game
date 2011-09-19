/**
 * UtilsFileSystem
 *
 * Various file system function wrappers. Abstracted here to hide OS-specific implementations
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "UtilsFileSystem.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

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
 * Returns a vector containing all filenames in a given folder with the given extension
 */
int getFileList(std::string dir, std::string ext, std::vector<std::string> &files) {

    DIR *dp;
    struct dirent *dirp;
    
	if((dp  = opendir(dir.c_str())) == NULL) {
        //cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
	
	int extlen = ext.length();
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
