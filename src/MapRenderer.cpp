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

#include "MapRenderer.h"
#include "CampaignManager.h"
#include "EnemyGroupManager.h"
#include "FileParser.h"
#include "SharedResources.h"
#include "PowerManager.h"
#include "StatBlock.h"
#include "UtilsFileSystem.h"
#include "UtilsMath.h"
#include "UtilsParsing.h"

#include <stdint.h>

#include <iostream>
#include <limits>
using namespace std;

const int CLICK_RANGE = 3 * UNITS_PER_TILE; //for activating events

MapRenderer::MapRenderer(CampaignManager *_camp)
 : music(NULL)
 , tip(new WidgetTooltip())
 , tip_pos()
 , show_tooltip(false)
 , events()
 , background(NULL)
 , fringe(NULL)
 , object(NULL)
 , foreground(NULL)
 , collision(NULL)
 , shakycam(Point())
 , backgroundsurface(NULL)
 , backgroundsurfaceoffset()
 , repaint_background(false)
 , camp(_camp)
 , powers(NULL)
 , w(0)
 , h(0)
 , cam()
 , hero_tile()
 , spawn()
 , spawn_dir(0)
 , map_change(false)
 , teleportation(false)
 , teleport_destination()
 , respawn_point()
 , log_msg("")
 , shaky_cam_ticks(0)
 , stash(false)
 , stash_pos()
 , enemies_cleared(false)
{
}

void MapRenderer::clearEvents() {
	events.clear();
}

void MapRenderer::push_enemy_group(Map_Group g) {
	// activate at all?
	float activate_chance = (rand() % 100) / 100.0f;
	if (activate_chance > g.chance) {
		return;
	}

	// The algorithm tries to place the enemies at random locations.
	// However if a location is not possible (unwalkable or there is already an entity),
	// then try again.
	// This could result in an infinite loop if there were more enemies than
	// actual places, so have an upper bound of tries.

	// random number of enemies
	int enemies_to_spawn = randBetween(g.numbermin, g.numbermax);

	// pick an upper bound, which is definitely larger than threetimes the enemy number to spawn.
	int allowed_misses = 3 * g.numbermax;

	while (enemies_to_spawn && allowed_misses) {

		int x = (g.pos.x + (rand() % g.area.x)) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
		int y = (g.pos.y + (rand() % g.area.y)) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
		bool success = false;

		if (collider.is_empty(x, y)) {
			Enemy_Level enemy_lev = EnemyGroupManager::instance().getRandomEnemy(g.category, g.levelmin, g.levelmax);
			if (enemy_lev.type != ""){
				Map_Enemy group_member = Map_Enemy(enemy_lev.type, Point(x, y));
				enemies.push(group_member);

				success = true;
			}
		}
		if (success)
			enemies_to_spawn--;
		else
			allowed_misses--;
	}
}

int MapRenderer::load(string filename) {
	FileParser infile;
	maprow *cur_layer = NULL;

	clearEvents();
	clearLayers();
	clearQueues();

	std::queue<Map_Group> enemy_groups;

	/* unload sounds */
	snd->reset();
	while (!sids.empty()) {
		snd->unload(sids.back());
		sids.pop_back();
	}

	show_tooltip = false;

	if (!infile.open(mods->locate("maps/" + filename))) {
		cerr << "Unable to open maps/" << filename << endl;
		return 0;
	}

	while (infile.next()) {
		if (infile.new_section) {

			// for sections that are stored in collections, add a new object here
			if (infile.section == "enemy")
				enemies.push(Map_Enemy());
			else if (infile.section == "enemygroup")
				enemy_groups.push(Map_Group());
			else if (infile.section == "npc")
				npcs.push(Map_NPC());
			else if (infile.section == "event")
				events.push_back(Map_Event());

		}
		if (infile.section == "header")
			loadHeader(infile);
		else if (infile.section == "layer")
			loadLayer(infile, &cur_layer);
		else if (infile.section == "enemy")
			loadEnemy(infile);
		else if (infile.section == "enemygroup")
			loadEnemyGroup(infile, &enemy_groups.back());
		else if (infile.section == "npc")
			loadNPC(infile);
		else if (infile.section == "event")
			loadEvent(infile);
	}

	infile.close();

	while (!enemy_groups.empty()) {
		push_enemy_group(enemy_groups.front());
		enemy_groups.pop();
	}

	tset.load(this->tileset);

	// some events automatically trigger when the map loads
	// e.g. change map state based on campaign status
	executeOnLoadEvents();

	return 0;
}

void MapRenderer::loadHeader(FileParser &infile)
{
	if (infile.key == "title") {
		this->title = msg->get(infile.val);
	}
	else if (infile.key == "width") {
		this->w = toInt(infile.val);
	}
	else if (infile.key == "height") {
		this->h = toInt(infile.val);
	}
	else if (infile.key == "tileset") {
		this->tileset = infile.val;
	}
	else if (infile.key == "music") {
		loadMusic(infile.val);
	}
	else if (infile.key == "location") {
		spawn.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
		spawn.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
		spawn_dir = toInt(infile.nextValue());
	}
}

void MapRenderer::loadLayer(FileParser &infile, maprow **current_layer)
{
	if (infile.key == "type") {
		*current_layer = new maprow[w];
		if (infile.val == "background") background = *current_layer;
		else if (infile.val == "fringe") fringe = *current_layer;
		else if (infile.val == "object") object = *current_layer;
		else if (infile.val == "foreground") foreground = *current_layer;
		else if (infile.val == "collision") collision = *current_layer;
	}
	else if (infile.key == "format") {
		if (infile.val != "dec") {
			fprintf(stderr, "ERROR: maploading: The format of a layer must be \"dec\"!\n");
			SDL_Quit();
			exit(1);
		}
	}
	else if (infile.key == "data") {
		// layer map data handled as a special case
		// The next h lines must contain layer data.  TODO: err
		for (int j=0; j<h; j++) {
			string val = infile.getRawLine() + ',';
			for (int i=0; i<w; i++)
				(*current_layer)[i][j] = eatFirstInt(val, ',');
		}
		if ((*current_layer) == collision)
			collider.setmap(collision, w, h);
	}
}

void MapRenderer::loadEnemy(FileParser &infile)
{
	if (infile.key == "type") {
		enemies.back().type = infile.val;
	}
	else if (infile.key == "location") {
		enemies.back().pos.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
		enemies.back().pos.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
	}
	else if (infile.key == "direction") {
		enemies.back().direction = toInt(infile.val);
	}
	else if (infile.key == "waypoints") {
		string none = "";
		string a = infile.nextValue();
		string b = infile.nextValue();

		while (a != none) {
			Point p;
			p.x = toInt(a) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
			p.y = toInt(b) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
			enemies.back().waypoints.push(p);
			a = infile.nextValue();
			b = infile.nextValue();
		}
	} else if (infile.key == "wander_area") {
		enemies.back().wander = true;
		enemies.back().wander_area.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
		enemies.back().wander_area.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
		enemies.back().wander_area.w = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
		enemies.back().wander_area.h = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
	}
}

void MapRenderer::loadEnemyGroup(FileParser &infile, Map_Group *group)
{
	if (infile.key == "type") {
		group->category = infile.val;
	}
	else if (infile.key == "level") {
		group->levelmin = toInt(infile.nextValue());
		group->levelmax = toInt(infile.nextValue());
	}
	else if (infile.key == "location") {
		group->pos.x = toInt(infile.nextValue());
		group->pos.y = toInt(infile.nextValue());
		group->area.x = toInt(infile.nextValue());
		group->area.y = toInt(infile.nextValue());
	}
	else if (infile.key == "number") {
		group->numbermin = toInt(infile.nextValue());
		group->numbermax = toInt(infile.nextValue());
	}
	else if (infile.key == "chance") {
		float n = toInt(infile.nextValue()) / 100.0f;
		group->chance = min(1.0f, max(0.0f, n));
	}
}

void MapRenderer::loadNPC(FileParser &infile)
{
	if (infile.key == "type") {
		npcs.back().id = infile.val;
	}
	else if (infile.key == "location") {
		npcs.back().pos.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
		npcs.back().pos.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
	}
}

void MapRenderer::loadEvent(FileParser &infile)
{
	if (infile.key == "type") {
		events.back().type = infile.val;
	}
	else if (infile.key == "location") {
		events.back().location.x = toInt(infile.nextValue());
		events.back().location.y = toInt(infile.nextValue());
		events.back().location.w = toInt(infile.nextValue());
		events.back().location.h = toInt(infile.nextValue());
	}
	else if (infile.key == "hotspot") {
		if (infile.val == "location") {
			events.back().hotspot.x = events.back().location.x;
			events.back().hotspot.y = events.back().location.y;
			events.back().hotspot.w = events.back().location.w;
			events.back().hotspot.h = events.back().location.h;
		}
		else {
			events.back().hotspot.x = toInt(infile.nextValue());
			events.back().hotspot.y = toInt(infile.nextValue());
			events.back().hotspot.w = toInt(infile.nextValue());
			events.back().hotspot.h = toInt(infile.nextValue());
		}
	}
	else if (infile.key == "tooltip") {
		events.back().tooltip = msg->get(infile.val);
	}
	else if (infile.key == "power_path") {
		events.back().power_src.x = toInt(infile.nextValue());
		events.back().power_src.y = toInt(infile.nextValue());
		string dest = infile.nextValue();
		if (dest == "hero") {
			events.back().targetHero = true;
		}
		else {
			events.back().power_dest.x = toInt(dest);
			events.back().power_dest.y = toInt(infile.nextValue());
		}
	}
	else if (infile.key == "power_damage") {
		events.back().damagemin = toInt(infile.nextValue());
		events.back().damagemax = toInt(infile.nextValue());
	}
	else if (infile.key == "cooldown") {
		events.back().cooldown = parse_duration(infile.val);
	}
	else {
		loadEventComponent(infile);
	}
}

void MapRenderer::loadEventComponent(FileParser &infile)
{
	// new event component
	events.back().components.push_back(Event_Component());
	Event_Component *e = &events.back().components.back();
	e->type = infile.key;

	if (infile.key == "intermap") {
		e->s = infile.nextValue();
		e->x = toInt(infile.nextValue());
		e->y = toInt(infile.nextValue());
	}
	else if (infile.key == "intramap") {
		e->x = toInt(infile.nextValue());
		e->y = toInt(infile.nextValue());
	}
	else if (infile.key == "mapmod") {
		e->s = infile.nextValue();
		e->x = toInt(infile.nextValue());
		e->y = toInt(infile.nextValue());
		e->z = toInt(infile.nextValue());

		// add repeating mapmods
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->s = repeat_val;
			e->x = toInt(infile.nextValue());
			e->y = toInt(infile.nextValue());
			e->z = toInt(infile.nextValue());

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "soundfx") {
		e->s = infile.nextValue();
		e->x = e->y = -1;

		std::string s = infile.nextValue();
		if (s != "") e->x = toInt(s);

		s = infile.nextValue();
		if (s != "") e->y = toInt(s);

	}
	else if (infile.key == "loot") {
		e->s = infile.nextValue();
		e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
		e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;

		// drop chance
		string chance = infile.nextValue();
		if (chance == "fixed") e->z = 0;
		else e->z = toInt(chance);

		// quantity min/max
		e->a = toInt(infile.nextValue());
		if (e->a < 1) e->a = 1;
		e->b = toInt(infile.nextValue());
		if (e->b < e->a) e->b = e->a;

		// add repeating loot
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->s = repeat_val;
			e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
			e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;

			string chance = infile.nextValue();
			if (chance == "fixed") e->z = 0;
			else e->z = toInt(chance);

			e->a = toInt(infile.nextValue());
			if (e->a < 1) e->a = 1;
			e->b = toInt(infile.nextValue());
			if (e->b < e->a) e->b = e->a;

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "msg") {
		e->s = msg->get(infile.val);
	}
	else if (infile.key == "shakycam") {
		e->x = toInt(infile.val);
	}
	else if (infile.key == "requires_status") {
		e->s = infile.nextValue();

		// add repeating requires_status
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->s = repeat_val;

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "requires_not") {
		e->s = infile.nextValue();

		// add repeating requires_not
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->s = repeat_val;

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "requires_level") {
		e->x = toInt(infile.nextValue());
	}
	else if (infile.key == "requires_not_level") {
		e->x = toInt(infile.nextValue());
	}
	else if (infile.key == "requires_item") {
		e->x = toInt(infile.nextValue());

		// add repeating requires_item
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->x = toInt(repeat_val);

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "set_status") {
		e->s = infile.nextValue();

		// add repeating set_status
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->s = repeat_val;

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "unset_status") {
		e->s = infile.nextValue();

		// add repeating unset_status
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->s = repeat_val;

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "remove_item") {
		e->x = toInt(infile.nextValue());

		// add repeating remove_item
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;
			e->x = toInt(repeat_val);

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "reward_xp") {
		e->x = toInt(infile.val);
	}
	else if (infile.key == "power") {
		e->x = toInt(infile.val);
	}
	else if (infile.key == "spawn") {

		e->s = infile.nextValue();
		e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
		e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;

		// add repeating spawn
		string repeat_val = infile.nextValue();
		while (repeat_val != "") {
			events.back().components.push_back(Event_Component());
			e = &events.back().components.back();
			e->type = infile.key;

			e->s = repeat_val;
			e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
			e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;

			repeat_val = infile.nextValue();
		}
	}
	else if (infile.key == "npc") {
		npcs.back().id = infile.val;
		e->s = infile.val;
	}
	else if (infile.key == "music") {
		e->s = infile.val;
	}
}

void MapRenderer::clearQueues() {
	enemies = queue<Map_Enemy>();
	npcs = queue<Map_NPC>();
	loot.clear();
}

/**
 * No guarantee that maps will use all layers
 * Clear all tile layers (e.g. when loading a map)
 */
void MapRenderer::clearLayers() {
	delete[] background;
	delete[] fringe;
	delete[] object;
	delete[] foreground;
	delete[] collision;

	background = 0;
	fringe = 0;
	object = 0;
	foreground = 0;
	collision = 0;

	SDL_FreeSurface(backgroundsurface);
	backgroundsurface = 0;
}

void MapRenderer::loadMusic(const std::string &new_music_filename) {

	// keep playing if already the correct track
	if (music_filename == new_music_filename)
		return;

	music_filename = new_music_filename;

	if (music) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}
	if (AUDIO && MUSIC_VOLUME) {
		music = Mix_LoadMUS(mods->locate("music/" + this->music_filename).c_str());
		if(!music)
			cout << "Mix_LoadMUS: "<< Mix_GetError()<<endl;
	}

	if (music) {
		Mix_VolumeMusic(MUSIC_VOLUME);
		Mix_PlayMusic(music, -1);
	}
}

void MapRenderer::logic() {

	// handle camera shaking timer
	if (shaky_cam_ticks > 0) shaky_cam_ticks--;

	// handle tile set logic e.g. animations
	tset.logic();

	// handle event cooldowns
	vector<Map_Event>::iterator it;
	for (it = events.begin(); it < events.end(); ++it) {
		if ((*it).cooldown_ticks > 0) (*it).cooldown_ticks--;
	}

}

bool priocompare(const Renderable &r1, const Renderable &r2) {
	return r1.prio < r2.prio;
}

/**
 * Sort in the same order as the tiles are drawn
 * Depends upon the map implementation
 */
void calculatePriosIso(vector<Renderable> &r) {
	for (vector<Renderable>::iterator it = r.begin(); it != r.end(); ++it) {
		const unsigned tilex = it->map_pos.x >> TILE_SHIFT;
		const unsigned tiley = it->map_pos.y >> TILE_SHIFT;
		it->prio += (((uint64_t)(tilex + tiley)) << 48) + (((uint64_t)tilex) << 32) + ((it->map_pos.x + it->map_pos.y) << 16);
	}
}

void calculatePriosOrtho(vector<Renderable> &r) {
	for (vector<Renderable>::iterator it = r.begin(); it != r.end(); ++it) {
		const unsigned tilex = it->map_pos.x >> TILE_SHIFT;
		const unsigned tiley = it->map_pos.y >> TILE_SHIFT;
		it->prio += (((uint64_t)tiley) << 48) + (((uint64_t)tilex) << 32) + (it->map_pos.y << 16);
	}
}

void MapRenderer::render(vector<Renderable> &r, vector<Renderable> &r_dead) {

	if (shaky_cam_ticks == 0) {
		shakycam.x = cam.x;
		shakycam.y = cam.y;
	}
	else {
		shakycam.x = cam.x + (rand() % 16 - 8) /UNITS_PER_PIXEL_X;
		shakycam.y = cam.y + (rand() % 16 - 8) /UNITS_PER_PIXEL_Y;
	}

	if (TILESET_ORIENTATION == TILESET_ORTHOGONAL) {
		calculatePriosOrtho(r);
		calculatePriosOrtho(r_dead);
		std::sort(r.begin(), r.end(), priocompare);
		std::sort(r_dead.begin(), r_dead.end(), priocompare);
		renderOrtho(r, r_dead);
	} else {
		calculatePriosIso(r);
		calculatePriosIso(r_dead);
		std::sort(r.begin(), r.end(), priocompare);
		std::sort(r_dead.begin(), r_dead.end(), priocompare);
		renderIso(r, r_dead);
	}
}

void MapRenderer::createBackgroundSurface() {
	SDL_FreeSurface(backgroundsurface);
	backgroundsurface = createSurface(
			VIEW_W + 2 * movedistance_to_rerender * TILE_W * tset.max_size_x,
			VIEW_H + 2 * movedistance_to_rerender * TILE_H * tset.max_size_y);
	// background has no alpha:
	SDL_SetColorKey(backgroundsurface, 0, 0);
}

void MapRenderer::drawRenderable(vector<Renderable>::iterator r_cursor) {
	if (r_cursor->sprite) {
		SDL_Rect dest;
		Point p = map_to_screen(r_cursor->map_pos.x, r_cursor->map_pos.y, shakycam.x, shakycam.y);
		dest.x = p.x - r_cursor->offset.x;
		dest.y = p.y - r_cursor->offset.y;
		SDL_BlitSurface(r_cursor->sprite, &r_cursor->src, screen, &dest);
	}
}

void MapRenderer::renderIsoLayer(SDL_Surface *wheretorender, Point offset, const unsigned short layerdata[256][256]) {
	short int i;
	short int j;
	SDL_Rect dest;
	const Point upperright = screen_to_map(0, 0, shakycam.x, shakycam.y);
	const short max_tiles_width =   (VIEW_W / TILE_W) + 2 * tset.max_size_x;
	const short max_tiles_height = ((2 * VIEW_H / TILE_H) + 2 * tset.max_size_y) * 2;

	j = upperright.y / UNITS_PER_TILE - tset.max_size_y + tset.max_size_x;
	i = upperright.x / UNITS_PER_TILE - tset.max_size_y - tset.max_size_x;

	for (unsigned short y = max_tiles_height ; y; --y) {
		short tiles_width = 0;

		// make sure the isometric corners are not rendered:
		if (i < -1) {
			j += i + 1;
			tiles_width -= i + 1;
			i = -1;
		}
		const short d = j - h;
		if (d >= 0) {
			j -= d; tiles_width += d; i += d;
		}
		const short j_end = std::max((j+i-w+1), std::max(j - max_tiles_width, 0));


		Point p = map_to_screen(i * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
		p = center_tile(p);
		// draw one horizontal line
		while (j > j_end) {
			--j;
			++i;
			++tiles_width;
			p.x += TILE_W;

			const unsigned short current_tile = layerdata[i][j];

			if (current_tile) {
				dest.x = p.x - tset.tiles[current_tile].offset.x + offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y + offset.y;
				// no need to set w and h in dest, as it is ignored
				// by SDL_BlitSurface
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), wheretorender, &dest);
			}
		}
		j += tiles_width;
		i -= tiles_width;
		if (y % 2)
			i++;
		else
			j++;
	}
}

void MapRenderer::renderIsoBackObjects(vector<Renderable> &r) {
	vector<Renderable>::iterator it;
	for (it = r.begin(); it != r.end(); ++it)
		drawRenderable(it);
}

void MapRenderer::renderIsoFrontObjects(vector<Renderable> &r) {
	short int i;
	short int j;
	SDL_Rect dest;
	const Point upperright = screen_to_map(0, 0, shakycam.x, shakycam.y);
	const short max_tiles_width =   (VIEW_W / TILE_W) + 2 * tset.max_size_x;
	const short max_tiles_height = ((VIEW_H / TILE_H) + 2 * tset.max_size_y)*2;

	vector<Renderable>::iterator r_cursor = r.begin();
	vector<Renderable>::iterator r_end = r.end();

	// object layer
	j = upperright.y / UNITS_PER_TILE - tset.max_size_y + tset.max_size_x;
	i = upperright.x / UNITS_PER_TILE - tset.max_size_y - tset.max_size_x;

	while (r_cursor != r_end && ((r_cursor->map_pos.x>>TILE_SHIFT) + (r_cursor->map_pos.y>>TILE_SHIFT) < i + j || (r_cursor->map_pos.x>>TILE_SHIFT) < i))
		++r_cursor;

	for (unsigned short y = max_tiles_height ; y; --y) {
		short tiles_width = 0;

		// make sure the isometric corners are not rendered:
		if (i < -1) {
			j += i + 1;
			tiles_width -= i + 1;
			i = -1;
		}
		const short d = j - h;
		if (d >= 0) {
			j -= d; tiles_width += d; i += d;
		}
		const short j_end = std::max((j+i-w+1), std::max(j - max_tiles_width, 0));

		// draw one horizontal line
		Point p = map_to_screen(i * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
		p = center_tile(p);
		while (j > j_end) {
			--j;
			++i;
			++tiles_width;
			p.x += TILE_W;

			const unsigned short current_tile = object[i][j];

			if (current_tile) {

				dest.x = p.x - tset.tiles[current_tile].offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y;
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
			}

			// some renderable entities go in this layer
			while (r_cursor != r_end && (r_cursor->map_pos.x>>TILE_SHIFT) == i && (r_cursor->map_pos.y>>TILE_SHIFT) == j) {
				drawRenderable(r_cursor);
				++r_cursor;
			}
		}
		j += tiles_width;
		i -= tiles_width;
		if (y % 2)
			i++;
		else
			j++;
		while (r_cursor != r_end && ((r_cursor->map_pos.x>>TILE_SHIFT) + (r_cursor->map_pos.y>>TILE_SHIFT) < i + j || (r_cursor->map_pos.x>>TILE_SHIFT) <= i))
			++r_cursor;
	}
}

void MapRenderer::renderIso(vector<Renderable> &r, vector<Renderable> &r_dead) {
	const Point nulloffset(0, 0);
	if (ANIMATED_TILES) {
		if (background) renderIsoLayer(screen, nulloffset, background);
		if (fringe) renderIsoLayer(screen, nulloffset, fringe);
	}
	else {
		if (abs(shakycam.x - backgroundsurfaceoffset.x) > movedistance_to_rerender * TILE_W
			|| abs(shakycam.y - backgroundsurfaceoffset.y) > movedistance_to_rerender * TILE_H
			|| repaint_background) {

			if (!backgroundsurface)
				createBackgroundSurface();

			repaint_background = false;

			backgroundsurfaceoffset = shakycam;

			SDL_FillRect(backgroundsurface, 0, 0);
			Point off(VIEW_W_HALF, VIEW_H_HALF);
			if (background) renderIsoLayer(backgroundsurface, off, background);
			if (fringe) renderIsoLayer(backgroundsurface, off, fringe);
		}
		Point p = map_to_screen(shakycam.x, shakycam.y , backgroundsurfaceoffset.x, backgroundsurfaceoffset.y);
		SDL_Rect src;
		src.x = p.x;
		src.y = p.y;
		src.w = 2 * VIEW_W;
		src.h = 2 * VIEW_H;
		SDL_BlitSurface(backgroundsurface, &src, screen , 0);
	}
	if (object) renderIsoBackObjects(r_dead);
	if (object) renderIsoFrontObjects(r);
	if (foreground) renderIsoLayer(screen, nulloffset, foreground);
	checkTooltip();
}

void MapRenderer::renderOrthoLayer(const unsigned short layerdata[256][256]) {

	const Point upperright = screen_to_map(0, 0, shakycam.x, shakycam.y);

	short int startj = max(0, upperright.y / UNITS_PER_TILE);
	short int starti = max(0, upperright.x / UNITS_PER_TILE);
	const short max_tiles_width =  min(w, static_cast<short int>(starti + (VIEW_W / TILE_W) + 2 * tset.max_size_x));
	const short max_tiles_height = min(h, static_cast<short int>(startj + (VIEW_H / TILE_H) + 2 * tset.max_size_y));

	short int i;
	short int j;

	for (j = startj; j < max_tiles_height; j++) {
		Point p = map_to_screen(starti * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
		p = center_tile(p);
		for (i = starti; i < max_tiles_width; i++) {

			if (const unsigned short current_tile = layerdata[i][j]) {
				SDL_Rect dest;
				dest.x = p.x - tset.tiles[current_tile].offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y;
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
			}
			p.x += TILE_W;
		}
	}
}

void MapRenderer::renderOrthoBackObjects(std::vector<Renderable> &r) {
	// some renderables are drawn above the background and below the objects
	vector<Renderable>::iterator it;
	for (it = r.begin(); it != r.end(); ++it)
		drawRenderable(it);
}

void MapRenderer::renderOrthoFrontObjects(std::vector<Renderable> &r) {

	short int i;
	short int j;
	SDL_Rect dest;
	vector<Renderable>::iterator r_cursor = r.begin();
	vector<Renderable>::iterator r_end = r.end();

	const Point upperright = screen_to_map(0, 0, shakycam.x, shakycam.y);

	short int startj = max(0, upperright.y / UNITS_PER_TILE);
	short int starti = max(0, upperright.x / UNITS_PER_TILE);
	const short max_tiles_width =  min(w, static_cast<short int>(starti + (VIEW_W / TILE_W) + 2 * tset.max_size_x));
	const short max_tiles_height = min(h, static_cast<short int>(startj + (VIEW_H / TILE_H) + 2 * tset.max_size_y));

	while (r_cursor != r_end && (r_cursor->map_pos.y>>TILE_SHIFT) < startj)
		++r_cursor;

	for (j = startj; j<max_tiles_height; j++) {
		Point p = map_to_screen(starti * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
		p = center_tile(p);
		for (i = starti; i<max_tiles_width; i++) {

			if (const unsigned short current_tile = object[i][j]) {
				dest.x = p.x - tset.tiles[current_tile].offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y;
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
			}
			p.x += TILE_W;

			while (r_cursor != r_end && (r_cursor->map_pos.y>>TILE_SHIFT) == j && (r_cursor->map_pos.x>>TILE_SHIFT) < i)
				++r_cursor;

			// some renderable entities go in this layer
			while (r_cursor != r_end && (r_cursor->map_pos.y>>TILE_SHIFT) == j && (r_cursor->map_pos.x>>TILE_SHIFT) == i)
				drawRenderable(r_cursor++);
		}
		while (r_cursor != r_end && (r_cursor->map_pos.y>>TILE_SHIFT) <= j)
			++r_cursor;
	}
}

void MapRenderer::renderOrtho(vector<Renderable> &r, vector<Renderable> &r_dead) {
	if (background) renderOrthoLayer(background);
	if (fringe) renderOrthoLayer(fringe);
	if (object) renderOrthoBackObjects(r_dead);
	if (object) renderOrthoFrontObjects(r);
	if (foreground) renderOrthoLayer(foreground);
	//render event tooltips
	checkTooltip();
}

void MapRenderer::executeOnLoadEvents() {
	vector<Map_Event>::iterator it;

	// loop in reverse because we may erase elements
	for (it = events.end(); it != events.begin(); ) {
		--it;

		// skip inactive events
		if (!isActive(*it)) continue;

		if ((*it).type == "on_load") {
			if (executeEvent(*it))
				it = events.erase(it);
		}
	}
}


void MapRenderer::checkEvents(Point loc) {
	Point maploc;
	maploc.x = loc.x >> TILE_SHIFT;
	maploc.y = loc.y >> TILE_SHIFT;
	vector<Map_Event>::iterator it;

	// loop in reverse because we may erase elements
	for (it = events.end(); it != events.begin(); ) {
		--it;

		// skip inactive events
		if (!isActive(*it)) continue;

		if ((*it).type == "on_clear") {
			if (enemies_cleared && executeEvent(*it))
				it = events.erase(it);
		}
		else if (maploc.x >= (*it).location.x &&
			maploc.y >= (*it).location.y &&
			maploc.x <= (*it).location.x + (*it).location.w-1 &&
			maploc.y <= (*it).location.y + (*it).location.h-1) {
			if (executeEvent(*it))
				it = events.erase(it);
		}
	}
}

/**
 * Some events have a hotspot (rectangle screen area) where the user can click
 * to trigger the event.
 *
 * The hero must be within range (CLICK_RANGE) to activate an event.
 *
 * This function checks valid mouse clicks against all clickable events, and
 * executes
 */
void MapRenderer::checkHotspots() {
	show_tooltip = false;

	vector<Map_Event>::iterator it;

	// work backwards through events because events can be erased in the loop.
	// this prevents the iterator from becoming invalid.
	for (it = events.end(); it != events.begin(); ) {
		--it;

		for (int x=it->hotspot.x; x < it->hotspot.x + it->hotspot.w; ++x) {
			for (int y=it->hotspot.y; y < it->hotspot.y + it->hotspot.h; ++y) {

				bool backgroundmatch = false;
				bool objectmatch = false;

				Point p = map_to_screen(x * UNITS_PER_TILE,
										y * UNITS_PER_TILE,
										shakycam.x,
										shakycam.y);
				p = center_tile(p);

				if (const short current_tile = background[x][y]) {
					// first check if mouse pointer is in rectangle of that tile:
					SDL_Rect dest;
					dest.x = p.x - tset.tiles[current_tile].offset.x;
					dest.y = p.y - tset.tiles[current_tile].offset.y;
					dest.w = tset.tiles[current_tile].src.w;
					dest.h = tset.tiles[current_tile].src.h;

					if (isWithin(dest, inpt->mouse)) {
						// Now that the mouse is within the rectangle of the tile, we can check for
						// pixel precision. We need to have checked the rectangle first, because
						// otherwise the pixel precise check might hit a neighbouring tile in the
						// tileset. We need to calculate the point relative to the
						Point p1;
						p1.x = inpt->mouse.x - dest.x + tset.tiles[current_tile].src.x;
						p1.y = inpt->mouse.y - dest.y + tset.tiles[current_tile].src.y;
						backgroundmatch = checkPixel(p1, tset.sprites);
						tip_pos.x = dest.x + dest.w/2;
						tip_pos.y = dest.y;
					}
				}
				if (const short current_tile = object[x][y]) {
					SDL_Rect dest;
					dest.x = p.x - tset.tiles[current_tile].offset.x;
					dest.y = p.y - tset.tiles[current_tile].offset.y;
					dest.w = tset.tiles[current_tile].src.w;
					dest.h = tset.tiles[current_tile].src.h;

					if (isWithin(dest, inpt->mouse)) {
						Point p1;
						p1.x = inpt->mouse.x - dest.x + tset.tiles[current_tile].src.x;
						p1.y = inpt->mouse.y - dest.y + tset.tiles[current_tile].src.y;
						objectmatch = checkPixel(p1, tset.sprites);
						tip_pos.x = dest.x + dest.w/2;
						tip_pos.y = dest.y;
					}
				}
				if (backgroundmatch || objectmatch) {
					// skip inactive events
					if (!isActive(*it)) continue;

					// skip events without hotspots
					if ((*it).hotspot.h == 0) continue;

					// skip events on cooldown
					if ((*it).cooldown_ticks != 0) continue;

					// new tooltip?
					if (!(*it).tooltip.empty() && TOOLTIP_CONTEXT != TOOLTIP_MENU) {
						show_tooltip = true;
						if (!tip_buf.compareFirstLine((*it).tooltip)) {
							tip_buf.clear();
							tip_buf.addText((*it).tooltip);
						}
						TOOLTIP_CONTEXT = TOOLTIP_MAP;
					} else if (TOOLTIP_CONTEXT != TOOLTIP_MENU) {
						TOOLTIP_CONTEXT = TOOLTIP_NONE;
					}

					if ((abs(cam.x - (*it).location.x * UNITS_PER_TILE) < CLICK_RANGE)
						&& (abs(cam.y - (*it).location.y * UNITS_PER_TILE) < CLICK_RANGE)) {

						// only check events if the player is clicking
						// and allowed to click
						if (!inpt->pressing[MAIN1]) return;
						else if (inpt->lock[MAIN1]) return;

						inpt->lock[MAIN1] = true;
						if (executeEvent(*it))
							it = events.erase(it);
					}
					return;
				} else show_tooltip = false;
			}
		}
	}
}

void MapRenderer::checkNearestEvent(Point loc) {
	if (inpt->pressing[ACCEPT] && !inpt->lock[ACCEPT]) {
		if (inpt->pressing[ACCEPT]) inpt->lock[ACCEPT] = true;

		vector<Map_Event>::iterator it;
		vector<Map_Event>::iterator nearest;
		int best_distance = std::numeric_limits<int>::max();

		// loop in reverse because we may erase elements
		for (it = events.end(); it != events.begin(); ) {
			--it;

			// skip inactive events
			if (!isActive(*it)) continue;

			// skip events without hotspots
			if ((*it).hotspot.h == 0) continue;

			// skip events on cooldown
			if ((*it).cooldown_ticks != 0) continue;

			Point ev_loc;
			ev_loc.x = (*it).location.x * UNITS_PER_TILE;
			ev_loc.y = (*it).location.y * UNITS_PER_TILE;
			int distance = (int)calcDist(loc,ev_loc);
			if (distance < CLICK_RANGE && distance < best_distance) {
				best_distance = distance;
				nearest = it;
			}
		}
		if (executeEvent(*nearest))
			events.erase(nearest);
	}
}

bool MapRenderer::isActive(const Map_Event &e){
	for (unsigned i=0; i < e.components.size(); i++) {
		if (e.components[i].type == "requires_not") {
			if (camp->checkStatus(e.components[i].s)) {
				return false;
			}
		}
		else if (e.components[i].type == "requires_status") {
			if (!camp->checkStatus(e.components[i].s)) {
				return false;
			}
		}
		else if (e.components[i].type == "requires_item") {
			if (!camp->checkItem(e.components[i].x)) {
				return false;
			}
		}
		else if (e.components[i].type == "requires_level") {
			if (camp->hero->level < e.components[i].x) {
				return false;
			}
		}
		else if (e.components[i].type == "requires_not_level") {
			if (camp->hero->level >= e.components[i].x) {
				return false;
			}
		}
	}
	return true;
}

void MapRenderer::checkTooltip() {
	if (show_tooltip)
		tip->render(tip_buf, tip_pos, STYLE_TOPLABEL);
}

/**
 * A particular event has been triggered.
 * Process all of this events components.
 *
 * @param The triggered event
 * @return Returns true if the event shall not be run again.
 */
bool MapRenderer::executeEvent(Map_Event &ev) {
	if(&ev == NULL) return false;

	// skip executing events that are on cooldown
	if (ev.cooldown_ticks > 0) return false;

	// set cooldown
	ev.cooldown_ticks = ev.cooldown;

	const Event_Component *ec;
	bool destroy_event = false;

	for (unsigned i=0; i<ev.components.size(); i++) {
		ec = &ev.components[i];

		if (ec->type == "set_status") {
			camp->setStatus(ec->s);
		}
		else if (ec->type == "unset_status") {
			camp->unsetStatus(ec->s);
		}
		else if (ec->type == "intermap") {

			if (fileExists(mods->locate("maps/" + ec->s))) {
				teleportation = true;
				teleport_mapname = ec->s;
				teleport_destination.x = ec->x * UNITS_PER_TILE + UNITS_PER_TILE/2;
				teleport_destination.y = ec->y * UNITS_PER_TILE + UNITS_PER_TILE/2;
			}
			else {
				destroy_event = true;
				log_msg = msg->get("Unknown destination");
			}
		}
		else if (ec->type == "intramap") {
			teleportation = true;
			teleport_mapname = "";
			teleport_destination.x = ec->x * UNITS_PER_TILE + UNITS_PER_TILE/2;
			teleport_destination.y = ec->y * UNITS_PER_TILE + UNITS_PER_TILE/2;
		}
		else if (ec->type == "mapmod") {
			if (ec->s == "collision") {
				collision[ec->x][ec->y] = ec->z;
				collider.colmap[ec->x][ec->y] = ec->z;
			}
			else if (ec->s == "object") {
				object[ec->x][ec->y] = ec->z;
			}
			else if (ec->s == "foreground") {
				foreground[ec->x][ec->y] = ec->z;
			}
			else if (ec->s == "fringe") {
				fringe[ec->x][ec->y] = ec->z;
			}
			else if (ec->s == "background") {
				background[ec->x][ec->y] = ec->z;
				repaint_background = false;
			}
			map_change = true;
		}
		else if (ec->type == "soundfx") {
			Point pos(0,0);
			bool loop = false;

			if (ec->x != -1 && ec->y != -1) {
				if (ec->x != 0 && ec->y != 0) {
					pos.x = ec->x * UNITS_PER_TILE + UNITS_PER_TILE/2;
					pos.y = ec->y * UNITS_PER_TILE + UNITS_PER_TILE/2;
				}
			}
			else if (ev.location.x != 0 && ev.location.y != 0) {
				pos.x = ev.location.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
				pos.y = ev.location.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
			}

			if (ev.type == "on_load")
				loop = true;

			SoundManager::SoundID sid = snd->load(ec->s, "MapRenderer background soundfx");

			snd->play(sid, GLOBAL_VIRTUAL_CHANNEL, pos, loop);
			sids.push_back(sid);
		}
		else if (ec->type == "loot") {
			loot.push_back(*ec);
		}
		else if (ec->type == "msg") {
			log_msg = ec->s;
		}
		else if (ec->type == "shakycam") {
			shaky_cam_ticks = ec->x;
		}
		else if (ec->type == "remove_item") {
			camp->removeItem(ec->x);
		}
		else if (ec->type == "reward_xp") {
			camp->rewardXP(ec->x, true);
		}
		else if (ec->type == "spawn") {
			Point spawn_pos;
			spawn_pos.x = ec->x;
			spawn_pos.y = ec->y;
			powers->spawn(ec->s, spawn_pos);
		}
		else if (ec->type == "power") {

			int power_index = ec->x;

			if (ev.stats == NULL) {
				ev.stats = new StatBlock();

				ev.stats->accuracy = 1000; //always hits its target

				// if a power path was specified, place the source position there
				if (ev.power_src.x > 0) {
					ev.stats->pos.x = ev.power_src.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
					ev.stats->pos.y = ev.power_src.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
				}
				// otherwise the source position is the event position
				else {
					ev.stats->pos.x = ev.location.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
					ev.stats->pos.y = ev.location.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
				}

				ev.stats->dmg_melee_min = ev.stats->dmg_ranged_min = ev.stats->dmg_ment_min = ev.damagemin;
				ev.stats->dmg_melee_max = ev.stats->dmg_ranged_max = ev.stats->dmg_ment_max = ev.damagemax;
			}

			Point target;

			// if a power path was specified:
			// targets hero option
			if (ev.targetHero) {
				target.x = cam.x;
				target.y = cam.y;
			}
			// targets fixed path option
			else if (ev.power_dest.x != 0) {
				target.x = ev.power_dest.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
				target.y = ev.power_dest.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
			}
			// no path specified, targets self location
			else {
				target.x = ev.stats->pos.x;
				target.y = ev.stats->pos.y;
			}

			powers->activate(power_index, ev.stats, target);

		}
		else if (ec->type == "stash") {
			stash = true;
			stash_pos.x = ev.location.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
			stash_pos.y = ev.location.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
		}
		else if (ec->type == "npc") {
			event_npc = ec->s;
		}
		else if (ec->type == "music") {
			if (this->music_filename != ec->s) {
				this->music_filename = ec->s;
				loadMusic(ec->s);
			}
		}
	}
	if (ev.type == "run_once" || ev.type == "on_load" || ev.type == "on_clear" || destroy_event)
		return true;
	else
		return false;
}

MapRenderer::~MapRenderer() {
	if (music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
	}

	tip_buf.clear();
	clearLayers();
	clearEvents();
	clearQueues();
	delete tip;

	/* unload sounds */
	snd->reset();
	while (!sids.empty()) {
		snd->unload(sids.back());
		sids.pop_back();
	}
}

