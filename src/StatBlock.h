/**
 * class StatBlock
 *
 * Character stats and calculations
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef STAT_BLOCK_H
#define STAT_BLOCK_H

#include <string>
#include <fstream>
#include "Settings.h"
#include "Utils.h"
#include "UtilsParsing.h"
using namespace std;

const int STAT_EFFECT_SHIELD = 0;
const int STAT_EFFECT_VENGEANCE = 1;

const int POWERSLOT_COUNT = 4;
const int MELEE_PHYS = 0;
const int MELEE_MENT = 1;
const int RANGED_PHYS = 2;
const int RANGED_MENT = 3;


class StatBlock {
private:

public:
	StatBlock();
	~StatBlock();
	
	void load(string filename);
	void takeDamage(int dmg);
	void recalc();
	void logic();
	void clearEffects();
	Renderable getEffectRender(int effect_type);

	bool alive;
	bool corpse; // creature is dead and done animating
	bool hero; // else, enemy or other
	int targeted; // after being subject to an attack, cannot be hit by an attack for a short time
	
	string name;
	string sfx_prefix;
	string gfx_prefix;
	
	int level;
	int xp;
	int xp_table[18];
	
	// base stats
	int offense;
	int defense;
	int physical;
	int mental;
	
	// derived stats
	int physoff;
	int physdef;
	int mentoff;
	int mentdef;
	
	// physical stats
	int hp;
	int maxhp;
	int hp_per_minute;
	int hp_ticker;

	// mental stats
	int mp;
	int maxmp;
	int mp_per_minute;
	int mp_ticker;
	
	// offense stats
	int accuracy;
	
	// defense stats
	int avoidance;

	// overall stats
	int crit;

	// equipment stats	
	int dmg_melee_min;
	int dmg_melee_max;
	int dmg_ment_min;
	int dmg_ment_max;
	int dmg_ranged_min;
	int dmg_ranged_max;
	int absorb_min;
	int absorb_max;	
	bool wielding_physical;
	bool wielding_mental;
	bool wielding_offense;
	bool ammo_arrows;
	int attunement_fire;
	int attunement_ice;

	// buff and debuff stats
	int slow_duration;
	int bleed_duration;
	int stun_duration;
	int immobilize_duration;
	int immunity_duration;	
	int haste_duration;
	int hot_duration;
	int hot_value;
	
	int shield_hp; // shield
	int shield_frame;
	bool blocking;
	int vengeance_stacks;
	int vengeance_frame;
	
	int speed;
	int dspeed;
	Point pos;
	int direction;
		
	// enemy behavioral stats
	int chance_pursue;
	int chance_flee;
	
	int power_chance[POWERSLOT_COUNT];
	int power_index[POWERSLOT_COUNT];
	int power_cooldown[POWERSLOT_COUNT];
	int power_ticks[POWERSLOT_COUNT];
		
	int melee_range;
	int threat_range;
	Point hero_pos;
	bool hero_alive;
	Point last_seen; 
	int dir_favor;
	int dir_ticks;
	int patrol_ticks;
	bool in_combat;
	int cooldown_ticks;
	int cooldown; // min. # of frames between abilities
	
	int loot_chance;
	
	// rendering info
	int cur_state;
	int cur_frame;
	int disp_frame;
	
	
	Point render_size;
	Point render_offset;

	int anim_stance_position;
	int anim_stance_frames;
	int anim_stance_duration;
	int anim_run_position;
	int anim_run_frames;
	int anim_run_duration;
	int anim_melee_position;
	int anim_melee_frames;
	int anim_melee_duration;
	int anim_ment_position;
	int anim_ment_frames;
	int anim_ment_duration;
	int anim_ranged_position;
	int anim_ranged_frames;
	int anim_ranged_duration;
	int anim_block_position;
	int anim_block_frames;
	int anim_block_duration;
	int anim_hit_position;
	int anim_hit_frames;
	int anim_hit_duration;
	int anim_die_position;
	int anim_die_frames;
	int anim_die_duration;
	int anim_critdie_position;
	int anim_critdie_frames;
	int anim_critdie_duration;

	// for the teleport spell
	bool teleportation;
	Point teleport_destination;
	
	// weapons can modify spells
	int melee_weapon_power;
	int mental_weapon_power;
	int ranged_weapon_power;
	
	// for purchasing tracking
	int gold;
	
	// marked for death
	bool death_penalty;
	
	// Campaign event interaction
	string defeat_status;
	string quest_loot_requires;
	string quest_loot_not;
	int quest_loot_id;
	int first_defeat_loot;
	
};

#endif
