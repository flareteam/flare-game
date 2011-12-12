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
 * class QuestLog
 *
 * Helper text to remind the player of active quests
 */

#ifndef QUEST_LOG_H
#define QUEST_LOG_H

#include <string>
#include "Utils.h"
#include "CampaignManager.h"
#include "MenuLog.h"

const int MAX_QUESTS = 1024;
const int MAX_QUEST_EVENTS = 8;

class QuestLog {
private:
	CampaignManager *camp;
	MenuLog *log;
	
	Event_Component quests[MAX_QUESTS][MAX_QUEST_EVENTS];
	int quest_count;
	
public:
	QuestLog(CampaignManager *_camp, MenuLog *_log);
	void loadAll();
	void loadIndex(const std::string& filename);
	void load(const std::string& filename);
	void logic();
	void createQuestList();
    bool newQuestNotification;
    bool resetQuestNotification;
};

#endif
