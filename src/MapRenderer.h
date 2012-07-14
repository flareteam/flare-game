/*
Copyright © 2011-2012 Clint Bellanger

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

/**
 * class MapRenderer
 *
 * Map data structure and rendering
 * This class is capable of rendering isometric and orthogonal maps.
 */

#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include "Enemy.h"
#include "GameStatePlay.h"
#include "Utils.h"
#include "TileSet.h"
#include "MapCollision.h"
#include "Settings.h"
#include "WidgetTooltip.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>
#include <queue>
#include <vector>

class CampaignManager;
class PowerManager;

// TODO:
// Move these Map_* classes to its own file.

class Map_Group {
public:
	std::string category;
	Point pos;
	Point area;
	int levelmin;
	int levelmax;
	int numbermin;
	int numbermax;
	float chance;
	void clear() {
		category = "";
		pos.x = 0;
		pos.y = 0;
		area.x = 0;
		area.y = 0;
		levelmin = 0;
		levelmax = 0;
		numbermin = 0;
		numbermax = 0;
		chance = 1.0f;
	}
};

class Map_NPC {
public:
	std::string id;
	Point pos;
	void clear() {
		id = "";
		pos.x = 0;
		pos.y = 0;
	}
};

class Map_Event {
public:
	std::string type;
	SDL_Rect location;
	Event_Component components[256];
	int comp_num;
	SDL_Rect hotspot;
	std::string tooltip;

	//power spawn variables
	Point power_src;
	Point power_dest;
	bool targetHero;
	int damagemin;
	int damagemax;
	int power_cooldown;
	int cooldown_ticks;

	Map_Event() {
		type = "";
		location.x = 0;
		location.y = 0;
		location.w = 0;
		location.h = 0;
		comp_num = 0;
		tooltip = "";
		hotspot.x = hotspot.y = 0;
		hotspot.h = hotspot.w = 0;
		for (int j=0; j<256; j++) {
			components[j].type = "";
			components[j].s = "";
			components[j].x = 0;
			components[j].y = 0;
			components[j].z = 0;
		}
		power_src.x = power_src.y = 0;
		power_dest.x = power_dest.y = 0;
		targetHero = false;
		damagemin = damagemax = 0;
		power_cooldown = 0;
		cooldown_ticks = 0;
	}
};

class Map_Enemy {
public:
	std::string type;
	Point pos;
	int direction;
	std::queue<Point> waypoints;

	void clear() {
		pos.x = 0;
		pos.y = 0;
		// enemies face a random direction unless otherwise specified
		direction = rand() % 8;
		type = "";
		std::queue<Point> empty;
		waypoints = empty;
	}
};

const int CLICK_RANGE = 3 * UNITS_PER_TILE; //for activating events

class MapRenderer {
private:
	Mix_Music *music;

	WidgetTooltip *tip;
	TooltipData tip_buf;

	// map events can play random soundfx
	Mix_Chunk *sfx;
	std::string sfx_filename;

	bool executeEvent(Map_Event &e);
	void playSFX(std::string filename);
	void push_enemy_group(Map_Group g);
	bool isActive(const Map_Event &e);

	void loadMusic();

	// map events
	std::vector<Map_Event> events;

	unsigned short background[256][256];
	unsigned short object[256][256];
	unsigned short collision[256][256];

	void renderIsoBackground();
	void renderIsoBackObjects(std::vector<Renderable> &r);
	void renderIsoFrontObjects(std::vector<Renderable> &r);
	void renderIso(std::vector<Renderable> &r);

	void renderOrthoBackground();
	void renderOrthoBackObjects(std::vector<Renderable> &r);
	void renderOrthoFrontObjects(std::vector<Renderable> &r);
	void renderOrtho(std::vector<Renderable> &r);

	Point shakycam;
	bool new_music;
	TileSet tset;
	std::string tileset;
	std::string music_filename;

public:
	CampaignManager *camp;
	PowerManager *powers;

	// functions
	MapRenderer(CampaignManager *_camp);
	~MapRenderer();

	int load(std::string filename);
	void logic();
	void render(std::vector<Renderable> &r);
	void clearEvents();
	void checkEvents(Point loc);
	void checkEventClick();
	void checkTooltip();

	// vars
	std::string title;
	int w;
	int h;
	Point cam;
	Point hero_tile;
	Point spawn;
	int spawn_dir;

	MapCollision collider;

	// enemy load handling
	std::queue<Map_Enemy> enemies;
	Map_Enemy new_enemy;
	Map_Group new_group;
	bool enemy_awaiting_queue;
	bool group_awaiting_queue;

	// npc load handling
	std::queue<Map_NPC> npcs;
	Map_NPC new_npc;
	bool npc_awaiting_queue;

	// event-created loot or items
	std::queue<Event_Component> loot;

	// teleport handling
	bool teleportation;
	Point teleport_destination;
	std::string teleport_mapname;
	std::string respawn_map;
	Point respawn_point;

	// message handling
	std::string log_msg;

	// shaky cam
	int shaky_cam_ticks;
};


#endif
