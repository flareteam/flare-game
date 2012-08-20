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

#include "MapRenderer.h"
#include "CampaignManager.h"
#include "EnemyGroupManager.h"
#include "FileParser.h"
#include "SharedResources.h"
#include "PowerManager.h"
#include "StatBlock.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"

#include <iostream>
using namespace std;

MapRenderer::MapRenderer(CampaignManager *_camp) {

	camp = _camp;

	tip = new WidgetTooltip();

	// cam(x,y) is where on the map the camera is pointing
	// units found in Settings.h (UNITS_PER_TILE)
	cam.x = 0;
	cam.y = 0;

	new_music = false;
	map_change = false;

	clearEvents();
	enemy_awaiting_queue = false;
	npc_awaiting_queue = false;
	group_awaiting_queue = false;
	new_enemy.clear();
	new_npc.clear();
	new_group.clear();

	sfx = NULL;
	sfx_filename = "";
	music = NULL;
	log_msg = "";
	shaky_cam_ticks = 0;

	backgroundsurface = 0;
	backgroundsurfaceoffset.x = 0;
	backgroundsurfaceoffset.y = 0;
	repaint_background = false;
	stash = false;
}

void MapRenderer::clearEvents() {
	events.clear();
}

void MapRenderer::playSFX(string filename) {
	// only load from file if the requested soundfx isn't already loaded
	if (filename != sfx_filename) {
		Mix_FreeChunk(sfx);
		sfx = NULL;
		if (audio) {
			sfx = Mix_LoadWAV(mods->locate(filename).c_str());
			sfx_filename = filename;
		}
	}
	if (sfx) Mix_PlayChannel(-1, sfx, 0);
}

void MapRenderer::push_enemy_group(Map_Group g) {
	// activate at all?
	float activate_chance = (rand() % 100) / 100.0f;
	if (activate_chance > g.chance) {
		return;
	}

	// populate valid_locations
	vector<Point> valid_locations;
	Point pt;
	for (int width = 0; width < g.area.x; width++) {
		for (int height = 0; height < g.area.y; height++) {
			pt.x = (g.pos.x + width) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
			pt.y = (g.pos.y + height) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
			if (collider.is_empty(pt.x, pt.y)) {
				valid_locations.push_back(pt);
			}
		}
	}
	//remove locations that already have an enemy on them
	Map_Enemy test_enemy;
	for (size_t i = 0; i < enemies.size(); i++) {
		test_enemy = enemies.front();
		enemies.pop();
		enemies.push(test_enemy);
		for (size_t j = 0; j < valid_locations.size(); j++) {
			if ( (test_enemy.pos.x == valid_locations.at(j).x) && (test_enemy.pos.y == valid_locations.at(j).y) ) {
				valid_locations.erase(valid_locations.begin() + j);
			}
		}
	}

	// spawn the appropriate number of enemies
	int number = rand() % (g.numbermax + 1 - g.numbermin) + g.numbermin;

	for(int i = 0; i < number; i++) {
		Enemy_Level enemy_lev = EnemyGroupManager::instance().getRandomEnemy(g.category, g.levelmin, g.levelmax);
		Map_Enemy group_member;
		if ((enemy_lev.type != "") && (valid_locations.size() != 0)){
			group_member.clear();
			group_member.type = enemy_lev.type;
			int index = rand() % valid_locations.size();
			group_member.pos = valid_locations.at(index);
			valid_locations.erase(valid_locations.begin() + index);
			group_member.direction = rand() % 8;
			enemies.push(group_member);
		}
	}
}

/**
 * load
 */
int MapRenderer::load(string filename) {
	FileParser infile;
	string val;
	string cur_layer;
	string data_format;

	clearEvents();

	bool collider_set = false;

	if (infile.open(mods->locate("maps/" + filename))) {
		while (infile.next()) {
			if (infile.new_section) {
				data_format = "dec"; // default

				if (enemy_awaiting_queue) {
					enemies.push(new_enemy);
					enemy_awaiting_queue = false;
				}
				if (npc_awaiting_queue) {
					npcs.push(new_npc);
					npc_awaiting_queue = false;
				}
				if (group_awaiting_queue) {
					push_enemy_group(new_group);
					group_awaiting_queue = false;
				}

				// for sections that are stored in collections, add a new object here
				if (infile.section == "enemy") {
					new_enemy.clear();
					enemy_awaiting_queue = true;
				}
				else if (infile.section == "enemygroup") {
					new_group.clear();
					group_awaiting_queue = true;
				}
				else if (infile.section == "npc") {
					new_npc.clear();
					npc_awaiting_queue = true;
				}
				else if (infile.section == "event") {
					events.push_back(Map_Event());
				}

			}
			if (infile.section == "header") {
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
					if (this->music_filename == infile.val) {
						this->new_music = false;
					}
					else {
						this->music_filename = infile.val;
						this->new_music = true;
					}
				}
				else if (infile.key == "location") {
					spawn.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					spawn.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					spawn_dir = toInt(infile.nextValue());
				}
			}
			else if (infile.section == "layer") {
				if (infile.key == "type") {
					cur_layer = infile.val;
				}
				else if (infile.key == "format") {
					data_format = infile.val;
				}
				else if (infile.key == "data") {
					// layer map data handled as a special case

					// The next h lines must contain layer data.  TODO: err
					if (data_format == "hex") {
						for (int j=0; j<h; j++) {
							val = infile.getRawLine() + ',';
							for (int i=0; i<w; i++) {
								if (cur_layer == "background") background[i][j] = eatFirstHex(val, ',');
								else if (cur_layer == "object") object[i][j] = eatFirstHex(val, ',');
								else if (cur_layer == "collision") collision[i][j] = eatFirstHex(val, ',');
							}
						}
					}
					else if (data_format == "dec") {
						for (int j=0; j<h; j++) {
							val = infile.getRawLine() + ',';
							for (int i=0; i<w; i++) {
								if (cur_layer == "background") background[i][j] = eatFirstInt(val, ',');
								else if (cur_layer == "object") object[i][j] = eatFirstInt(val, ',');
								else if (cur_layer == "collision") collision[i][j] = eatFirstInt(val, ',');
							}
						}
					}
					if ((cur_layer == "collision") && !collider_set) {
						collider.setmap(collision);
						collider.map_size.x = w;
						collider.map_size.y = h;
					}
				}
			}
			else if (infile.section == "enemy") {
				if (infile.key == "type") {
					new_enemy.type = infile.val;
				}
				else if (infile.key == "location") {
					new_enemy.pos.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					new_enemy.pos.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
				}
				else if (infile.key == "direction") {
					new_enemy.direction = toInt(infile.val);
				}
				else if (infile.key == "waypoints") {
					string none = "";
					string a = infile.nextValue();
					string b = infile.nextValue();

					while (a != none) {
						Point p;
						p.x = toInt(a) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
						p.y = toInt(b) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
						new_enemy.waypoints.push(p);
						a = infile.nextValue();
						b = infile.nextValue();
					}
				} else if (infile.key == "wander_area") {
					new_enemy.wander = true;
					new_enemy.wander_area.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
					new_enemy.wander_area.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
					new_enemy.wander_area.w = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
					new_enemy.wander_area.h = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE / 2;
				}
			}
			else if (infile.section == "enemygroup") {
				if (infile.key == "type") {
					new_group.category = infile.val;
				}
				else if (infile.key == "level") {
					new_group.levelmin = toInt(infile.nextValue());
					new_group.levelmax = toInt(infile.nextValue());
				}
				else if (infile.key == "location") {
					new_group.pos.x = toInt(infile.nextValue());
					new_group.pos.y = toInt(infile.nextValue());
					new_group.area.x = toInt(infile.nextValue());
					new_group.area.y = toInt(infile.nextValue());
				}
				else if (infile.key == "number") {
					new_group.numbermin = toInt(infile.nextValue());
					new_group.numbermax = toInt(infile.nextValue());
				}
				else if (infile.key == "chance") {
					new_group.chance = toInt(infile.nextValue()) / 100.0f;
					if (new_group.chance > 1.0f) {
						new_group.chance = 1.0f;
					}
					if (new_group.chance < 0.0f) {
						new_group.chance = 0.0f;
					}
				}
			}
			else if (infile.section == "npc") {
				if (infile.key == "type") {
					new_npc.id = infile.val;
				}
				else if (infile.key == "location") {
					new_npc.pos.x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
					new_npc.pos.y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
				}
			}
			else if (infile.section == "event") {
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
					events.back().hotspot.x = toInt(infile.nextValue());
					events.back().hotspot.y = toInt(infile.nextValue());
					events.back().hotspot.w = toInt(infile.nextValue());
					events.back().hotspot.h = toInt(infile.nextValue());
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
					events.back().cooldown = toInt(infile.val);
				}
				else {
					// new event component
					Event_Component *e = &(events.back()).components[events.back().comp_num];
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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
							e->type = infile.key;
							e->s = repeat_val;
							e->x = toInt(infile.nextValue());
							e->y = toInt(infile.nextValue());
							e->z = toInt(infile.nextValue());

							repeat_val = infile.nextValue();
						}
					}
					else if (infile.key == "soundfx") {
						e->s = infile.val;
					}
					else if (infile.key == "loot") {
						e->s = infile.nextValue();
						e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
						e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
						e->z = toInt(infile.nextValue());

						// add repeating loot
						string repeat_val = infile.nextValue();
						while (repeat_val != "") {
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
							e->type = infile.key;
							e->s = repeat_val;
							e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
							e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
							e->z = toInt(infile.nextValue());

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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
							e->type = infile.key;
							e->s = repeat_val;

							repeat_val = infile.nextValue();
						}
					}
					else if (infile.key == "requires_item") {
						e->x = toInt(infile.nextValue());

						// add repeating requires_item
						string repeat_val = infile.nextValue();
						while (repeat_val != "") {
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
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
							events.back().comp_num++;
							e = &events.back().components[events.back().comp_num];
							e->type = infile.key;

							e->s = repeat_val;
							e->x = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;
							e->y = toInt(infile.nextValue()) * UNITS_PER_TILE + UNITS_PER_TILE/2;

							repeat_val = infile.nextValue();
						}
					}
					events.back().comp_num++;
				}
			}
		}

		infile.close();

		// reached end of file.  Handle any final sections.
		if (enemy_awaiting_queue) {
			enemies.push(new_enemy);
			enemy_awaiting_queue = false;
		}
		if (npc_awaiting_queue) {
			npcs.push(new_npc);
			npc_awaiting_queue = false;
		}
		if (group_awaiting_queue){
			push_enemy_group(new_group);
			group_awaiting_queue = false;
		}
	}

	if (this->new_music) {
		loadMusic();
		this->new_music = false;
	}
	tset.load(this->tileset);

	// some events automatically trigger when the map loads
	// e.g. change map state based on campaign status
	executeOnLoadEvents();

	return 0;
}

void MapRenderer::loadMusic() {

	if (music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}
	if (audio && MUSIC_VOLUME) {
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
	for (it = events.begin(); it < events.end(); it++) {
		if ((*it).cooldown_ticks > 0) (*it).cooldown_ticks--;
	}

}

/**
 * Sort in the same order as the tiles are drawn
 * Depends upon the map implementation
 */
bool zcompare_iso(const Renderable &r1, const Renderable &r2) {
	const int x1 = r1.tile.x;
	const int x2 = r2.tile.x;
	const int z1 = r1.tile.y + x1;
	const int z2 = r2.tile.y + x2;
	if (z1 > z2)
		return false;
	else if (z1 == z2) {
		if (x1 > x2) {
			return false;
		}
		else if (x1 == x2) {
			// same tile, sort by subtile
			if (r1.map_pos.x + r1.map_pos.y >= r2.map_pos.x + r2.map_pos.y)
				return false;
		}
	}
	return true;
}

bool zcompare_ortho(const Renderable &r1, const Renderable &r2) {
	const int x1 = r1.tile.x;
	const int x2 = r2.tile.x;
	const int y1 = r1.tile.y;
	const int y2 = r2.tile.y;
	if (y1 > y2) {
		return false;
	}
	else if (y1 == y2) {
		if (x1 > x2) {
			return false;
		}
		else if (x1 == x2) {
			if (r1.map_pos.y >= r2.map_pos.y)
				return false;
			else
				if (r1.map_pos.x >= r2.map_pos.x)
					return false;
		}
	}
	return true;
}

void MapRenderer::render(vector<Renderable> &r, vector<Renderable> &r_dead) {

	vector<Renderable>::iterator it;
	for (it = r.begin(); it != r.end(); it++) {
		// calculate tile
		it->tile.x = it->map_pos.x >> TILE_SHIFT;
		it->tile.y = it->map_pos.y >> TILE_SHIFT;
	}
	for (it = r_dead.begin(); it != r_dead.end(); it++) {
		// calculate tile
		it->tile.x = it->map_pos.x >> TILE_SHIFT;
		it->tile.y = it->map_pos.y >> TILE_SHIFT;
	}

	if (shaky_cam_ticks == 0) {
		shakycam.x = cam.x;
		shakycam.y = cam.y;
	}
	else {
		shakycam.x = cam.x + (rand() % 16 - 8) /UNITS_PER_PIXEL_X;
		shakycam.y = cam.y + (rand() % 16 - 8) /UNITS_PER_PIXEL_Y;
	}

	if (TILESET_ORIENTATION == TILESET_ORTHOGONAL) {
		std::sort(r.begin(), r.end(), zcompare_ortho);
		std::sort(r_dead.begin(), r_dead.end(), zcompare_ortho);
		renderOrtho(r, r_dead);
	} else {
		std::sort(r.begin(), r.end(), zcompare_iso);
		std::sort(r_dead.begin(), r_dead.end(), zcompare_iso);
		renderIso(r, r_dead);
	}
}

void MapRenderer::createBackgroundSurface() {

	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	SDL_FreeSurface(backgroundsurface);
	SDL_Surface *surface;
	if (HWSURFACE)
		surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, 2 * VIEW_W, 2 * VIEW_H, 32, rmask, gmask, bmask, amask);
	else
		surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 2 * VIEW_W, 2 * VIEW_H, 32, rmask, gmask, bmask, amask);

	if (surface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
	}
	backgroundsurface = SDL_DisplayFormat(surface);
	SDL_FreeSurface(surface);
}

void MapRenderer::drawRenderable(vector<Renderable>::iterator r_cursor) {
	SDL_Rect dest;
	Point p = map_to_screen(r_cursor->map_pos.x, r_cursor->map_pos.y, shakycam.x, shakycam.y);
	dest.x = p.x - r_cursor->offset.x;
	dest.y = p.y - r_cursor->offset.y;
	SDL_BlitSurface(r_cursor->sprite, &r_cursor->src, screen, &dest);
}

void MapRenderer::renderIsoBackground(SDL_Surface *wheretorender, Point offset) {
	short int i;
	short int j;
	SDL_Rect dest;

	const Point upperright = screen_to_map(0, 0, shakycam.x, shakycam.y);
	const short tiles_outside_ofscreen = 12;
	const short max_tiles_width = (VIEW_W / TILE_W) + 2 * tiles_outside_ofscreen;
	const short max_tiles_height = (2 * VIEW_H / TILE_H) + 2 * tiles_outside_ofscreen;
	j = upperright.y / UNITS_PER_TILE;
	i = upperright.x / UNITS_PER_TILE - tiles_outside_ofscreen;

	for (unsigned short y = max_tiles_height ; y; --y) {
		short tiles_width = 0;

		// make sure the isometric corners are not rendered:
		if (i < -1) {
			j += i + 1;
			tiles_width -= i + 1;
			i = -1;
		}
		short d = j - h;
		if (d >= 0) {
			j -= d; tiles_width += d; i += d;
		}
		short j_end = std::max((j+i-w+1), std::max(j - max_tiles_width, 0));

		// draw one horizontal line
		while (j > j_end) {
			--j; ++i;
			++tiles_width;

			unsigned short current_tile = background[i][j];

			if (current_tile) {
				Point p = map_to_screen(i * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
				p = center_tile(p);
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
	for (it = r.begin(); it != r.end(); it++) {
		drawRenderable(it);
	}
}

void MapRenderer::renderIsoFrontObjects(vector<Renderable> &r) {
	short int i;
	short int j;
	SDL_Rect dest;
	const Point upperright = screen_to_map(0, 0, shakycam.x, shakycam.y);
	const short tiles_outside_ofscreen = 12;
	const short max_tiles_width = (VIEW_W / TILE_W) + 2 * tiles_outside_ofscreen;
	const short max_tiles_height = (2 * VIEW_H / TILE_H) + 2 * tiles_outside_ofscreen;

	vector<Renderable>::iterator r_cursor = r.begin();
	vector<Renderable>::iterator r_end = r.end();

	// object layer
	j = upperright.y / UNITS_PER_TILE;
	i = upperright.x / UNITS_PER_TILE - tiles_outside_ofscreen;

	while (r_cursor != r_end && (r_cursor->tile.x + r_cursor->tile.y < i + j || r_cursor->tile.x < i))
		r_cursor++;

	for (unsigned short y = max_tiles_height ; y; --y) {
		short tiles_width = 0;

		// make sure the isometric corners are not rendered:
		if (i < -1) {
			j += i + 1;
			tiles_width -= i + 1;
			i = -1;
		}
		short d = j - h;
		if (d >= 0) {
			j -= d; tiles_width += d; i += d;
		}
		short j_end = std::max((j+i-w+1), std::max(j - max_tiles_width, 0));

		// draw one horizontal line
		while (j > j_end) {
			--j; ++i;
			++tiles_width;

			unsigned short current_tile = object[i][j];

			if (current_tile) {
				Point p = map_to_screen(i * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
				p = center_tile(p);
				dest.x = p.x - tset.tiles[current_tile].offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y;
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
			}

			// some renderable entities go in this layer
			while (r_cursor != r_end && r_cursor->tile.x == i && r_cursor->tile.y == j) {
				drawRenderable(r_cursor);
				r_cursor++;
			}
		}
		j += tiles_width;
		i -= tiles_width;
		if (y % 2)
			i++;
		else
			j++;
		while (r_cursor != r_end && (r_cursor->tile.x + r_cursor->tile.y < i + j || r_cursor->tile.x <= i))
			r_cursor++;
	}
}

void MapRenderer::renderIso(vector<Renderable> &r, vector<Renderable> &r_dead) {
	const Point nulloffset = {0, 0};
	if (ANIMATED_TILES) {
		renderIsoBackground(screen, nulloffset);
	}
	else {
		if (abs(shakycam.x - backgroundsurfaceoffset.x) > 4 * UNITS_PER_TILE
			|| abs(shakycam.y - backgroundsurfaceoffset.y) > 4 * UNITS_PER_TILE
			|| repaint_background) {

			if (!backgroundsurface)
				createBackgroundSurface();

			repaint_background = false;

			backgroundsurfaceoffset = shakycam;

			SDL_FillRect(backgroundsurface, 0, 0);
			Point off = {VIEW_W_HALF, VIEW_H_HALF};
			renderIsoBackground(backgroundsurface, off);
		}
		Point p = map_to_screen(shakycam.x, shakycam.y , backgroundsurfaceoffset.x, backgroundsurfaceoffset.y);
		SDL_Rect src;
		src.x = p.x;
		src.y = p.y;
		src.w = 2 * VIEW_W;
		src.h = 2 * VIEW_H;
		SDL_BlitSurface(backgroundsurface, &src, screen , 0);
	}
	renderIsoBackObjects(r_dead);
	renderIsoFrontObjects(r);
	checkTooltip();
}

void MapRenderer::renderOrthoBackground() {
	short int i;
	short int j;
	SDL_Rect dest;
	unsigned short current_tile;

	for (j=0; j<h; j++) {
		for (i=0; i<w; i++) {

			current_tile = background[i][j];

			if (current_tile) {
				Point p = map_to_screen(i * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
				p = center_tile(p);
				dest.x = p.x - tset.tiles[current_tile].offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y;
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
			}
		}
	}
}

void MapRenderer::renderOrthoBackObjects(std::vector<Renderable> &r) {
	// some renderables are drawn above the background and below the objects
	vector<Renderable>::iterator it;
	for (it = r.begin(); it != r.end(); it++)
		drawRenderable(it);
}

void MapRenderer::renderOrthoFrontObjects(std::vector<Renderable> &r) {
	short int i;
	short int j;
	SDL_Rect dest;
	unsigned short current_tile;
	vector<Renderable>::iterator r_cursor = r.begin();
	vector<Renderable>::iterator r_end = r.end();

	// TODO: trim by screen rect
	// object layer
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++) {

			current_tile = object[i][j];

			if (current_tile) {
				Point p = map_to_screen(i * UNITS_PER_TILE, j * UNITS_PER_TILE, shakycam.x, shakycam.y);
				p = center_tile(p);
				dest.x = p.x - tset.tiles[current_tile].offset.x;
				dest.y = p.y - tset.tiles[current_tile].offset.y;
				SDL_BlitSurface(tset.sprites, &(tset.tiles[current_tile].src), screen, &dest);
			}

			// some renderable entities go in this layer
			while (r_cursor != r_end && r_cursor->tile.x == i && r_cursor->tile.y == j) {
				drawRenderable(r_cursor);
				r_cursor++;
			}
		}
	}
}

void MapRenderer::renderOrtho(vector<Renderable> &r, vector<Renderable> &r_dead) {

	renderOrthoBackground();
	renderOrthoBackObjects(r_dead);
	renderOrthoFrontObjects(r);
	//render event tooltips
	checkTooltip();
}

void MapRenderer::executeOnLoadEvents() {
	vector<Map_Event>::iterator it;

	// loop in reverse because we may erase elements
	for (it = events.end(); it != events.begin(); ) {
		it--;

		// skip inactive events
		if (!isActive(*it)) continue;

		if ((*it).type == "on_load") {
			if (executeEvent(*it))
				events.erase(it);
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
		it--;

		// skip inactive events
		if (!isActive(*it)) continue;

		if (maploc.x >= (*it).location.x &&
			maploc.y >= (*it).location.y &&
			maploc.x <= (*it).location.x + (*it).location.w-1 &&
			maploc.y <= (*it).location.y + (*it).location.h-1) {
			if (executeEvent(*it))
				events.erase(it);
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
void MapRenderer::checkEventClick() {

	// only check events if the player is clicking
	// and allowed to click
	if (!inpt->pressing[MAIN1]) return;
	else if (inpt->lock[MAIN1]) return;

	Point p;
	SDL_Rect r;
	vector<Map_Event>::iterator it;

	// work backwards through events because events can be erased in the loop.
	// this prevents the iterator from becoming invalid.
	for (it = events.end(); it != events.begin(); ) {
		it--;

		// skip inactive events
		if (!isActive(*it)) continue;

		// skip events without hotspots
		if ((*it).hotspot.h == 0) continue;

		// skip events on cooldown
		if ((*it).cooldown_ticks != 0) continue;

		p = map_to_screen((*it).location.x * UNITS_PER_TILE + UNITS_PER_TILE/2, (*it).location.y * UNITS_PER_TILE + UNITS_PER_TILE/2, cam.x, cam.y);
		r.x = p.x + (*it).hotspot.x;
		r.y = p.y + (*it).hotspot.y;
		r.h = (*it).hotspot.h;
		r.w = (*it).hotspot.w;

		// execute if mouse in hotspot && hero within range
		if (isWithin(r, inpt->mouse)
				&& (abs(cam.x - (*it).location.x * UNITS_PER_TILE) < CLICK_RANGE
				&& abs(cam.y - (*it).location.y * UNITS_PER_TILE) < CLICK_RANGE)) {

			inpt->lock[MAIN1] = true;
			if (executeEvent(*it))
				events.erase(it);
		}
	}
}

bool MapRenderer::isActive(const Map_Event &e){
	for (int i=0; i < e.comp_num; i++) {
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
	}
	return true;
}

void MapRenderer::checkTooltip() {
	Point p;
	SDL_Rect r;
	Point tip_pos;

	vector<Map_Event>::iterator it;
	for (it = events.begin(); it != events.end(); it++) {
		if(!isActive(*it)) continue;

		p = map_to_screen((*it).location.x * UNITS_PER_TILE + UNITS_PER_TILE/2, (*it).location.y * UNITS_PER_TILE + UNITS_PER_TILE/2, cam.x, cam.y);
		r.x = p.x + (*it).hotspot.x;
		r.y = p.y + (*it).hotspot.y;
		r.h = (*it).hotspot.h;
		r.w = (*it).hotspot.w;

		// DEBUG TOOL: outline hotspot
#ifdef DEBUG
		SDL_Rect screen_size;
		screen_size.x = screen_size.y = 0;
		screen_size.w = VIEW_W;
		screen_size.h = VIEW_H;
		Point pixpos;
		pixpos.x = r.x;
		pixpos.y = r.y;
		if (isWithin(screen_size, pixpos))
			drawPixel(screen, r.x, r.y, 255);
		pixpos.x = r.x+r.w;
		pixpos.y = r.y;
		if (isWithin(screen_size, pixpos))
			drawPixel(screen, r.x+r.w, r.y, 255);
		pixpos.x = r.x;
		pixpos.y = r.y+r.h;
		if (isWithin(screen_size, pixpos))
			drawPixel(screen, r.x, r.y+r.h, 255);
		pixpos.x = r.x+r.w;
		pixpos.y = r.y+r.h;
		if (isWithin(screen_size, pixpos))
			drawPixel(screen, r.x+r.w, r.y+r.h, 255);
#endif

		if (isWithin(r,inpt->mouse) && (*it).tooltip != "") {

			// new tooltip?
			if (tip_buf.lines[0] != (*it).tooltip) {
				tip_buf.clear();
				tip_buf.num_lines = 1;
				tip_buf.lines[0] = (*it).tooltip;
			}

			tip_pos.x = r.x + r.w/2;
			// FIXME should depend on art resolution
			tip_pos.y = r.y;
			tip->render(tip_buf, tip_pos, STYLE_TOPLABEL);
		}
	}
}

/**
 * A particular event has been triggered.
 * Process all of this events components.
 *
 * @param The triggered event
 * @return Returns true if the event shall not be run again.
 */
bool MapRenderer::executeEvent(Map_Event &ev) {

	// skip executing events that are on cooldown
	if (ev.cooldown_ticks > 0) return false;

	// set cooldown
	ev.cooldown_ticks = ev.cooldown;

	const Event_Component *ec;
	bool destroy_event = false;

	for (int i=0; i<ev.comp_num; i++) {
		ec = &ev.components[i];

		// requirements should be checked by isActive() before calling executeEvent()
		//if (ec->type == "requires_status") {
		//	if (!camp->checkStatus(ec->s)) return false;
		//}
		//else if (ec->type == "requires_not") {
		//	if (camp->checkStatus(ec->s)) return false;
		//}
		//else if (ec->type == "requires_item") {
		//	if (!camp->checkItem(ec->x)) return false;
		//}

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
			else if (ec->s == "background") {
				background[ec->x][ec->y] = ec->z;
				repaint_background = false;
			}
			map_change = true;
		}
		else if (ec->type == "soundfx") {
			playSFX(ec->s);
		}
		else if (ec->type == "loot") {
			loot.push(*ec);
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
			camp->rewardXP(ec->x);
		}
		else if (ec->type == "spawn") {
			Point spawn_pos;
			spawn_pos.x = ec->x;
			spawn_pos.y = ec->y;
			powers->spawn(ec->s, spawn_pos);
		}
		else if (ec->type == "power") {

			int power_index = ec->x;

			// TODO: delete this without breaking hazards, takeHit, etc.
			StatBlock *dummy = new StatBlock();
			dummy->accuracy = 1000; //always hits its target

			// if a power path was specified, place the source position there
			if (ev.power_src.x > 0) {
				dummy->pos.x = ev.power_src.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
				dummy->pos.y = ev.power_src.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
			}
			// otherwise the source position is the event position
			else {
				dummy->pos.x = ev.location.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
				dummy->pos.y = ev.location.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
			}

			dummy->dmg_melee_min = dummy->dmg_ranged_min = dummy->dmg_ment_min = ev.damagemin;
			dummy->dmg_melee_max = dummy->dmg_ranged_max = dummy->dmg_ment_max = ev.damagemax;

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
				target.x = dummy->pos.x;
				target.y = dummy->pos.y;
			}

			powers->activate(power_index, dummy, target);

		}
		else if (ec->type == "stash") {
			stash = true;
			stash_pos.x = ev.location.x * UNITS_PER_TILE + UNITS_PER_TILE/2;
			stash_pos.y = ev.location.y * UNITS_PER_TILE + UNITS_PER_TILE/2;
		}
	}
	if (ev.type == "run_once" || ev.type == "on_load" || destroy_event)
		return true;
	else
		return false;
}

MapRenderer::~MapRenderer() {
	if (music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
	}
	Mix_FreeChunk(sfx);

	SDL_FreeSurface(backgroundsurface);
	tip_buf.clear();
	delete tip;
}

