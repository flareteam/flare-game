/**
 * class EnemyGroupManager
 *
 * Loads Enemies into category lists and manages spawning randomized groups of enemies
 *
 * @author Thane Brimhall
 * @license GPL
 */

#include "EnemyGroupManager.h"

EnemyGroupManager::EnemyGroupManager() {
}
EnemyGroupManager::~EnemyGroupManager() {
}

/**
 * Returns a vector containing all filenames in a given folder with the given extension
 */
int EnemyGroupManager::getdir(string dir, string ext, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    
	if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
	
	int extlen = ext.length();
    while ((dirp = readdir(dp)) != NULL) {
	//	if(dirp->d_type == 0x8) { //0x4 for directories, 0x8 for files
		string filename = string(dirp->d_name);
		if(filename.length() > extlen) {
			if(filename.substr(filename.length()-extlen,extlen) == ext) {
				files.push_back(filename);
			}
		}
    }
    closedir(dp);
    return 0;
}

// Fills the array with the enemy data
void EnemyGroupManager::generate() {
	string dir = string("enemies");
	vector<string> files = vector<string>();
	getdir(dir,".txt",files);
	for (int i = 0; i < files.size(); i++) {
		extract_and_sort(files[i]);
	}
}


//NYI
void EnemyGroupManager::extract_and_sort(string filename) {
	FileParser infile;
	Enemy_Level new_enemy;
	vector<string> categories;

	if (infile.open(("enemies/" + filename).c_str())) {
		new_enemy.type = filename.substr(0,filename.length()-4); //removes the ".txt" from the filename
		while (infile.next()) {
			if(infile.key == "level") {
				new_enemy.level = atoi(infile.val.c_str());
			}
			else if(infile.key == "categories") {
				string cat = "";
				while ( (cat = infile.nextValue()) != "") {
					categories.push_back(cat);
				}
			}
		}
	}
	//push the enemy data into each category it belongs to
	for (int i = 0; i < categories.size(); i++){
		category_list[categories[i]].push_back(new_enemy);
	}

	infile.close();
	return;
}

// Returns a random monster that fits the category and level range
Enemy_Level EnemyGroupManager::random_enemy(string category, int minlevel, int maxlevel) {
	Enemy_Level new_enemy;
	vector<Enemy_Level> enemy_list;
	//load only the data that fit the criteria
	for (int i = 0; i < category_list[category].size(); i++){
		new_enemy = category_list[category][i];
		if ( (new_enemy.level >= minlevel) && (new_enemy.level <= maxlevel)){
			enemy_list.push_back(new_enemy);
		}
	}
	if (enemy_list.size() == 0) return new_enemy;
	return enemy_list[rand() % enemy_list.size()];
}










