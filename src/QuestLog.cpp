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

#include "QuestLog.h"
#include <fstream>
#include "FileParser.h"

QuestLog::QuestLog(CampaignManager *_camp, MenuLog *_log) {
	camp = _camp;
	log = _log;
	
    newQuestNotification = false;
    resetQuestNotification = false;
	quest_count = 0;
	loadAll();
}

/**
 * Load the quest index file
 * It simply contains a list of quest files
 * Generally each quest arch has its own file
 */
void QuestLog::loadAll() {
	ifstream infile;
	string line;
	
	infile.open((PATH_DATA + "quests/index.txt").c_str(), ios::in);
	
	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);
			if (line.length() > 0) {
			
				// each line contains a quest file name
				load(line);
			}
		}
		infile.close();
	}
}

/**
 * Load the quests in the specific quest file
 */
void QuestLog::load(string filename) {

	FileParser infile;
	int event_count = 0;
	
	if (infile.open(PATH_DATA + "quests/" + filename)) {
		while (infile.next()) {
			if (infile.new_section) {
				if (infile.section == "quest") {
					quest_count++;
					event_count = 0;
				}
			}
			
			quests[quest_count-1][event_count].type = infile.key;
			quests[quest_count-1][event_count].s = infile.val;
			event_count++;
			
			// requires_status=s
			// requires_not=s
			// quest_text=s
		}
		infile.close();
	}
}

void QuestLog::logic() {
	if (camp->quest_update) {
        resetQuestNotification = true;
		camp->quest_update = false;
		createQuestList();
	}
}

/**
 * All active quests are placed in the Quest tab of the Log Menu
 */
void QuestLog::createQuestList() {
	log->clear(LOG_TYPE_QUESTS);

	for (int i=0; i<quest_count; i++) {
		for (int j=0; j<MAX_QUEST_EVENTS; j++) {
			
			// check requirements
			// break (skip to next dialog node) if any requirement fails
			// if we reach an event that is not a requirement, succeed

			if (quests[i][j].type == "requires_status") {
				if (!camp->checkStatus(quests[i][j].s)) break;
			}
			else if (quests[i][j].type == "requires_not") {
				if (camp->checkStatus(quests[i][j].s)) break;
			}
			else if (quests[i][j].type == "quest_text") {
				log->add(quests[i][j].s, LOG_TYPE_QUESTS);
                newQuestNotification = true;
				break;
			}
			else if (quests[i][j].type == "") {
				break;
			}
		}
	}
	
}
