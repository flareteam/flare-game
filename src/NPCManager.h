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


#pragma once
#ifndef NPC_MANAGER_H
#define NPC_MANAGER_H

#include "WidgetTooltip.h"

#include <SDL.h>

#include <vector>

class ItemManager;
class LootManager;
class MapRenderer;
class StatBlock;
class NPC;

class NPCManager {
private:
	MapRenderer *map;
	WidgetTooltip *tip;
	LootManager *loot;
	ItemManager *items;
	StatBlock *stats;
	TooltipData tip_buf;
	int tooltip_margin;

public:
	NPCManager(MapRenderer *_map, LootManager *_loot, ItemManager *_items, StatBlock *stats);
	NPCManager(const NPCManager &copy); // not implemented
	~NPCManager();

	std::vector<NPC*> npcs;
	void handleNewMap();
	void logic();
	void addRenders(std::vector<Renderable> &r);
	int getID(std::string npcName);
	int checkNPCClick(Point mouse, Point cam);
	int getNearestNPC(Point pos);
	void renderTooltips(Point cam, Point mouse);
};

#endif
