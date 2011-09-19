/**
 * UtilsFileSystem
 *
 * Various file system function wrappers. Abstracted here to hide OS-specific implementations
 *
 * @author Clint Bellanger
 * @license GPL
 */
 
#include <string>
#include <vector>

/**
 * Check to see if a directory exists
 */
bool dirExists(std::string path);
void createDir(std::string path);
int getFileList(std::string dir, std::string ext, std::vector<std::string> &files);


