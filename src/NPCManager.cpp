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
 * class NPCManager
 *
 * NPCs which are not combatative enemies are handled by this Manager.
 * Most commonly this involves vendor and conversation townspeople.
 */

#include "Animation.h"
#include "FileParser.h"
#include "NPCManager.h"
#include "NPC.h"
#include "SharedResources.h"
#include "MapRenderer.h"
#include "LootManager.h"
#include "StatBlock.h"
#include <limits>

using namespace std;

NPCManager::NPCManager(MapRenderer *_map, LootManager *_loot, ItemManager *_items, StatBlock *_stats)
	: map(_map)
	, tip(new WidgetTooltip())
	, loot(_loot)
	, items(_items)
	, stats(_stats)
	, tip_buf()
 {
	FileParser infile;
	// load tooltip_margin from engine config file
	if (infile.open(mods->locate("engine/tooltips.txt").c_str())) {
		while (infile.next()) {
			if (infile.key == "npc_tooltip_margin") {
				tooltip_margin = atoi(infile.val.c_str());
			}
		}
		infile.close();
	}
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

		npc->stock.sort();
		npcs.push_back(npc);
	}

}

void NPCManager::logic() {
	for (unsigned i=0; i<npcs.size(); i++) {
		npcs[i]->logic();
	}
}

int NPCManager::getID(std::string npcName) {
	for (unsigned i=0; i<npcs.size(); i++) {
		if (npcs[i]->filename == npcName) return i;
	}
	return -1;
}

int NPCManager::checkNPCClick(Point mouse, Point cam) {
	Point p;
	SDL_Rect r;
	for (unsigned i=0; i<npcs.size(); i++) {

		p = map_to_screen(npcs[i]->pos.x, npcs[i]->pos.y, cam.x, cam.y);

		Renderable ren = npcs[i]->activeAnimation->getCurrentFrame(npcs[i]->direction);
		r.w = ren.src.w;
		r.h = ren.src.h;
		r.x = p.x - ren.offset.x;
		r.y = p.y - ren.offset.y;

		if (isWithin(r, mouse)) {
			return i;
		}
	}
	return -1;
}

int NPCManager::getNearestNPC(Point pos) {
	int nearest = -1;
	int best_distance = std::numeric_limits<int>::max();

	for (unsigned i=0; i<npcs.size(); i++) {
		int distance = (int)calcDist(pos, npcs[i]->pos);
		if (distance < best_distance) {
			best_distance = distance;
			nearest = i;
		}
	}

	return nearest;
}

/**
 * On mouseover, display NPC's name
 */
void NPCManager::renderTooltips(Point cam, Point mouse) {
	Point p;
	SDL_Rect r;

	for (unsigned i=0; i<npcs.size(); i++) {

		p = map_to_screen(npcs[i]->pos.x, npcs[i]->pos.y, cam.x, cam.y);

		Renderable ren = npcs[i]->activeAnimation->getCurrentFrame(npcs[i]->direction);
		r.w = ren.src.w;
		r.h = ren.src.h;
		r.x = p.x - ren.offset.x;
		r.y = p.y - ren.offset.y;

		if (isWithin(r, mouse) && TOOLTIP_CONTEXT != TOOLTIP_MENU) {

			// adjust dest.y so that the tooltip floats above the item
			p.y -= tooltip_margin;

			// use current tip or make a new one?
			if (!tip_buf.compareFirstLine(npcs[i]->name)) {
				tip_buf.clear();
				tip_buf.addText(npcs[i]->name);
			}

			tip->render(tip_buf, p, STYLE_TOPLABEL);
			TOOLTIP_CONTEXT = TOOLTIP_MAP;

			break; // display only one NPC tooltip at a time
		} else if (TOOLTIP_CONTEXT != TOOLTIP_MENU) {
			TOOLTIP_CONTEXT = TOOLTIP_NONE;
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
