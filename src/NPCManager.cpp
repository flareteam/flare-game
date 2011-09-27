/*
Copyright 2011 Clint Bellanger

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

#include "NPCManager.h"

NPCManager::NPCManager(MapIso *_map, MenuTooltip *_tip, LootManager *_loot, ItemDatabase *_items) {

	map = _map;
	tip = _tip;
	loot = _loot;
	items = _items;

	npc_count = 0;
	for (int i=0; i<MAX_NPC_COUNT; i++) {
		npcs[i] = NULL;
	}
	
	tooltip_margin = 64;
}

void NPCManager::handleNewMap() {
	
	Map_NPC mn;
	ItemStack item_roll;
	
	// remove existing NPCs
	for (int i=0; i<npc_count; i++) {
		delete(npcs[i]);
		npcs[i] = NULL;
	}
	
	npc_count = 0;
	
	// read the queued NPCs in the map file
	while (!map->npcs.empty()) {
		mn = map->npcs.front();
		map->npcs.pop();
		
		npcs[npc_count] = new NPC(map, items);
		npcs[npc_count]->load(mn.id);
		npcs[npc_count]->pos.x = mn.pos.x;
		npcs[npc_count]->pos.y = mn.pos.y;
		
		// if this NPC needs randomized items
		while (npcs[npc_count]->random_stock > 0 && npcs[npc_count]->stock_count < NPC_VENDOR_MAX_STOCK) {
			item_roll.item = loot->randomItem(npcs[npc_count]->level);
			item_roll.quantity = rand() % items->items[item_roll.item].rand_vendor + 1;
			npcs[npc_count]->stock.add( item_roll);
			npcs[npc_count]->random_stock--;
		}
		npcs[npc_count]->stock.sort();

		npc_count++;
	}

}

void NPCManager::logic() {
	for (int i=0; i<npc_count; i++) {
		npcs[i]->logic();
	}
}

int NPCManager::checkNPCClick(Point mouse, Point cam) {
	Point p;
	SDL_Rect r;
	for(int i=0; i<npc_count; i++) {

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
	TooltipData td;
	
	for(int i=0; i<npc_count; i++) {

		p = map_to_screen(npcs[i]->pos.x, npcs[i]->pos.y, cam.x, cam.y);
	
		r.w = npcs[i]->render_size.x;
		r.h = npcs[i]->render_size.y;
		r.x = p.x - npcs[i]->render_offset.x;
		r.y = p.y - npcs[i]->render_offset.y;
		
		if (isWithin(r, mouse)) {
		
			// adjust dest.y so that the tooltip floats above the item
			p.y -= tooltip_margin;
			
			td.num_lines = 1;
			td.colors[0] = FONT_WHITE;
			td.lines[0] = npcs[i]->name;
			
			tip->render(td, p, STYLE_TOPLABEL);
		}
	}
}

NPCManager::~NPCManager() {
	for (int i=0; i<npc_count; i++) {
		delete npcs[i];
	}

}
