/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2013 Henrik Andersson

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
 * class MenuCharacter
 */

#include "FileParser.h"
#include "Menu.h"
#include "MenuCharacter.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "UtilsParsing.h"
#include "WidgetButton.h"
#include "WidgetListBox.h"


using namespace std;


MenuCharacter::MenuCharacter(StatBlock *_stats) {
	stats = _stats;

	skill_points = 0;

	visible = false;
	newPowerNotification = false;

	for (int i=0; i<CSTAT_COUNT; i++) {
		cstat[i].label = new WidgetLabel();
		cstat[i].value = new WidgetLabel();
		cstat[i].hover.x = cstat[i].hover.y = 0;
		cstat[i].hover.w = cstat[i].hover.h = 0;
		cstat[i].visible = true;
	}
	for (int i=0; i<STATLIST_COUNT; i++) {
		show_stat[i] = true;
	}
	statlist_rows = 10;
	statlist_scrollbar_offset = 0;

	closeButton = new WidgetButton("images/menus/buttons/button_x.png");

	// Upgrade buttons
	for (int i=0; i<4; i++) {
		upgradeButton[i] = new WidgetButton("images/menus/buttons/upgrade.png");
		upgradeButton[i]->enabled = false;
		show_upgrade[i] = true;
	}
	physical_up = false;
	mental_up = false;
	offense_up = false;
	defense_up = false;

	// menu title
	labelCharacter = new WidgetLabel();

	// unspent points
	labelUnspent = new WidgetLabel();

	// Load config settings
	FileParser infile;
	if(infile.open(mods->locate("menus/character.txt"))) {
		while(infile.next()) {
			infile.val = infile.val + ',';

			if(infile.key == "close") {
				close_pos.x = eatFirstInt(infile.val,',');
				close_pos.y = eatFirstInt(infile.val,',');
			} else if(infile.key == "caption") {
				title = eatLabelInfo(infile.val);
			} else if(infile.key == "upgrade_physical") {
				upgrade_pos[0].x = eatFirstInt(infile.val,',');
				upgrade_pos[0].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_mental") {
				upgrade_pos[1].x = eatFirstInt(infile.val,',');
				upgrade_pos[1].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_offense") {
				upgrade_pos[2].x = eatFirstInt(infile.val,',');
				upgrade_pos[2].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "upgrade_defense") {
				upgrade_pos[3].x = eatFirstInt(infile.val,',');
				upgrade_pos[3].y = eatFirstInt(infile.val,',');
			} else if(infile.key == "statlist") {
				statlist_pos.x = eatFirstInt(infile.val,',');
				statlist_pos.y = eatFirstInt(infile.val,',');
			} else if (infile.key == "statlist_rows") {
				statlist_rows = eatFirstInt(infile.val,',');
			} else if (infile.key == "statlist_scrollbar_offset") {
				statlist_scrollbar_offset = eatFirstInt(infile.val,',');
			} else if(infile.key == "label_name") {
				label_pos[0] = eatLabelInfo(infile.val);
				cstat[CSTAT_NAME].visible = !label_pos[0].hidden;
			} else if(infile.key == "label_level") {
				label_pos[1] = eatLabelInfo(infile.val);
				cstat[CSTAT_LEVEL].visible = !label_pos[1].hidden;
			} else if(infile.key == "label_physical") {
				label_pos[2] = eatLabelInfo(infile.val);
				cstat[CSTAT_PHYSICAL].visible = !label_pos[2].hidden;
			} else if(infile.key == "label_mental") {
				label_pos[3] = eatLabelInfo(infile.val);
				cstat[CSTAT_MENTAL].visible = !label_pos[3].hidden;
			} else if(infile.key == "label_offense") {
				label_pos[4] = eatLabelInfo(infile.val);
				cstat[CSTAT_OFFENSE].visible = !label_pos[4].hidden;
			} else if(infile.key == "label_defense") {
				label_pos[5] = eatLabelInfo(infile.val);
				cstat[CSTAT_DEFENSE].visible = !label_pos[5].hidden;
			} else if(infile.key == "name") {
				value_pos[0].x = eatFirstInt(infile.val,',');
				value_pos[0].y = eatFirstInt(infile.val,',');
				value_pos[0].w = eatFirstInt(infile.val,',');
				value_pos[0].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "level") {
				value_pos[1].x = eatFirstInt(infile.val,',');
				value_pos[1].y = eatFirstInt(infile.val,',');
				value_pos[1].w = eatFirstInt(infile.val,',');
				value_pos[1].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "physical") {
				value_pos[2].x = eatFirstInt(infile.val,',');
				value_pos[2].y = eatFirstInt(infile.val,',');
				value_pos[2].w = eatFirstInt(infile.val,',');
				value_pos[2].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "mental") {
				value_pos[3].x = eatFirstInt(infile.val,',');
				value_pos[3].y = eatFirstInt(infile.val,',');
				value_pos[3].w = eatFirstInt(infile.val,',');
				value_pos[3].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "offense") {
				value_pos[4].x = eatFirstInt(infile.val,',');
				value_pos[4].y = eatFirstInt(infile.val,',');
				value_pos[4].w = eatFirstInt(infile.val,',');
				value_pos[4].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "defense") {
				value_pos[5].x = eatFirstInt(infile.val,',');
				value_pos[5].y = eatFirstInt(infile.val,',');
				value_pos[5].w = eatFirstInt(infile.val,',');
				value_pos[5].h = eatFirstInt(infile.val,',');
			} else if(infile.key == "unspent") {
				unspent_pos = eatLabelInfo(infile.val);
			} else if (infile.key == "show_upgrade_physical"){
				if (eatFirstInt(infile.val,',') == 0) show_upgrade[0] = false;
			} else if (infile.key == "show_upgrade_mental"){
				if (eatFirstInt(infile.val,',') == 0) show_upgrade[1] = false;
			} else if (infile.key == "show_upgrade_offense"){
				if (eatFirstInt(infile.val,',') == 0) show_upgrade[2] = false;
			} else if (infile.key == "show_upgrade_defense"){
				if (eatFirstInt(infile.val,',') == 0) show_upgrade[3] = false;
			} else if (infile.key == "show_maxhp"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[0] = false;
			} else if (infile.key == "show_hpregen"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[1] = false;
			} else if (infile.key == "show_maxmp"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[2] = false;
			} else if (infile.key == "show_mpregen"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[3] = false;
			} else if (infile.key == "show_accuracy"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[4] = false;
			} else if (infile.key == "show_avoidance"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[5] = false;
			} else if (infile.key == "show_melee"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[6] = false;
			} else if (infile.key == "show_ranged"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[7] = false;
			} else if (infile.key == "show_mental"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[8] = false;
			} else if (infile.key == "show_crit"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[9] = false;
			} else if (infile.key == "show_absorb"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[10] = false;
			} else if (infile.key == "show_poise"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[11] = false;
			} else if (infile.key == "show_bonus_xp"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[12] = false;
			} else if (infile.key == "show_bonus_currency"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[13] = false;
			} else if (infile.key == "show_bonus_itemfind"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[14] = false;
			} else if (infile.key == "show_bonus_stealth"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[15] = false;
			} else if (infile.key == "show_resists"){
				if (eatFirstInt(infile.val,',') == 0) show_stat[16] = false;
			}
		}
		infile.close();
	}

	// stat list
	statList = new WidgetListBox(STATLIST_COUNT-1+stats->vulnerable.size(), statlist_rows, "images/menus/buttons/listbox_char.png");
	statList->can_select = false;
	statList->scrollbar_offset = statlist_scrollbar_offset;

	background = loadGraphicSurface("images/menus/character.png");
}

void MenuCharacter::update() {

	// close button
	closeButton->pos.x = window_area.x + close_pos.x;
	closeButton->pos.y = window_area.y + close_pos.y;

	// menu title
	labelCharacter->set(window_area.x+title.x, window_area.y+title.y, title.justify, title.valign, msg->get("Character"), font->getColor("menu_normal"), title.font_style);

	// upgrade buttons
	for (int i=0; i<4; i++) {
		upgradeButton[i]->pos.x = window_area.x+upgrade_pos[i].x;
		upgradeButton[i]->pos.y = window_area.y+upgrade_pos[i].y;
	}

	// stat list
	statList->pos.x = window_area.x+statlist_pos.x;
	statList->pos.y = window_area.y+statlist_pos.y;

	// setup static labels
	cstat[CSTAT_NAME].label->set(window_area.x+label_pos[0].x, window_area.y+label_pos[0].y, label_pos[0].justify, label_pos[0].valign, msg->get("Name"), font->getColor("menu_normal"), label_pos[0].font_style);
	cstat[CSTAT_LEVEL].label->set(window_area.x+label_pos[1].x, window_area.y+label_pos[1].y, label_pos[1].justify, label_pos[1].valign, msg->get("Level"), font->getColor("menu_normal"), label_pos[1].font_style);
	cstat[CSTAT_PHYSICAL].label->set(window_area.x+label_pos[2].x, window_area.y+label_pos[2].y, label_pos[2].justify, label_pos[2].valign, msg->get("Physical"), font->getColor("menu_normal"), label_pos[2].font_style);
	cstat[CSTAT_MENTAL].label->set(window_area.x+label_pos[3].x, window_area.y+label_pos[3].y, label_pos[3].justify, label_pos[3].valign, msg->get("Mental"), font->getColor("menu_normal"), label_pos[3].font_style);
	cstat[CSTAT_OFFENSE].label->set(window_area.x+label_pos[4].x, window_area.y+label_pos[4].y, label_pos[4].justify, label_pos[4].valign, msg->get("Offense"), font->getColor("menu_normal"), label_pos[4].font_style);
	cstat[CSTAT_DEFENSE].label->set(window_area.x+label_pos[5].x, window_area.y+label_pos[5].y, label_pos[5].justify, label_pos[5].valign, msg->get("Defense"), font->getColor("menu_normal"), label_pos[5].font_style);

	// setup hotspot locations
	cstat[CSTAT_NAME].setHover(window_area.x+value_pos[0].x, window_area.y+value_pos[0].y, value_pos[0].w, value_pos[0].h);
	cstat[CSTAT_LEVEL].setHover(window_area.x+value_pos[1].x, window_area.y+value_pos[1].y, value_pos[1].w, value_pos[1].h);
	cstat[CSTAT_PHYSICAL].setHover(window_area.x+value_pos[2].x, window_area.y+value_pos[2].y, value_pos[2].w, value_pos[2].h);
	cstat[CSTAT_MENTAL].setHover(window_area.x+value_pos[3].x, window_area.y+value_pos[3].y, value_pos[3].w, value_pos[3].h);
	cstat[CSTAT_OFFENSE].setHover(window_area.x+value_pos[4].x, window_area.y+value_pos[4].y, value_pos[4].w, value_pos[4].h);
	cstat[CSTAT_DEFENSE].setHover(window_area.x+value_pos[5].x, window_area.y+value_pos[5].y, value_pos[5].w, value_pos[5].h);
}

/**
 * Rebuild all stat values and tooltip info
 */
void MenuCharacter::refreshStats() {

	stats->refresh_stats = false;

	stringstream ss;

	// update stat text
	cstat[CSTAT_NAME].value->set(window_area.x+value_pos[0].x+4, window_area.y+value_pos[0].y+value_pos[0].h/2, JUSTIFY_LEFT, VALIGN_CENTER, stats->name, font->getColor("menu_normal"));

	ss.str("");
	ss << stats->level;
	cstat[CSTAT_LEVEL].value->set(window_area.x+value_pos[1].x+value_pos[1].w/2, window_area.y+value_pos[1].y+value_pos[1].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), font->getColor("menu_normal"));

	ss.str("");
	ss << stats->get_physical();
	cstat[CSTAT_PHYSICAL].value->set(window_area.x+value_pos[2].x+value_pos[2].w/2, window_area.y+value_pos[2].y+value_pos[2].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->physical_additional));

	ss.str("");
	ss << stats->get_mental();
	cstat[CSTAT_MENTAL].value->set(window_area.x+value_pos[3].x+value_pos[3].w/2, window_area.y+value_pos[3].y+value_pos[3].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->mental_additional));

	ss.str("");
	ss << stats->get_offense();
	cstat[CSTAT_OFFENSE].value->set(window_area.x+value_pos[4].x+value_pos[4].w/2, window_area.y+value_pos[4].y+value_pos[4].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->offense_additional));

	ss.str("");
	ss << stats->get_defense();
	cstat[CSTAT_DEFENSE].value->set(window_area.x+value_pos[5].x+value_pos[5].w/2, window_area.y+value_pos[5].y+value_pos[5].h/2, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->defense_additional));

	ss.str("");
	if (skill_points > 0) ss << skill_points << " " << msg->get("points remaining");
	else ss.str("");
	labelUnspent->set(window_area.x+unspent_pos.x, window_area.y+unspent_pos.y, unspent_pos.justify, unspent_pos.valign, ss.str(), font->getColor("menu_bonus"), unspent_pos.font_style);

	// scrolling stat list

	int visible_stats = 0;

	if (show_stat[0]) {
		ss.str("");
		ss << msg->get("Max HP:") << " " << stats->maxhp;
		statList->set(visible_stats++, ss.str(),msg->get("Each point of Physical grants +%d HP. Each level grants +%d HP", stats->hp_per_physical, stats->hp_per_level));
	}

	if (show_stat[1]) {
		ss.str("");
		ss << msg->get("HP Regen:") << " " << stats->hp_per_minute;
		statList->set(visible_stats++, ss.str(),msg->get("Ticks of HP regen per minute. Each point of Physical grants +%d HP regen. Each level grants +%d HP regen",stats->hp_regen_per_physical, stats->hp_regen_per_level));
	}

	if (show_stat[2]) {
		ss.str("");
		ss << msg->get("Max MP:") << " " << stats->maxmp;
		statList->set(visible_stats++, ss.str(),msg->get("Each point of Mental grants +%d MP. Each level grants +%d MP", stats->mp_per_mental, stats->mp_per_level));
	}

	if (show_stat[3]) {
		ss.str("");
		ss << msg->get("MP Regen:") << " " << stats->mp_per_minute;
		statList->set(visible_stats++, ss.str(),msg->get("Ticks of MP regen per minute. Each point of Mental grants +%d MP regen. Each level grants +%d MP regen", stats->mp_regen_per_mental, stats->mp_regen_per_level));
	}

	if (show_stat[4]) {
		ss.str("");
		ss << msg->get("Accuracy:") << " " << stats->accuracy << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Each point of Offense grants +%d accuracy. Each level grants +%d accuracy", stats->accuracy_per_offense, stats->accuracy_per_level));
	}

	if (show_stat[5]) {
		ss.str("");
		ss << msg->get("Avoidance:") << " " << stats->avoidance << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Each point of Defense grants +%d avoidance. Each level grants +%d avoidance", stats->avoidance_per_defense, stats->avoidance_per_level));
	}

	if (show_stat[6]) {
		ss.str("");
		ss << msg->get("Melee Damage:") << " ";
		if (stats->dmg_melee_min == stats->dmg_melee_max)
			ss << stats->dmg_melee_min;
		else
			ss << stats->dmg_melee_min << "-" << stats->dmg_melee_max;
		statList->set(visible_stats++, ss.str(),"");
	}

	if (show_stat[7]) {
		ss.str("");
		ss << msg->get("Ranged Damage:") << " ";
		if (stats->dmg_ranged_min == stats->dmg_ranged_max)
			ss << stats->dmg_ranged_min;
		else
			ss << stats->dmg_ranged_min << "-" << stats->dmg_ranged_max;
		statList->set(visible_stats++, ss.str(),"");
	}

	if (show_stat[8]) {
		ss.str("");
		ss << msg->get("Mental Damage:") << " ";
		if (stats->dmg_ment_min == stats->dmg_ment_max)
			ss << stats->dmg_ment_min;
		else
			ss << stats->dmg_ment_min << "-" << stats->dmg_ment_max;
		statList->set(visible_stats++, ss.str(),"");
	}

	if (show_stat[9]) {
		ss.str("");
		ss << msg->get("Crit:") << " " << stats->crit << "%";
		statList->set(visible_stats++, ss.str(),"");
	}

	if (show_stat[10]) {
		ss.str("");
		ss << msg->get("Absorb:") << " ";
		if (stats->absorb_min == stats->absorb_max)
			ss << stats->absorb_min;
		else
		ss << stats->absorb_min << "-" << stats->absorb_max;
		statList->set(visible_stats++, ss.str(),"");
	}

	if (show_stat[11]) {
		ss.str("");
		ss << msg->get("Poise: ") << stats->poise << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Reduces your chance of stumbling when hit"));
	}

	if (show_stat[12]) {
		ss.str("");
		ss << msg->get("Bonus XP: ") << stats->effects.bonus_xp << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Increases the XP gained per kill"));
	}

	if (show_stat[13]) {
		ss.str("");
		ss << msg->get("Bonus") << " " << CURRENCY << ": " << stats->effects.bonus_currency << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Increases the %s found per drop",CURRENCY));
	}

	if (show_stat[14]) {
		ss.str("");
		ss << msg->get("Bonus Item Find: ") << stats->effects.bonus_item_find << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Increases the chance that an enemy will drop an item when killed"));
	}

	if (show_stat[15]) {
		ss.str("");
		ss << msg->get("Stealth: ") << stats->effects.bonus_stealth << "%";
		statList->set(visible_stats++, ss.str(),msg->get("Increases your ability to move undetected"));
	}

	if (show_stat[16]) {
		for (unsigned int j=0; j<stats->vulnerable.size(); j++) {
			ss.str("");
			ss << msg->get(ELEMENTS[j].resist) << ": " << (100 - stats->vulnerable[j]) << "%";
			statList->set(visible_stats++, ss.str(),"");
		}
	}

	statList->refresh();

	// update tool tips
	cstat[CSTAT_NAME].tip.clear();
	cstat[CSTAT_NAME].tip.addText(msg->get(stats->character_class));

	cstat[CSTAT_LEVEL].tip.clear();
	cstat[CSTAT_LEVEL].tip.addText(msg->get("XP: %d", stats->xp));
	if (stats->level < MAX_CHARACTER_LEVEL) {
		cstat[CSTAT_LEVEL].tip.addText(msg->get("Next: %d", stats->xp_table[stats->level]));
	}

	cstat[CSTAT_PHYSICAL].tip.clear();
	cstat[CSTAT_PHYSICAL].tip.addText(msg->get("Physical (P) increases melee weapon proficiency and total HP."));
	cstat[CSTAT_PHYSICAL].tip.addText(msg->get("base (%d), bonus (%d)", stats->physical_character, stats->physical_additional));

	cstat[CSTAT_MENTAL].tip.clear();
	cstat[CSTAT_MENTAL].tip.addText(msg->get("Mental (M) increases mental weapon proficiency and total MP."));
	cstat[CSTAT_MENTAL].tip.addText(msg->get("base (%d), bonus (%d)", stats->mental_character, stats->mental_additional));

	cstat[CSTAT_OFFENSE].tip.clear();
	cstat[CSTAT_OFFENSE].tip.addText(msg->get("Offense (O) increases ranged weapon proficiency and accuracy."));
	cstat[CSTAT_OFFENSE].tip.addText(msg->get("base (%d), bonus (%d)", stats->offense_character, stats->offense_additional));

	cstat[CSTAT_DEFENSE].tip.clear();
	cstat[CSTAT_DEFENSE].tip.addText(msg->get("Defense (D) increases armor proficiency and avoidance."));
	cstat[CSTAT_DEFENSE].tip.addText(msg->get("base (%d), bonus (%d)", stats->defense_character, stats->defense_additional));
}


/**
 * Color-coding for positive/negative/no bonus
 */
SDL_Color MenuCharacter::bonusColor(int stat) {
	if (stat > 0) return font->getColor("menu_bonus");
	if (stat < 0) return font->getColor("menu_penalty");
	return font->getColor("menu_label");
}

void MenuCharacter::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
		snd->play(sfx_close);
	}

	// upgrade buttons
	for (int i=0; i<4; i++) {
		upgradeButton[i]->enabled = false;
	}

	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	skill_points = (stats->level * stats->stat_points_per_level) - spent;

	if (stats->hp > 0 && spent < (stats->level * stats->stat_points_per_level) && spent < stats->max_spendable_stat_points && stats->humanoid ) {
		if (stats->physical_character < stats->max_points_per_stat && show_upgrade[0]) upgradeButton[0]->enabled = true;
		if (stats->mental_character  < stats->max_points_per_stat && show_upgrade[1]) upgradeButton[1]->enabled = true;
		if (stats->offense_character < stats->max_points_per_stat && show_upgrade[2]) upgradeButton[2]->enabled = true;
		if (stats->defense_character < stats->max_points_per_stat && show_upgrade[3]) upgradeButton[3]->enabled = true;
	}

	if (upgradeButton[0]->checkClick()) physical_up = true;
	if (upgradeButton[1]->checkClick()) mental_up = true;
	if (upgradeButton[2]->checkClick()) offense_up = true;
	if (upgradeButton[3]->checkClick()) defense_up = true;

	statList->checkClick();

	if (stats->refresh_stats) refreshStats();
}



void MenuCharacter::render() {
	if (!visible) return;

	SDL_Rect src;
	SDL_Rect dest;

	// background
	src.x = 0;
	src.y = 0;
	dest.x = window_area.x;
	dest.y = window_area.y;
	src.w = dest.w = 320;
	src.h = dest.h = 416;
	SDL_BlitSurface(background, &src, screen, &dest);

	// close button
	closeButton->render();

	// title
	labelCharacter->render();

	// unspent points
	labelUnspent->render();

	// labels and values
	for (int i=0; i<CSTAT_COUNT; i++) {
		if (cstat[i].visible) {
			cstat[i].label->render();
			cstat[i].value->render();
		}
	}

	// upgrade buttons
	for (int i=0; i<4; i++) {
		if (upgradeButton[i]->enabled) upgradeButton[i]->render();
	}

	statList->render();
}


/**
 * Display various mouseovers tooltips depending on cursor location
 */
TooltipData MenuCharacter::checkTooltip() {

	for (int i=0; i<CSTAT_COUNT; i++) {
		if (isWithin(cstat[i].hover, inpt->mouse) && !cstat[i].tip.isEmpty() && cstat[i].visible)
			return cstat[i].tip;
	}

	return statList->checkTooltip(inpt->mouse);
}

/**
 * User might click this menu to upgrade a stat.  Check for this situation.
 * Return true if a stat was upgraded.
 */
bool MenuCharacter::checkUpgrade() {
	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	skill_points = (stats->level * stats->stat_points_per_level) - spent;

	// check to see if there are skill points available
	if (spent < (stats->level * stats->stat_points_per_level) && spent < stats->max_spendable_stat_points) {

		// physical
		if (physical_up) {
			stats->physical_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true; //TODO: Only show if a NEW power is unlocked...
			physical_up = false;
			return true;
		}
		// mental
		else if (mental_up) {
			stats->mental_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			mental_up = false;
			return true;
		}
		// offense
		else if (offense_up) {
			stats->offense_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			offense_up = false;
			return true;
		}
		// defense
		else if (defense_up) {
			stats->defense_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			defense_up = false;
			return true;
		}
	}

	return false;
}

MenuCharacter::~MenuCharacter() {
	SDL_FreeSurface(background);
	delete closeButton;

	delete labelCharacter;
	delete labelUnspent;
	for (int i=0; i<CSTAT_COUNT; i++) {
		delete cstat[i].label;
		delete cstat[i].value;
	}
	for (int i=0; i<4; i++) {
		delete upgradeButton[i];
	}
	delete statList;
}
