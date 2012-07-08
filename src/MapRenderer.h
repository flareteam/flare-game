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

class CampaignManager;
class PowerManager;

struct Map_Group {
	std::string category;
	Point pos;
	Point area;
	int levelmin;
	int levelmax;
	int numbermin;
	int numbermax;
	float chance;
};

struct Map_NPC {
	std::string id;
	Point pos;
};

struct Map_Event {
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
};

struct Map_Enemy {
	std::string type;
	Point pos;
	int direction;
	std::queue<Point> waypoints;
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

	void executeEvent(int eid);
	void removeEvent(int eid);
	void playSFX(std::string filename);
	void push_enemy_group(Map_Group g);
	bool isActive(int eventid);

	// map events
	Map_Event events[256];
	int event_count;

public:
	CampaignManager *camp;
	PowerManager *powers;

	// functions
	MapRenderer(CampaignManager *_camp);
	~MapRenderer();
	void clearEnemy(Map_Enemy &e);
	void clearNPC(Map_NPC &n);
	void clearGroup(Map_Group &g);

	int load(std::string filename);
	void loadMusic();
	void logic();
	void render(Renderable r[], int rnum);
	void renderIso(Renderable r[], int rnum);
	void renderOrtho(Renderable r[], int rnum);
	void checkEvents(Point loc);
	void checkEventClick();
	void clearEvents();
	void checkTooltip();

	// vars
	std::string title;
	int w;
	int h;
	Point cam;
	Point hero_tile;
	Point spawn;
	int spawn_dir;
	std::string tileset;
	std::string music_filename;
	bool new_music;
	TileSet tset;

	unsigned short background[256][256];
	unsigned short object[256][256];
	unsigned short collision[256][256];
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
