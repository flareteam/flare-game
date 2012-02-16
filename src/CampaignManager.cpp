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
 * class CampaignManager
 *
 * Contains data for story mode
 */

#include "CampaignManager.h"
#include "SharedResources.h"

#include <sstream>

using namespace std;


CampaignManager::CampaignManager() {

	drop_stack.item = 0;
	drop_stack.quantity = 0;

	items = NULL;
	carried_items = NULL;
	currency = NULL;
	xp = NULL;
	
	log_msg = "";
	quest_update = true;
	
	clearAll();

}

void CampaignManager::clearAll() {
	// clear campaign data
	for (int i=0; i<MAX_STATUS; i++) {
		status[i] = "";
	}
	status_count = 0;
}

/**
 * Take the savefile campaign= and convert to status array
 */
void CampaignManager::setAll(std::string s) {
	string str = s + ',';
	string token;
	while (str != "" && status_count < MAX_STATUS) {
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
	for (int i=0; i<status_count; i++) {
		ss << status[i];
		if (i < status_count-1) ss << ',';
	}
	return ss.str();
}

bool CampaignManager::checkStatus(std::string s) {

	// avoid searching empty statuses
	if (s == "") return false;
	
	for (int i=0; i<status_count; i++) {
		if (status[i] == s) return true;
	}
	return false;
}

void CampaignManager::setStatus(std::string s) {

	// avoid adding empty statuses
	if (s == "") return;

	// hit upper limit for status
	// TODO: add a warning
	if (status_count >= MAX_STATUS) return;
	
	// if it's already set, don't add it again
	if (checkStatus(s)) return;
	
	status[status_count++] = s;
	quest_update = true;
}

void CampaignManager::unsetStatus(std::string s) {

	// avoid searching empty statuses
	if (s == "") return;

	for (int i=status_count-1; i>=0; i--) {
		if (status[i] == s) {
		
			// bubble existing statuses down
			for (int j=i; j<status_count-1; j++) {
				status[j] = status[j+1];
			}
			status_count--;
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

	if (carried_items->full()) {
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
	addMsg(msg->get("You receive %d gold.", amount));
	items->playCoinsSound();
}

void CampaignManager::rewardXP(int amount) {
	*xp += amount;
	addMsg(msg->get("You receive %d XP.", amount));
}	

void CampaignManager::addMsg(const string& new_msg) {
	if (log_msg != "") log_msg += " ";
	log_msg += new_msg;
}

CampaignManager::~CampaignManager() {
}
