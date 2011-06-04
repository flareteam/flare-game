/**
 * class CampaignManager
 *
 * Contains data for story mode
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef CAMPAIGN_MANAGER_H
#define CAMPAIGN_MANAGER_H

#include <string>
#include <sstream>
#include "UtilsParsing.h"
#include "MenuItemStorage.h"
#include "ItemDatabase.h"

const int MAX_STATUS = 1024;

class CampaignManager {
private:

public:
	CampaignManager();
	~CampaignManager();
	
	void clearAll();
	void setAll(std::string s);
	std::string getAll();
	bool checkStatus(std::string s);
	void setStatus(std::string s);
	void unsetStatus(std::string s);
	bool checkItem(int item_id);
	void removeItem(int item_id);
	void rewardItem(ItemStack istack);
	void rewardCurrency(int amount);
	void rewardXP(int amount);
	void addMsg(string msg);
	
	string status[MAX_STATUS];
	int status_count;
	string log_msg;
	ItemStack drop_stack;
	
	// pointers to various info that can be changed
	ItemDatabase *items;
	MenuItemStorage *carried_items;
	int *currency;
	int *xp;
	
	bool quest_update;
};


#endif
