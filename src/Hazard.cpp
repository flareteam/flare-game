/**
 * class Hazard
 *
 * Stand-alone object that can harm the hero or creatures
 * These are generated whenever something makes any attack
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "Hazard.h"

Hazard::Hazard() {
	src_stats = NULL;
	sprites = NULL;
	speed.x = 0.0;
	speed.y = 0.0;
	direction = 0;
	visual_option = 0;
	multitarget = false;
	dmg_min = 0;
	dmg_max = 0;
	crit_chance = 0;
	power_index = -1;
	rendered = false;
	lifespan=1;
	frame=0;
	frame_duration=1;
	frame_loop=1;
	active_frame=-1;
	frame_size.x = 64;
	frame_size.y = 64;
	frame_offset.x = 32;
	frame_offset.y = 32;
	delay_frames = 0;
	complete_animation = false;
	floor=false;
	active=true;
	stun_duration=0;
	immobilize_duration=0;
	slow_duration=0;
	bleed_duration=0;
	hp_steal=0;
	mp_steal=0;
	trait_armor_penetration = false;
	trait_crits_impaired = 0;
	trait_elemental = -1;
	remove_now = false;
	post_power = -1;
	wall_power = -1;
	hit_wall = false;
	equipment_modified = false;
	base_speed = 0;
}

void Hazard::setCollision(MapCollision *_collider) {
	collider = _collider;
}

void Hazard::logic() {
	
	// if the hazard is on delay, take no action
	if (delay_frames > 0) {
		delay_frames--;
		return;
	}
	
	// handle tickers
	if (lifespan > 0) lifespan--;
	frame++;
	if (frame == frame_loop) frame=0;
	
	// handle movement
	if (!(round(speed.x) == 0 && round(speed.y) == 0)) {
		pos.x += speed.x;
		pos.y += speed.y;
		
		// very simplified collider, could skim around corners
		// or even pass through thin walls if speed > tilesize
		if (collider->is_wall(round(pos.x), round(pos.y))) {
			lifespan = 0;
			hit_wall = true;
			
			if (collider->outsideMap(round(pos.x) >> TILE_SHIFT, round(pos.y) >> TILE_SHIFT))
				remove_now = true;
		}
	}

}
