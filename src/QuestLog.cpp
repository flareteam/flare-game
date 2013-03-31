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
 * class QuestLog
 *
 * Helper text to remind the player of active quests
 */

#include "QuestLog.h"
#include "CampaignManager.h"
#include "Menu.h"
#include "MenuLog.h"
#include "SharedResources.h"
#include "Settings.h"
#include "FileParser.h"
#include "UtilsFileSystem.h"
#include "UtilsParsing.h"

#include <fstream>

using namespace std;


QuestLog::QuestLog(CampaignManager *_camp, MenuLog *_log) {
	camp = _camp;
	log = _log;

	newQuestNotification = false;
	resetQuestNotification = false;
	loadAll();
}

QuestLog::~QuestLog()
{
}

/**
 * Load each [mod]/quests/index.txt file
 */
void QuestLog::loadAll() {
	string test_path;

	// load each items.txt file. Individual item IDs can be overwritten with mods.
	for (unsigned int i = 0; i < mods->mod_list.size(); i++) {

		test_path = PATH_DATA + "mods/" + mods->mod_list[i] + "/quests/index.txt";

		if (fileExists(test_path)) {
			this->loadIndex(test_path);
		}
	}

}

/**
 * Load all the quest files from the given index
 * It simply contains a list of quest files
 * Generally each quest arc has its own file
 *
 * @param filename The full path and filename to the [mod]/quests/index.txt file
 */
void QuestLog::loadIndex(const std::string& filename) {
	ifstream infile;
	string line;

	infile.open(filename.c_str(), ios::in);

	if (infile.is_open()) {
		while (infile.good()) {
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
 * Load the quests in the specific quest file.
 * Searches for the last-defined such file in all mods
 *
 * @param filename The quest file name and extension, no path
 */
void QuestLog::load(const std::string& filename) {
	FileParser infile;
	if (!infile.open(mods->locate("quests/" + filename)))
		return;

	while (infile.next()) {
		if (infile.new_section) {
			if (infile.section == "quest")
				quests.push_back(vector<Event_Component>());
		}
		Event_Component ev;
		ev.type = infile.key;
		ev.s = msg->get(infile.val);
		quests.back().push_back(ev);
	}
	infile.close();
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

	for (unsigned int i=0; i<quests.size(); i++) {
		for (unsigned int j=0; j<quests[i].size(); j++) {

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
