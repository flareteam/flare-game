/*
 * class EnemyManager
 *
 * @author Clint Bellanger
 * @license GPL
 *
 */
 
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "MapIso.h"
#include "Enemy.h"
#include "Utils.h"
#include "PowerManager.h"

// TODO: rename these to something more specific to EnemyManager
const int max_sfx = 8;
const int max_gfx = 32;

class EnemyManager {
private:

	MapIso *map;
	PowerManager *powers;
	void loadGraphics(string type_id);
	void loadSounds(string type_id);

	string gfx_prefixes[max_gfx];
	int gfx_count;
	string sfx_prefixes[max_sfx];
	int sfx_count;
	
	SDL_Surface *sprites[max_gfx];	
	Mix_Chunk *sound_phys[max_sfx];
	Mix_Chunk *sound_ment[max_sfx];
	Mix_Chunk *sound_hit[max_sfx];
	Mix_Chunk *sound_die[max_sfx];
	Mix_Chunk *sound_critdie[max_sfx];
	
public:
	EnemyManager(PowerManager *_powers, MapIso *_map);
	~EnemyManager();
	void handleNewMap();
	void logic();
	Renderable getRender(int enemyIndex);
	void checkEnemiesforXP(StatBlock *stats);
	Enemy *enemyFocus(Point mouse, Point cam, bool alive_only);

	// vars
	Enemy *enemies[256]; // TODO: change to dynamic list without limits
	Point hero_pos;
	bool hero_alive;
	int enemy_count;
};


#endif
