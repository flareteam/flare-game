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
 * class CampaignManager
 *
 * Contains data for story mode
 */

#include "CampaignManager.h"
#include "MenuItemStorage.h"
#include "Settings.h"
#include "StatBlock.h"
#include "SharedResources.h"
#include "UtilsParsing.h"

#include <sstream>

using namespace std;


CampaignManager::CampaignManager() {

	drop_stack.item = 0;
	drop_stack.quantity = 0;

	items = NULL;
	carried_items = NULL;
	currency = NULL;
	hero = NULL;

	log_msg = "";
	quest_update = true;

	clearAll();
}

void CampaignManager::clearAll() {
	// clear campaign data
	status.clear();
}

/**
 * Take the savefile campaign= and convert to status array
 */
void CampaignManager::setAll(std::string s) {
	string str = s + ',';
	string token;
	while (str != "") {
		token = eatFirstString(str, ',');
		if (token != "") this->setStatus(token);
	}
	quest_update = true;
}

/**
 * Convert status array to savefile campaign= (status csv)
 */
std::string CampaignManager::getAll() {
	stringstream ss;
	ss.str("");
	for (unsigned int i=0; i < status.size(); i++) {
		ss << status[i];
		if (i < status.size()-1) ss << ',';
	}
	return ss.str();
}

bool CampaignManager::checkStatus(std::string s) {

	// avoid searching empty statuses
	if (s == "") return false;

	for (unsigned int i=0; i < status.size(); i++) {
		if (status[i] == s) return true;
	}
	return false;
}

void CampaignManager::setStatus(std::string s) {

	// avoid adding empty statuses
	if (s == "") return;

	// if it's already set, don't add it again
	if (checkStatus(s)) return;

	status.push_back(s);
	quest_update = true;
}

void CampaignManager::unsetStatus(std::string s) {

	// avoid searching empty statuses
	if (s == "") return;

	vector<string>::iterator it;
	// see http://stackoverflow.com/a/223405
	for (it = status.end(); it != status.begin();) {
		--it;
		if ((*it) == s) {
			status.erase(it);
			quest_update = true;
			return;
		}
	}
}

bool CampaignManager::checkItem(int item_id) {
	return carried_items->contain(item_id);
}

void CampaignManager::removeItem(int item_id) {
	carried_items->remove(item_id);
}

void CampaignManager::rewardItem(ItemStack istack) {

	if (carried_items->full(istack.item)) {
		drop_stack.item = istack.item;
		drop_stack.quantity = istack.quantity;
	}
	else {
		carried_items->add(istack);

		if (istack.quantity <= 1)
			addMsg(msg->get("You receive %s.", items->items[istack.item].name));
		if (istack.quantity > 1)
			addMsg(msg->get("You receive %s x%d.", istack.quantity, items->items[istack.item].name));

		items->playSound(istack.item);
	}
}

void CampaignManager::rewardCurrency(int amount) {
	*currency += amount;
	addMsg(msg->get("You receive %d %s.", amount, CURRENCY));
	items->playCoinsSound();
}

void CampaignManager::rewardXP(int amount, bool show_message) {
	hero->xp += amount;
	hero->refresh_stats = true;
	if (show_message) addMsg(msg->get("You receive %d XP.", amount));
}

void CampaignManager::restoreHPMP(std::string s) {
	if (s == "hp") {
		hero->hp = hero->maxhp;
		addMsg(msg->get("HP restored."));
	}
	else if (s == "mp") {
		hero->mp = hero->maxmp;
		addMsg(msg->get("MP restored."));
	}
	else if (s == "hpmp") {
		hero->hp = hero->maxhp;
		hero->mp = hero->maxmp;
		addMsg(msg->get("HP and MP restored."));
	}
	else if (s == "status") {
		hero->clearNegativeEffects();
		addMsg(msg->get("Negative effects removed."));
	}
	else if (s == "all") {
		hero->hp = hero->maxhp;
		hero->mp = hero->maxmp;
		hero->clearNegativeEffects();
		addMsg(msg->get("HP and MP restored, negative effects removed"));
	}
}

void CampaignManager::addMsg(const string& new_msg) {
	if (log_msg != "") log_msg += " ";
	log_msg += new_msg;
}

CampaignManager::~CampaignManager() {
}
