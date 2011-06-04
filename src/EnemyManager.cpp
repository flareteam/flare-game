/*
 * class EnemyManager
 *
 * @author Clint Bellanger
 * @license GPL
 *
 */

#include "EnemyManager.h"

EnemyManager::EnemyManager(PowerManager *_powers, MapIso *_map) {
	powers = _powers;
	map = _map;
	enemy_count = 0;
	sfx_count = 0;
	gfx_count = 0;
	hero_pos.x = hero_pos.y = -1;
	hero_alive = true;
	handleNewMap();
}



/**
 * Enemies share graphic/sound resources (usually there are groups of similar enemies)
 */
void EnemyManager::loadGraphics(string type_id) {
	
	// TODO: throw an error if a map tries to use too many monsters
	if (gfx_count == max_gfx) return;
	
	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<gfx_count; i++) {
		if (gfx_prefixes[i] == type_id) {
			return; // already have this one
		}
	}

	sprites[gfx_count] = IMG_Load(("images/enemies/" + type_id + ".png").c_str());
	if(!sprites[gfx_count]) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	SDL_SetColorKey( sprites[gfx_count], SDL_SRCCOLORKEY, SDL_MapRGB(sprites[gfx_count]->format, 255, 0, 255) ); 

	// optimize
	SDL_Surface *cleanup = sprites[gfx_count];
	sprites[gfx_count] = SDL_DisplayFormatAlpha(sprites[gfx_count]);
	SDL_FreeSurface(cleanup);	
	
	gfx_prefixes[gfx_count] = type_id;
	gfx_count++;

}

void EnemyManager::loadSounds(string type_id) {

	// TODO: throw an error if a map tries to use too many monsters
	if (sfx_count == max_sfx) return;
	
	// first check to make sure the sprite isn't already loaded
	for (int i=0; i<sfx_count; i++) {
		if (sfx_prefixes[i] == type_id) {
			return; // already have this one
		}
	}
	
	sound_phys[sfx_count] = Mix_LoadWAV(("soundfx/enemies/" + type_id + "_phys.ogg").c_str());
	sound_ment[sfx_count] = Mix_LoadWAV(("soundfx/enemies/" + type_id + "_ment.ogg").c_str());
	sound_hit[sfx_count] = Mix_LoadWAV(("soundfx/enemies/" + type_id + "_hit.ogg").c_str());
	sound_die[sfx_count] = Mix_LoadWAV(("soundfx/enemies/" + type_id + "_die.ogg").c_str());
	sound_critdie[sfx_count] = Mix_LoadWAV(("soundfx/enemies/" + type_id + "_critdie.ogg").c_str());
	
	sfx_prefixes[sfx_count] = type_id;
	sfx_count++;
}

/**
 * When loading a new map, we eliminate existing enemies and load the new ones.
 * The map will have loaded Entity blocks into an array; retrieve the Enemies and init them
 */
void EnemyManager::handleNewMap () {
	
	Map_Enemy me;
	
	// delete existing enemies
	for (int i=0; i<enemy_count; i++) {
		delete(enemies[i]);
	}
	enemy_count = 0;
	
	// free shared resources
	for (int j=0; j<gfx_count; j++) {
		SDL_FreeSurface(sprites[j]);
	}
	for (int j=0; j<sfx_count; j++) {
		Mix_FreeChunk(sound_phys[j]);
		Mix_FreeChunk(sound_ment[j]);
		Mix_FreeChunk(sound_hit[j]);
		Mix_FreeChunk(sound_die[j]);
		Mix_FreeChunk(sound_critdie[j]);
	}
	gfx_count = 0;
	sfx_count = 0;
	
	// load new enemies
	while (!map->enemies.empty()) {
		me = map->enemies.front();
		map->enemies.pop();
		
		enemies[enemy_count] = new Enemy(powers, map);
		enemies[enemy_count]->stats.pos.x = me.pos.x;
		enemies[enemy_count]->stats.pos.y = me.pos.y;
		enemies[enemy_count]->stats.direction = me.direction;
		enemies[enemy_count]->stats.load("enemies/" + me.type + ".txt");
		loadGraphics(enemies[enemy_count]->stats.gfx_prefix);
		loadSounds(enemies[enemy_count]->stats.sfx_prefix);
		enemy_count++;
	}
}

/**
 * perform logic() for all enemies
 */
void EnemyManager::logic() {
	int pref_id;

	for (int i=0; i<enemy_count; i++) {
	
		// hazards are processed after Avatar and Enemy[]
		// so process and clear sound effects from previous frames
		// check sound effects
		for (int j=0; j<sfx_count; j++) {
			if (sfx_prefixes[j] == enemies[i]->stats.sfx_prefix)
				pref_id = j;
		}
		
		if (enemies[i]->sfx_phys) Mix_PlayChannel(-1, sound_phys[pref_id], 0);
		if (enemies[i]->sfx_ment) Mix_PlayChannel(-1, sound_ment[pref_id], 0);
		if (enemies[i]->sfx_hit) Mix_PlayChannel(-1, sound_hit[pref_id], 0);
		if (enemies[i]->sfx_die) Mix_PlayChannel(-1, sound_die[pref_id], 0);		
		if (enemies[i]->sfx_critdie) Mix_PlayChannel(-1, sound_critdie[pref_id], 0);		
		
		// clear sound flags
		enemies[i]->sfx_hit = false;
		enemies[i]->sfx_phys = false;
		enemies[i]->sfx_ment = false;
		enemies[i]->sfx_die = false;
		enemies[i]->sfx_critdie = false;
		
		// new actions this round
		enemies[i]->stats.hero_pos = hero_pos;
		enemies[i]->stats.hero_alive = hero_alive;
		enemies[i]->logic();

	}
}

Enemy* EnemyManager::enemyFocus(Point mouse, Point cam, bool alive_only) {
	Point p;
	SDL_Rect r;
	for(int i = 0; i < enemy_count; i++) {
		if(alive_only && (enemies[i]->stats.cur_state == ENEMY_DEAD || enemies[i]->stats.cur_state == ENEMY_CRITDEAD)) {
			continue;
		}
		p = map_to_screen(enemies[i]->stats.pos.x, enemies[i]->stats.pos.y, cam.x, cam.y);
	
		r.w = enemies[i]->stats.render_size.x;
		r.h = enemies[i]->stats.render_size.y;
		r.x = p.x - enemies[i]->stats.render_offset.x;
		r.y = p.y - enemies[i]->stats.render_offset.y;
		
		if (isWithin(r, mouse)) {
			Enemy *enemy = enemies[i];
			return enemy;
		}
	}
	return NULL;
}

/**
 * If an enemy has died, reward the hero with experience points
 */
void EnemyManager::checkEnemiesforXP(StatBlock *stats) {
	for (int i=0; i<enemy_count; i++) {
		if (enemies[i]->reward_xp) {
			stats->xp += enemies[i]->stats.level;
			enemies[i]->reward_xp = false; // clear flag
		}
	}
}

/**
 * getRender()
 * Map objects need to be drawn in Z order, so we allow a parent object (GameEngine)
 * to collect all mobile sprites each frame.
 * 
 * This wrapper function is necessary because EnemyManager holds shared sprites for identical-looking enemies
 */
Renderable EnemyManager::getRender(int enemyIndex) {
	Renderable r = enemies[enemyIndex]->getRender();
	for (int i=0; i<gfx_count; i++) {
		if (gfx_prefixes[i] == enemies[enemyIndex]->stats.gfx_prefix)
			r.sprite = sprites[i];
	}
	return r;	
}

EnemyManager::~EnemyManager() {
	for (int i=0; i<enemy_count; i++) {
		delete enemies[i];
	}
	
	for (int i=0; i<gfx_count; i++) {
		SDL_FreeSurface(sprites[i]);
	}
	for (int i=0; i<sfx_count; i++) {
		Mix_FreeChunk(sound_phys[i]);
		Mix_FreeChunk(sound_ment[i]);
		Mix_FreeChunk(sound_hit[i]);
		Mix_FreeChunk(sound_die[i]);
		Mix_FreeChunk(sound_critdie[i]);
	}

}

