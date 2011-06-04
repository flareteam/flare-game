/**
 * class QuestLog
 *
 * Helper text to remind the player of active quests
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "QuestLog.h"

QuestLog::QuestLog(CampaignManager *_camp, MenuLog *_log) {
	camp = _camp;
	log = _log;
	
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
	
	infile.open("quests/index.txt", ios::in);
	
	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);
			if (line.length() > 0) {
			
				// each line contains a quest file name
				load(line);
			}
		}
	}
	infile.close();
}

/**
 * Load the quests in the specific quest file
 */
void QuestLog::load(string filename) {

	ifstream infile;
	string line;
	string key;
	string val;
	string starts_with;
	string section = "";
	int event_count = 0;
	
	infile.open(("quests/" + filename).c_str(), ios::in);

	if (infile.is_open()) {
		while (!infile.eof()) {
			line = getLine(infile);

			if (line.length() > 0) {
				starts_with = line.at(0);
				
				if (starts_with == "#") {
					// skip comments
				}
				else if (starts_with == "[") {
					section = parse_section_title(line);
					if (section == "quest") {
						quest_count++;
						event_count = 0;
					}
				}
				else { // this is data.  treatment depends on key
					parse_key_pair(line, key, val);          
					key = trim(key, ' ');
					val = trim(val, ' ');
					
					quests[quest_count-1][event_count].type = key;
					quests[quest_count-1][event_count].s = val;
					event_count++;
					
					// requires_status=s
					// requires_not=s
					// quest_text=s			
				}
			}
		}
	}
	infile.close();	
}

void QuestLog::logic() {
	if (camp->quest_update) {
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
				break;
			}
			else if (quests[i][j].type == "") {
				break;
			}
		}
	}
	
}
