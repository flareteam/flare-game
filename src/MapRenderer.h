/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller
Copyright © 2013 Henrik Andersson

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


#pragma once
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

// TODO: Move these Map_* classes to its own file.

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
	std::vector<Event_Component> components;
	SDL_Rect hotspot;
	std::string tooltip;
	int cooldown; // events that run multiple times pause this long in frames

	//power spawn variables
	Point power_src;
	Point power_dest;
	bool targetHero;
	int damagemin;
	int damagemax;
	int cooldown_ticks;

	StatBlock *stats;

	Map_Event()
	 : type("")
	 , components(std::vector<Event_Component>())
	 , tooltip("")
	 , cooldown(0)
	 , targetHero(false)
	 , damagemin(0)
	 , damagemax(0)
	 , cooldown_ticks(0)
	 , stats(NULL)
	{
		location.x = location.y = location.w = location.h = 0;
		hotspot.x = hotspot.y = hotspot.w = hotspot.h = 0;
		power_src.x = power_src.y = 0;
		power_dest.x = power_dest.y = 0;
	}

	~Map_Event()
	{
		delete stats; // may be NULL, but delete can deal with null pointers.
	}
};

class Map_Enemy {
public:
	std::string type;
	Point pos;
	int direction;
	std::queue<Point> waypoints;
	bool wander;
	SDL_Rect wander_area;

	Map_Enemy(std::string _type="", Point _pos=Point())
	 : type(_type)
	 , pos(_pos)
	 , direction(rand() % 8)
	 , waypoints(std::queue<Point>())
	 , wander(false)
	{
		wander_area.x = 0;
		wander_area.y = 0;
		wander_area.w = 0;
		wander_area.h = 0;
	}
};

class MapRenderer {
private:
	Mix_Music *music;

	WidgetTooltip *tip;
	TooltipData tip_buf;
	Point tip_pos;
	bool show_tooltip;

	bool executeEvent(Map_Event &e);
	void push_enemy_group(Map_Group g);
	bool isActive(const Map_Event &e);

	void loadMusic();

	// map events
	std::vector<Map_Event> events;

	// map soundids
	std::vector<SoundManager::SoundID> sids;

	typedef unsigned short maprow[256];

	maprow *background;
	maprow *fringe;
	maprow *object; // must exist in each map!
	maprow *foreground;
	maprow *collision; // must exist in each map!

	// When the animated tiles are switched off, the background is
	// not rendered all the time but everytime you have moved away too much.
	// then the background is completely rendered, else it is just blit
	// onto screen. units in tiles:
	static const short movedistance_to_rerender = 4;


	// some events are automatically triggered when the map is loaded
	void executeOnLoadEvents();

	void drawRenderable(std::vector<Renderable>::iterator r_cursor);

	// renders one layer into the given sdl surface with offset
	// offset in screen/surface coordinates.
	void renderIsoLayer(SDL_Surface *wheretorender, Point offset, const unsigned short layerdata[][256]);

	// renders only objects
	void renderIsoBackObjects(std::vector<Renderable> &r);

	// renders interleaved objects and layer
	void renderIsoFrontObjects(std::vector<Renderable> &r);
	void renderIso(std::vector<Renderable> &r, std::vector<Renderable> &r_dead);

	void renderOrthoLayer(const unsigned short layerdata[][256]);
	void renderOrthoBackObjects(std::vector<Renderable> &r);
	void renderOrthoFrontObjects(std::vector<Renderable> &r);
	void renderOrtho(std::vector<Renderable> &r, std::vector<Renderable> &r_dead);

	void clearLayers();
	void clearQueues();

	Point shakycam;
	bool new_music;
	TileSet tset;
	std::string tileset;
	std::string music_filename;

	// in case of animated tiles switched off, prerender background into this layer
	SDL_Surface* backgroundsurface;

	// only called if the background surface is actually used, i.e. ANIMATED_TILES = false;
	void createBackgroundSurface();

	// point where the backgroundsurface was rendered. same coordinates as shakycam
	Point backgroundsurfaceoffset;

	// force a rendering of the background in the next render step.
	bool repaint_background;

public:
	CampaignManager *camp;
	PowerManager *powers;

	// functions
	MapRenderer(CampaignManager *_camp);
	~MapRenderer();

	int load(std::string filename);
	void logic();
	void render(std::vector<Renderable> &r, std::vector<Renderable> &r_dead);

	void clearEvents();
	void checkEvents(Point loc);
	void checkHotspots();
	void checkNearestEvent(Point loc);
	void checkTooltip();

	// vars
	std::string title;
	short w;
	short h;

	// cam(x,y) is where on the map the camera is pointing
	// units found in Settings.h (UNITS_PER_TILE)
	Point cam;
	Point hero_tile;
	Point spawn;
	int spawn_dir;

	// indicates that the map was changed by an event, so the GameStatePlay
	// will tell the mini map to update.
	bool map_change;

	MapCollision collider;

	// enemy load handling
	std::queue<Map_Enemy> enemies;

	// npc load handling
	std::queue<Map_NPC> npcs;

	// event-created loot or items
	std::vector<Event_Component> loot;

	// teleport handling
	bool teleportation;
	Point teleport_destination;
	std::string teleport_mapname;
	std::string respawn_map;
	Point respawn_point;

	// cutscene handling
	bool cutscene;
	std::string cutscene_file;

	// message handling
	std::string log_msg;

	// shaky cam
	int shaky_cam_ticks;

	// stash handling
	bool stash;
	Point stash_pos;

	// enemy clear
	bool enemies_cleared;

	// event talker
	std::string event_npc;
};


#endif
