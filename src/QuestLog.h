/**
 * class QuestLog
 *
 * Helper text to remind the player of active quests
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef QUEST_LOG_H
#define QUEST_LOG_H

#include <fstream>
#include <string>
#include "Utils.h"
#include "CampaignManager.h"
#include "MenuLog.h"

const int MAX_QUESTS = 1024;
const int MAX_QUEST_EVENTS = 4;

class QuestLog {
private:
	CampaignManager *camp;
	MenuLog *log;
	
	Event_Component quests[MAX_QUESTS][MAX_QUEST_EVENTS];
	int quest_count;
	
public:
	QuestLog(CampaignManager *_camp, MenuLog *_log);
	void loadAll();
	void load(string filename);
	void logic();
	void createQuestList();
};

#endif
