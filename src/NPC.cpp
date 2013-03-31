/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012-2013 Henrik Andersson
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
 * class NPC
 */

#include "NPC.h"

#include "Animation.h"
#include "AnimationSet.h"
#include "AnimationManager.h"
#include "CampaignManager.h"
#include "FileParser.h"
#include "ItemManager.h"
#include "MapRenderer.h"
#include "SharedResources.h"
#include "UtilsParsing.h"

using namespace std;


ItemStorage stock;

std::vector<SoundManager::SoundID> vox_intro;
std::vector<SoundManager::SoundID> vox_quests;
std::vector<std::vector<Event_Component> > dialog;

NPC::NPC(MapRenderer *_map, ItemManager *_items)
	: Entity(_map)
	, items(_items)
	, name("")
	, gfx("")
	, pos()
	, level(1)
	, direction(0)
	, portrait(NULL)
	, talker(false)
	, vendor(false)
	, stock()
	, stock_count(0)
	, vox_intro()
	, vox_quests()
	, dialog()
{
	stock.init(NPC_VENDOR_MAX_STOCK, _items);
}

/**
 * NPCs are stored in simple config files
 *
 * @param npc_id Config file loaded at npcs/[npc_id].txt
 */
void NPC::load(const string& npc_id, int hero_level) {

	FileParser infile;
	ItemStack stack;

	string filename_portrait = "";

	if (infile.open(mods->locate("npcs/" + npc_id + ".txt"))) {
		while (infile.next()) {
			if (infile.section == "dialog") {
				if (infile.new_section) {
					dialog.push_back(vector<Event_Component>());
				}
				Event_Component e;
				e.type = infile.key;
				if (infile.key == "requires_status")
					e.s = infile.val;
				else if (infile.key == "requires_not")
					e.s = infile.val;
				else if (infile.key == "requires_level")
					e.x = toInt(infile.val);
				else if (infile.key == "requires_not_level")
					e.x = toInt(infile.val);
				else if (infile.key == "requires_item")
					e.x = toInt(infile.val);
				else if (infile.key == "him" || infile.key == "her")
					e.s = msg->get(infile.val);
				else if (infile.key == "you")
					e.s = msg->get(infile.val);
				else if (infile.key == "reward_item") {
					// id,count
					e.x = toInt(infile.nextValue());
					e.y = toInt(infile.val);
				}
				else if (infile.key == "reward_xp")
					e.x = toInt(infile.val);
				else if (infile.key == "restore")
					e.s = infile.val;
				else if (infile.key == "reward_currency")
					e.x = toInt(infile.val);
				else if (infile.key == "remove_item")
					e.x = toInt(infile.val);
				else if (infile.key == "set_status")
					e.s = infile.val;
				else if (infile.key == "unset_status")
					e.s = infile.val;
				else if (infile.key == "voice") {
					e.x = loadSound(infile.val, NPC_VOX_QUEST);
				}
				else if (infile.key == "topic") {
					e.s = msg->get(infile.val);
				}
				else if (infile.key == "group") {
					e.s = infile.val;
				}

				dialog.back().push_back(e);
			}
			else {
				filename = npc_id;
				if (infile.key == "name") {
					name = msg->get(infile.val);
				}
				else if (infile.key == "level") {
					if (infile.val == "hero")
						level = hero_level;
					else
						level = toInt(infile.val);
				}
				else if (infile.key == "gfx") {
					gfx = infile.val;
				}

				// handle talkers
				else if (infile.key == "talker") {
					if (infile.val == "true") talker = true;
				}
				else if (infile.key == "portrait") {
					filename_portrait = infile.val;
				}

				// handle vendors
				else if (infile.key == "vendor") {
					if (infile.val == "true") vendor = true;
				}
				else if (infile.key == "constant_stock") {
					stack.quantity = 1;
					while (infile.val != "") {
						stack.item = toInt(infile.nextValue());
						stock.add(stack);
					}
				}
				else if (infile.key == "status_stock") {
					if (map->camp->checkStatus(infile.nextValue())) {
						stack.quantity = 1;
						while (infile.val != "") {
							stack.item = toInt(infile.nextValue());
							stock.add(stack);
						}
					}
				}

				// handle vocals
				else if (infile.key == "vox_intro") {
					loadSound(infile.val, NPC_VOX_INTRO);
				}
			}
		}
		infile.close();
	}
	loadGraphics(filename_portrait);
}

void NPC::loadGraphics(const string& filename_portrait) {

	if (gfx != "") {
		std::string anim_name = "animations/npcs/" + gfx + ".txt";
		anim->increaseCount(anim_name);
		animationSet = anim->getAnimationSet(anim_name);
		activeAnimation = animationSet->getAnimation();
	}
	if (filename_portrait != "")
		portrait = loadGraphicSurface("images/portraits/" + filename_portrait + ".png", "Couldn't load NPC portrait", false, true);
}

/**
 * filename assumes the file is in soundfx/npcs/
 * type is a const int enum, see NPC.h
 * returns -1 if not loaded or error.
 * returns index in specific vector where to be found.
 */
int NPC::loadSound(const string& fname, int type) {

	SoundManager::SoundID a = snd->load("soundfx/npcs/" + fname, "NPC voice");

	if (!a)
		return -1;

	if (type == NPC_VOX_INTRO) {
		vox_intro.push_back(a);
		return vox_intro.size() - 1;
	}

	if (type == NPC_VOX_QUEST) {
		vox_quests.push_back(a);
		return vox_quests.size() - 1;
	}
	return -1;
}

void NPC::logic() {
	activeAnimation->advanceFrame();
}

/**
 * type is a const int enum, see NPC.h
 */
bool NPC::playSound(int type, int id) {
	if (type == NPC_VOX_INTRO) {
		int roll;
		if (vox_intro.empty()) return false;
		roll = rand() % vox_intro.size();
		snd->play(vox_intro[roll], "NPC_VOX");
		return true;
	}
	if (type == NPC_VOX_QUEST) {
		if (id < 0 || id >= (int)vox_quests.size()) return false;
		snd->play(vox_quests[id], "NPC_VOX");
		return true;
	}
	return false;
}

/**
 * get list of available dialogs with NPC
 */
void NPC::getDialogNodes(std::vector<int> &result) {
	result.clear();
	if (!talker)
		return;

	std::string group;
	typedef std::vector<int> Dialogs;
	typedef std::map<std::string, Dialogs > DialogGroups;
	DialogGroups groups;

	for (int i=dialog.size()-1; i>=0; i--) {
		bool is_available = true;
		bool is_grouped = false;
		for (unsigned int j=0; j<dialog[i].size(); j++) {

			if (dialog[i][j].type == "requires_status") {
				if (map->camp->checkStatus(dialog[i][j].s))
					continue;
				is_available = false;
				break;
			}
			else if (dialog[i][j].type == "requires_not") {
				if (!map->camp->checkStatus(dialog[i][j].s))
					continue;
				is_available = false;
				break;
			}
			else if (dialog[i][j].type == "requires_item") {
				if (map->camp->checkItem(dialog[i][j].x))
					continue;
				is_available = false;
				break;
			}
			else if (dialog[i][j].type == "requires_level") {
				if (!map->camp->hero->level < dialog[i][j].x)
					continue;
				is_available = false;
				break;
			}
			else if (dialog[i][j].type == "requires_not_level") {
				if (!map->camp->hero->level >= dialog[i][j].x)
					continue;
				is_available = false;
				break;
			}
			else if (dialog[i][j].type == "group") {
				is_grouped = true;
				group = dialog[i][j].s;
			}
		}

		if (is_available) {
			if (!is_grouped) {
				result.push_back(i);
			} else {
				DialogGroups::iterator it;
				it = groups.find(group);
				if (it == groups.end()) {
					groups.insert(DialogGroups::value_type(group, Dialogs()));
				}
				else
					it->second.push_back(i);

			}
		}
	}

	/* Iterate over dialoggroups and roll a dialog to add to result */
	DialogGroups::iterator it;
	it = groups.begin();
	if (it == groups.end())
		return;

	while (it != groups.end()) {
		/* roll a dialog for this group and add to result */
		int di = it->second[rand() % it->second.size()];
		result.push_back(di);
		++it;
	}
}

std::string NPC::getDialogTopic(unsigned int dialog_node) {
	if (!talker)
		return "";

	for (unsigned int j=0; j<dialog[dialog_node].size(); j++) {
		if (dialog[dialog_node][j].type == "topic")
			return dialog[dialog_node][j].s;
	}

	return "";
}

/**
 * Process the current dialog
 *
 * Return false if the dialog has ended
 */
bool NPC::processDialog(unsigned int dialog_node, unsigned int &event_cursor) {

	while (event_cursor < dialog[dialog_node].size()) {

		// we've already determined requirements are met, so skip these
		if (dialog[dialog_node][event_cursor].type == "requires_status") {
			// continue to next event component
		}
		else if (dialog[dialog_node][event_cursor].type == "requires_not") {
			// continue to next event component
		}
		else if (dialog[dialog_node][event_cursor].type == "requires_item") {
			// continue to next event component
		}
		else if (dialog[dialog_node][event_cursor].type == "set_status") {
			map->camp->setStatus(dialog[dialog_node][event_cursor].s);
		}
		else if (dialog[dialog_node][event_cursor].type == "unset_status") {
			map->camp->unsetStatus(dialog[dialog_node][event_cursor].s);
		}
		else if (dialog[dialog_node][event_cursor].type == "him") {
			return true;
		}
		else if (dialog[dialog_node][event_cursor].type == "her") {
			return true;
		}
		else if (dialog[dialog_node][event_cursor].type == "you") {
			return true;
		}
		else if (dialog[dialog_node][event_cursor].type == "reward_xp") {
			map->camp->rewardXP(dialog[dialog_node][event_cursor].x, true);
		}
		else if (dialog[dialog_node][event_cursor].type == "restore") {
			map->camp->restoreHPMP(dialog[dialog_node][event_cursor].s);
		}
		else if (dialog[dialog_node][event_cursor].type == "reward_currency") {
			map->camp->rewardCurrency(dialog[dialog_node][event_cursor].x);
		}
		else if (dialog[dialog_node][event_cursor].type == "reward_item") {
			ItemStack istack;
			istack.item = dialog[dialog_node][event_cursor].x;
			istack.quantity = dialog[dialog_node][event_cursor].y;
			map->camp->rewardItem(istack);
		}
		else if (dialog[dialog_node][event_cursor].type == "remove_item") {
			map->camp->removeItem(dialog[dialog_node][event_cursor].x);
		}
		else if (dialog[dialog_node][event_cursor].type == "voice") {
			playSound(NPC_VOX_QUEST, dialog[dialog_node][event_cursor].x);
		}
		else if (dialog[dialog_node][event_cursor].type == "") {
			// conversation ends
			return false;
		}

		event_cursor++;
	}
	return false;
}

Renderable NPC::getRender() {
	Renderable r = activeAnimation->getCurrentFrame(direction);
	r.map_pos.x = pos.x;
	r.map_pos.y = pos.y;

	return r;
}


NPC::~NPC() {
	if (gfx != "") {
		const string anim_name = "animations/npcs/" + gfx + ".txt";
		anim->decreaseCount(anim_name);
	}

	if (portrait != NULL) SDL_FreeSurface(portrait);
	while (!vox_intro.empty()) {
		snd->unload(vox_intro.back());
		vox_intro.pop_back();
	}
	while (!vox_quests.empty()) {
		snd->unload(vox_quests.back());
		vox_quests.pop_back();
	}
}
