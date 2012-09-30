/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Stefan Beller

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
	Event_Component components[256];
	int comp_num;
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
		cooldown = 0;
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
		cooldown_ticks = 0;
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

	void clear() {
		pos.x = 0;
		pos.y = 0;
		// enemies face a random direction unless otherwise specified
		direction = rand() % 8;
		type = "";
		std::queue<Point> empty;
		waypoints = empty;
		wander = false;
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


	typedef unsigned short maprow[256];

	maprow *background;
	maprow *fringe;
	maprow *object; // must exist in each map!
	maprow *foreground;
	maprow *collision; // must exist in each map!

	// This determines the size of the backgroundsurface outside the actual
	// screen. So this setting depends on the movedistance_to_rerender and
	// also on the size of the individual tiles. if the tiles are large, you
	// need to choose a larger value here. (i.e. consider trees having a huge
	// height, so you need to paint them even if the tree stump are out of the screen.
	// For isometric mode this is actually only half of the number of tiles
	// outside the screen. For orthogonal projection this is the number of tiles
	// at each side of the screen, so if your tree has a height of 4 tiles,
	// you should make this setting at least 4 + movedistance_to_rerender.
	static const short tiles_outside_of_screen = 12;

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
	void checkTooltip();

	// vars
	std::string title;
	short w;
	short h;
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

	// stash handling
	bool stash;
	Point stash_pos;
};


#endif
