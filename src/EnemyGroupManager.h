/**
 * class EnemyGroupManager
 *
 * Loads Enemies into category lists and manages spawning randomized groups of enemies
 *
 * @author Thane Brimhall
 * @license GPL
 */
#ifndef ENEMYGROUPMANAGER_H
#define ENEMYGROUPMANAGER_H

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "Settings.h"
#include "MapIso.h"
#include "FileParser.h"

using namespace std;

struct Enemy_Level {
	string type;
	int level;
};

class EnemyGroupManager {
private:
	// variables
	map <string, vector<Enemy_Level> > category_list; 
	// functions
	void extract_and_sort(string filename);
	int getdir(string dir, string ext, vector<string> &files);
public:
	// functions
	EnemyGroupManager(/*ARGS WILL GO HERE*/);
	~EnemyGroupManager();
	void generate();
	Enemy_Level random_enemy(string category, int minlevel, int maxlevel);
};

#endif
