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
 * class NPCManager
 *
 * NPCs which are not combatative enemies are handled by this Manager.
 * Most commonly this involves vendor and conversation townspeople.
 */

#include "FileParser.h"
#include "NPCManager.h"
#include "NPC.h"
#include "SharedResources.h"
#include "MapRenderer.h"
#include "LootManager.h"
#include "StatBlock.h"

using namespace std;


NPCManager::NPCManager(MapRenderer *_map, LootManager *_loot, ItemManager *_items, StatBlock *_stats) {

	stats = _stats;
	map = _map;
	loot = _loot;
	items = _items;

	tip = new WidgetTooltip();

	FileParser infile;
	// load tooltip_margin from engine config file
	if (infile.open(mods->locate("engine/tooltips.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "npc_tooltip_margin") {
				tooltip_margin = atoi(infile.val.c_str());
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open engine/tooltips.txt!\n");
	
}

void NPCManager::addRenders(std::vector<Renderable> &r) {
	for (unsigned i=0; i<npcs.size(); i++) {
		r.push_back(npcs[i]->getRender());
	}
}

void NPCManager::handleNewMap() {

	Map_NPC mn;
	ItemStack item_roll;

	// remove existing NPCs
	for (unsigned i=0; i<npcs.size(); i++)
		delete(npcs[i]);

	npcs.clear();

	// read the queued NPCs in the map file
	while (!map->npcs.empty()) {
		mn = map->npcs.front();
		map->npcs.pop();

		NPC *npc = new NPC(map, items);
		npc->load(mn.id, stats->level);
		npc->pos.x = mn.pos.x;
		npc->pos.y = mn.pos.y;

		// if this NPC needs randomized items
		while (npc->random_stock > 0 && npc->stock_count < NPC_VENDOR_MAX_STOCK) {
			item_roll.item = loot->randomItem(npc->level);
			item_roll.quantity = rand() % items->items[item_roll.item].rand_vendor + 1;
			npc->stock.add(item_roll);
			npc->random_stock--;
		}
		npc->stock.sort();
		npcs.push_back(npc);
	}

}

void NPCManager::logic() {
	for (unsigned i=0; i<npcs.size(); i++) {
		npcs[i]->logic();
	}
}

int NPCManager::checkNPCClick(Point mouse, Point cam) {
	Point p;
	SDL_Rect r;
	for (unsigned i=0; i<npcs.size(); i++) {

		p = map_to_screen(npcs[i]->pos.x, npcs[i]->pos.y, cam.x, cam.y);

		r.w = npcs[i]->render_size.x;
		r.h = npcs[i]->render_size.y;
		r.x = p.x - npcs[i]->render_offset.x;
		r.y = p.y - npcs[i]->render_offset.y;

		if (isWithin(r, mouse)) {
			return i;
		}
	}
	return -1;
}

/**
 * On mouseover, display NPC's name
 */
void NPCManager::renderTooltips(Point cam, Point mouse) {
	Point p;
	SDL_Rect r;

	for (unsigned i=0; i<npcs.size(); i++) {

		p = map_to_screen(npcs[i]->pos.x, npcs[i]->pos.y, cam.x, cam.y);

		r.w = npcs[i]->render_size.x;
		r.h = npcs[i]->render_size.y;
		r.x = p.x - npcs[i]->render_offset.x;
		r.y = p.y - npcs[i]->render_offset.y;

		if (isWithin(r, mouse)) {

			// adjust dest.y so that the tooltip floats above the item
			p.y -= tooltip_margin;

			// use current tip or make a new one?
			if (tip_buf.lines[0] != npcs[i]->name) {
				tip_buf.clear();
				tip_buf.num_lines = 1;
				tip_buf.lines[0] = npcs[i]->name;
			}

			tip->render(tip_buf, p, STYLE_TOPLABEL);

			break; // display only one NPC tooltip at a time
		}
	}
}

NPCManager::~NPCManager() {
	for (unsigned i=0; i<npcs.size(); i++) {
		delete npcs[i];
	}

	tip_buf.clear();
	delete tip;
}
