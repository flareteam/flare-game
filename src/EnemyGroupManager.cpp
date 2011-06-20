/**
 * class EnemyGroupManager
 *
 * Loads Enemies into category lists and manages spawning randomized groups of enemies
 *
 * @author Thane Brimhall
 * @license GPL
 */

#include "EnemyGroupManager.h"

EnemyGroupManager::EnemyGroupManager(/*ARGS will go here*/) {

}
EnemyGroupManager::~EnemyGroupManager(/*ARGS will go here*/) {

}

// Fills the array with the enemy data
void EnemyGroupManager::generate() {
	//extract_and_sort("rotting_zombie.txt");
	extract_and_sort("zombie.txt");
	//extract_and_sort("goblin.txt");
	//extract_and_sort("minotaur.txt");
}


//NYI
void EnemyGroupManager::extract_and_sort(string filename) {
	FileParser infile;
	Enemy_Level new_enemy;
	vector<string> categories;

	if (infile.open(("enemies/" + filename).c_str())) {
		while (infile.next()) {
			if(infile.key == "name") {
				new_enemy.type = infile.val;
			}
			else if(infile.key == "level") {
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
	
	for (int i = 0; i < categories.size(); i++){
		category_list[categories[i]].push_back(new_enemy);
	}

	infile.close();
	return;
}

//NYI
Enemy_Level EnemyGroupManager::random_enemy(string category, int minlevel, int maxlevel) {
	Enemy_Level new_enemy;
	new_enemy.type = "zombie";
	new_enemy.level = 3;
	return new_enemy;
}










