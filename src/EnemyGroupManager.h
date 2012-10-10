/*
Copyright © 2011-2012 Thane Brimhall
		Manuel A. Fernandez Montecelo <manuel.montezelo@gmail.com>

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

#ifndef ENEMYGROUPMANAGER_H
#define ENEMYGROUPMANAGER_H

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>


struct Enemy_Level {
	std::string type;
	int level;
	std::string rarity;

	Enemy_Level() : level(0), rarity("common") {}
};


/**
 * class EnemyGroupManager
 *
 * Loads Enemies into category lists and manages spawning randomized groups of
 * enemies.
 */
class EnemyGroupManager {
public:
	/** Get instance of the Singleton */
	static EnemyGroupManager& instance();

	/** To get a random enemy with the given characteristics
	 *
	 * @param category Enemy of the desired category
	 * @param minlevel Enemy of the desired level (minimum)
	 * @param maxlevel Enemy of the desired level (maximum)
	 *
	 * @return A random enemy
	 */
	Enemy_Level getRandomEnemy(const std::string& category, int minlevel, int maxlevel) const;

private:
	/** Instance of the Singleton */
	static EnemyGroupManager* _instance;


	/** Container to store enemy data */
	std::map <std::string, std::vector<Enemy_Level> > _categories;


	/** Constructor */
	EnemyGroupManager();
	/** Destructor */
	~EnemyGroupManager();

	/** Generate the list of categories, fills the container with the enemy
	 * data */
	void generate();

	/** Get information stored on files and insert into container */
	void parseEnemyFileAndStore(const std::string& filename);
};

#endif
