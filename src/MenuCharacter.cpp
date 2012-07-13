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
 * class MenuCharacter
 */

#include "Menu.h"
#include "MenuCharacter.h"
#include "SharedResources.h"
#include "Settings.h"
#include "StatBlock.h"
#include "WidgetButton.h"


using namespace std;


MenuCharacter::MenuCharacter(StatBlock *_stats) {
	stats = _stats;

	skill_points = 0;

	visible = false;
	newPowerNotification = false;

	loadGraphics();

	closeButton = new WidgetButton(mods->locate("images/menus/buttons/button_x.png"));

	// menu title
	labelCharacter = new WidgetLabel();

	for (int i=0; i<CSTAT_COUNT; i++) {
		cstat[i].label = new WidgetLabel();
		cstat[i].value = new WidgetLabel();
		cstat[i].hover.x = cstat[i].hover.y = 0;
		cstat[i].hover.w = cstat[i].hover.h = 0;
	}

	for (int i=0; i<CPROF_COUNT; i++) {
		cstat[i].hover.x = cstat[i].hover.y = 0;
		cstat[i].hover.w = cstat[i].hover.h = 0;
	}

}

void MenuCharacter::update() {
	// TODO put item position info in a config file

	// close button
	closeButton->pos.x = window_area.x + 294;
	closeButton->pos.y = window_area.y + 2;

	// menu title
	labelCharacter->set(window_area.x+window_area.w/2, window_area.y+16, JUSTIFY_CENTER, VALIGN_CENTER, msg->get("Character"), FONT_WHITE);

	// setup static labels
	cstat[CSTAT_NAME].label->set(window_area.x+72, window_area.y+40, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Name"), FONT_WHITE);
	cstat[CSTAT_LEVEL].label->set(window_area.x+264, window_area.y+40, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Level"), FONT_WHITE);
	cstat[CSTAT_PHYSICAL].label->set(window_area.x+40, window_area.y+80, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Physical"), FONT_WHITE);
	cstat[CSTAT_MENTAL].label->set(window_area.x+40, window_area.y+144, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Mental"), FONT_WHITE);
	cstat[CSTAT_OFFENSE].label->set(window_area.x+40, window_area.y+208, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Offense"), FONT_WHITE);
	cstat[CSTAT_DEFENSE].label->set(window_area.x+40, window_area.y+272, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("Defense"), FONT_WHITE);
	cstat[CSTAT_HP].label->set(window_area.x+152, window_area.y+112, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Total HP"), FONT_WHITE);
	cstat[CSTAT_HPREGEN].label->set(window_area.x+264, window_area.y+112, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Regen"), FONT_WHITE);
	cstat[CSTAT_MP].label->set(window_area.x+152, window_area.y+176, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Total MP"), FONT_WHITE);
	cstat[CSTAT_MPREGEN].label->set(window_area.x+264, window_area.y+176, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Regen"), FONT_WHITE);
	cstat[CSTAT_ACCURACYV1].label->set(window_area.x+152, window_area.y+240, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Accuracy vs. Def 1"), FONT_WHITE);
	cstat[CSTAT_ACCURACYV5].label->set(window_area.x+264, window_area.y+240, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("vs. Def 5"), FONT_WHITE);
	cstat[CSTAT_AVOIDANCEV1].label->set(window_area.x+152, window_area.y+304, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Avoidance vs. Off 1"), FONT_WHITE);
	cstat[CSTAT_AVOIDANCEV5].label->set(window_area.x+264, window_area.y+304, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("vs. Off 5"), FONT_WHITE);
	cstat[CSTAT_DMGMAIN].label->set(window_area.x+136, window_area.y+344, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Main Weapon"), FONT_WHITE);
	cstat[CSTAT_DMGRANGED].label->set(window_area.x+136, window_area.y+360, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Ranged Weapon"), FONT_WHITE);
	cstat[CSTAT_CRIT].label->set(window_area.x+136, window_area.y+376, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Crit Chance"), FONT_WHITE);
	cstat[CSTAT_ABSORB].label->set(window_area.x+264, window_area.y+344, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Absorb"), FONT_WHITE);
	cstat[CSTAT_FIRERESIST].label->set(window_area.x+264, window_area.y+360, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Fire Resist"), FONT_WHITE);
	cstat[CSTAT_ICERESIST].label->set(window_area.x+264, window_area.y+376, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("Ice Resist"), FONT_WHITE);

	// setup hotspot locations
	cstat[CSTAT_NAME].setHover(window_area.x+80, window_area.y+32, 104, 16);
	cstat[CSTAT_LEVEL].setHover(window_area.x+272, window_area.y+32, 32, 16);
	cstat[CSTAT_PHYSICAL].setHover(window_area.x+16, window_area.y+72, 16, 16);
	cstat[CSTAT_MENTAL].setHover(window_area.x+16, window_area.y+136, 16, 16);
	cstat[CSTAT_OFFENSE].setHover(window_area.x+16, window_area.y+200, 16, 16);
	cstat[CSTAT_DEFENSE].setHover(window_area.x+16, window_area.y+264, 16, 16);
	cstat[CSTAT_HP].setHover(window_area.x+160, window_area.y+104, 32, 16);
	cstat[CSTAT_HPREGEN].setHover(window_area.x+272, window_area.y+104, 32, 16);
	cstat[CSTAT_MP].setHover(window_area.x+160, window_area.y+168, 32, 16);
	cstat[CSTAT_MPREGEN].setHover(window_area.x+272, window_area.y+168, 32, 16);
	cstat[CSTAT_ACCURACYV1].setHover(window_area.x+160, window_area.y+232, 32, 16);
	cstat[CSTAT_ACCURACYV5].setHover(window_area.x+272, window_area.y+232, 32, 16);
	cstat[CSTAT_AVOIDANCEV1].setHover(window_area.x+160, window_area.y+296, 32, 16);
	cstat[CSTAT_AVOIDANCEV5].setHover(window_area.x+272, window_area.y+296, 32, 16);
	cstat[CSTAT_DMGMAIN].setHover(window_area.x+144, window_area.y+336, 32, 16);
	cstat[CSTAT_DMGRANGED].setHover(window_area.x+144, window_area.y+352, 32, 16);
	cstat[CSTAT_CRIT].setHover(window_area.x+144, window_area.y+368, 32, 16);
	cstat[CSTAT_ABSORB].setHover(window_area.x+272, window_area.y+336, 32, 16);
	cstat[CSTAT_FIRERESIST].setHover(window_area.x+272, window_area.y+352, 32, 16);
	cstat[CSTAT_ICERESIST].setHover(window_area.x+272, window_area.y+368, 32, 16);
	cstat[CSTAT_UNSPENT].setHover(window_area.x+90, window_area.y+392, 120, 16);

	cprof[CPROF_P2].setHover(window_area.x+128, window_area.y+64, 32, 32);
	cprof[CPROF_P3].setHover(window_area.x+176, window_area.y+64, 32, 32);
	cprof[CPROF_P4].setHover(window_area.x+224, window_area.y+64, 32, 32);
	cprof[CPROF_P5].setHover(window_area.x+272, window_area.y+64, 32, 32);
	cprof[CPROF_M2].setHover(window_area.x+128, window_area.y+128, 32, 32);
	cprof[CPROF_M3].setHover(window_area.x+176, window_area.y+128, 32, 32);
	cprof[CPROF_M4].setHover(window_area.x+224, window_area.y+128, 32, 32);
	cprof[CPROF_M5].setHover(window_area.x+272, window_area.y+128, 32, 32);
	cprof[CPROF_O2].setHover(window_area.x+128, window_area.y+192, 32, 32);
	cprof[CPROF_O3].setHover(window_area.x+176, window_area.y+192, 32, 32);
	cprof[CPROF_O4].setHover(window_area.x+224, window_area.y+192, 32, 32);
	cprof[CPROF_O5].setHover(window_area.x+272, window_area.y+192, 32, 32);
	cprof[CPROF_D2].setHover(window_area.x+128, window_area.y+256, 32, 32);
	cprof[CPROF_D3].setHover(window_area.x+176, window_area.y+256, 32, 32);
	cprof[CPROF_D4].setHover(window_area.x+224, window_area.y+256, 32, 32);
	cprof[CPROF_D5].setHover(window_area.x+272, window_area.y+256, 32, 32);

}

void MenuCharacter::loadGraphics() {

	background = IMG_Load(mods->locate("images/menus/character.png").c_str());
	proficiency = IMG_Load(mods->locate("images/menus/character_proficiency.png").c_str());
	upgrade = IMG_Load(mods->locate("images/menus/upgrade.png").c_str());
	if(!background || !proficiency || !upgrade) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}

	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	cleanup = proficiency;
	proficiency = SDL_DisplayFormatAlpha(proficiency);
	SDL_FreeSurface(cleanup);

	cleanup = upgrade;
	upgrade = SDL_DisplayFormatAlpha(upgrade);
	SDL_FreeSurface(cleanup);

}

/**
 * Rebuild all stat values and tooltip info
 */
void MenuCharacter::refreshStats() {

	stringstream ss;

	// update stat text
	cstat[CSTAT_NAME].value->set(window_area.x+84, window_area.y+40, JUSTIFY_LEFT, VALIGN_CENTER, stats->name, FONT_WHITE);

	ss.str("");
	ss << stats->level;
	cstat[CSTAT_LEVEL].value->set(window_area.x+288, window_area.y+40, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->get_physical();
	cstat[CSTAT_PHYSICAL].value->set(window_area.x+24, window_area.y+80, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->physical_additional));

	ss.str("");
	ss << stats->get_mental();
	cstat[CSTAT_MENTAL].value->set(window_area.x+24, window_area.y+144, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->mental_additional));

	ss.str("");
	ss << stats->get_offense();
	cstat[CSTAT_OFFENSE].value->set(window_area.x+24, window_area.y+208, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->offense_additional));

	ss.str("");
	ss << stats->get_defense();
	cstat[CSTAT_DEFENSE].value->set(window_area.x+24, window_area.y+272, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->defense_additional));

	ss.str("");
	ss << stats->maxhp;
	cstat[CSTAT_HP].value->set(window_area.x+176, window_area.y+112, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->hp_per_minute;
	cstat[CSTAT_HPREGEN].value->set(window_area.x+288, window_area.y+112, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->maxmp;
	cstat[CSTAT_MP].value->set(window_area.x+176, window_area.y+176, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->mp_per_minute;
	cstat[CSTAT_MPREGEN].value->set(window_area.x+288, window_area.y+176, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->accuracy) << "%";
	cstat[CSTAT_ACCURACYV1].value->set(window_area.x+176, window_area.y+240, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->accuracy - 20) << "%";
	cstat[CSTAT_ACCURACYV5].value->set(window_area.x+288, window_area.y+240, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->avoidance) << "%";
	cstat[CSTAT_AVOIDANCEV1].value->set(window_area.x+176, window_area.y+304, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->avoidance - 20) << "%";
	cstat[CSTAT_AVOIDANCEV5].value->set(window_area.x+288, window_area.y+304, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (stats->dmg_melee_max >= stats->dmg_ment_max)
		ss << stats->dmg_melee_min << "-" << stats->dmg_melee_max;
	else
		ss << stats->dmg_ment_min << "-" << stats->dmg_ment_max;
	cstat[CSTAT_DMGMAIN].value->set(window_area.x+160, window_area.y+344, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (stats->dmg_ranged_max > 0)
		ss << stats->dmg_ranged_min << "-" << stats->dmg_ranged_max;
	else
		ss << "-";
	cstat[CSTAT_DMGRANGED].value->set(window_area.x+160, window_area.y+360, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->crit << "%";
	cstat[CSTAT_CRIT].value->set(window_area.x+160, window_area.y+376, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (stats->absorb_min == stats->absorb_max)
		ss << stats->absorb_min;
	else
		ss << stats->absorb_min << "-" << stats->absorb_max;
	cstat[CSTAT_ABSORB].value->set(window_area.x+288, window_area.y+344, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (100 - stats->attunement_fire) << "%";
	cstat[CSTAT_FIRERESIST].value->set(window_area.x+288, window_area.y+360, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (100 - stats->attunement_ice) << "%";
	cstat[CSTAT_ICERESIST].value->set(window_area.x+288, window_area.y+376, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (skill_points > 0) ss << skill_points << " " << msg->get("points remaining");
	else ss.str("");
	cstat[CSTAT_UNSPENT].value->set(window_area.x+155, window_area.y+400, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_GREEN);
	ss.str("");

	// update tool tips
	cstat[CSTAT_NAME].tip.num_lines = 0;
	cstat[CSTAT_NAME].tip.lines[cstat[CSTAT_NAME].tip.num_lines++] = msg->get(stats->character_class);

	cstat[CSTAT_LEVEL].tip.num_lines = 0;
	cstat[CSTAT_LEVEL].tip.lines[cstat[CSTAT_LEVEL].tip.num_lines++] = msg->get("XP: %d", stats->xp);
	if (stats->level < MAX_CHARACTER_LEVEL) {
		cstat[CSTAT_LEVEL].tip.lines[cstat[CSTAT_LEVEL].tip.num_lines++] = msg->get("Next: %d", stats->xp_table[stats->level]);
	}

	cstat[CSTAT_PHYSICAL].tip.num_lines = 0;
	cstat[CSTAT_PHYSICAL].tip.lines[cstat[CSTAT_PHYSICAL].tip.num_lines++] = msg->get("Physical (P) increases melee weapon proficiency and total HP.");
	cstat[CSTAT_PHYSICAL].tip.lines[cstat[CSTAT_PHYSICAL].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->physical_character, stats->physical_additional);

	cstat[CSTAT_MENTAL].tip.num_lines = 0;
	cstat[CSTAT_MENTAL].tip.lines[cstat[CSTAT_MENTAL].tip.num_lines++] = msg->get("Mental (M) increases mental weapon proficiency and total MP.");
	cstat[CSTAT_MENTAL].tip.lines[cstat[CSTAT_MENTAL].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->mental_character, stats->mental_additional);

	cstat[CSTAT_OFFENSE].tip.num_lines = 0;
	cstat[CSTAT_OFFENSE].tip.lines[cstat[CSTAT_OFFENSE].tip.num_lines++] = msg->get("Offense (O) increases ranged weapon proficiency and accuracy.");
	cstat[CSTAT_OFFENSE].tip.lines[cstat[CSTAT_OFFENSE].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->offense_character, stats->offense_additional);

	cstat[CSTAT_DEFENSE].tip.num_lines = 0;
	cstat[CSTAT_DEFENSE].tip.lines[cstat[CSTAT_DEFENSE].tip.num_lines++] = msg->get("Defense (D) increases armor proficiency and avoidance.");
	cstat[CSTAT_DEFENSE].tip.lines[cstat[CSTAT_DEFENSE].tip.num_lines++] = msg->get("base (%d), bonus (%d)", stats->defense_character, stats->defense_additional);

	cstat[CSTAT_HP].tip.num_lines = 0;
	cstat[CSTAT_HP].tip.lines[cstat[CSTAT_HP].tip.num_lines++] = msg->get("Each point of Physical grants +8 HP");
	cstat[CSTAT_HP].tip.lines[cstat[CSTAT_HP].tip.num_lines++] = msg->get("Each level grants +2 HP");

	cstat[CSTAT_HPREGEN].tip.num_lines = 0;
	cstat[CSTAT_HPREGEN].tip.lines[cstat[CSTAT_HPREGEN].tip.num_lines++] = msg->get("Ticks of HP regen per minute");
	cstat[CSTAT_HPREGEN].tip.lines[cstat[CSTAT_HPREGEN].tip.num_lines++] = msg->get("Each point of Physical grants +4 HP regen");
	cstat[CSTAT_HPREGEN].tip.lines[cstat[CSTAT_HPREGEN].tip.num_lines++] = msg->get("Each level grants +1 HP regen");

	cstat[CSTAT_MP].tip.num_lines = 0;
	cstat[CSTAT_MP].tip.lines[cstat[CSTAT_MP].tip.num_lines++] = msg->get("Each point of Mental grants +8 MP");
	cstat[CSTAT_MP].tip.lines[cstat[CSTAT_MP].tip.num_lines++] = msg->get("Each level grants +2 MP");

	cstat[CSTAT_MPREGEN].tip.num_lines = 0;
	cstat[CSTAT_MPREGEN].tip.lines[cstat[CSTAT_MPREGEN].tip.num_lines++] = msg->get("Ticks of MP regen per minute");
	cstat[CSTAT_MPREGEN].tip.lines[cstat[CSTAT_MPREGEN].tip.num_lines++] = msg->get("Each point of Mental grants +4 MP regen");
	cstat[CSTAT_MPREGEN].tip.lines[cstat[CSTAT_MPREGEN].tip.num_lines++] = msg->get("Each level grants +1 MP regen");

	cstat[CSTAT_ACCURACYV1].tip.num_lines = 0;
	cstat[CSTAT_ACCURACYV1].tip.lines[cstat[CSTAT_ACCURACYV1].tip.num_lines++] = msg->get("Each point of Offense grants +5 accuracy");
	cstat[CSTAT_ACCURACYV1].tip.lines[cstat[CSTAT_ACCURACYV1].tip.num_lines++] = msg->get("Each level grants +1 accuracy");

	cstat[CSTAT_ACCURACYV5].tip.num_lines = 0;
	cstat[CSTAT_ACCURACYV5].tip.lines[cstat[CSTAT_ACCURACYV5].tip.num_lines++] = msg->get("Each point of Offense grants +5 accuracy");
	cstat[CSTAT_ACCURACYV5].tip.lines[cstat[CSTAT_ACCURACYV5].tip.num_lines++] = msg->get("Each level grants +1 accuracy");

	cstat[CSTAT_AVOIDANCEV1].tip.num_lines = 0;
	cstat[CSTAT_AVOIDANCEV1].tip.lines[cstat[CSTAT_AVOIDANCEV1].tip.num_lines++] = msg->get("Each point of Defense grants +5 avoidance");
	cstat[CSTAT_AVOIDANCEV1].tip.lines[cstat[CSTAT_AVOIDANCEV1].tip.num_lines++] = msg->get("Each level grants +1 avoidance");

	cstat[CSTAT_AVOIDANCEV5].tip.num_lines = 0;
	cstat[CSTAT_AVOIDANCEV5].tip.lines[cstat[CSTAT_AVOIDANCEV5].tip.num_lines++] = msg->get("Each point of Defense grants +5 avoidance");
	cstat[CSTAT_AVOIDANCEV5].tip.lines[cstat[CSTAT_AVOIDANCEV5].tip.num_lines++] = msg->get("Each level grants +1 avoidance");

	cstat[CSTAT_UNSPENT].tip.num_lines = 0;
	if (skill_points) cstat[CSTAT_UNSPENT].tip.lines[cstat[CSTAT_UNSPENT].tip.num_lines++] = msg->get("Unspent attribute points");

	// proficiency tooltips
	cprof[CPROF_P2].tip.num_lines = 0;
	cprof[CPROF_P2].tip.lines[cprof[CPROF_P2].tip.num_lines++] = msg->get("Dagger Proficiency");
	if (stats->get_physical() < 2) cprof[CPROF_P2].tip.colors[cprof[CPROF_P2].tip.num_lines] = FONT_RED;
	else cprof[CPROF_P2].tip.colors[cprof[CPROF_P2].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_P2].tip.lines[cprof[CPROF_P2].tip.num_lines++] = msg->get("Requires Physical %d", 2);

	cprof[CPROF_P3].tip.num_lines = 0;
	cprof[CPROF_P3].tip.lines[cprof[CPROF_P3].tip.num_lines++] = msg->get("Shortsword Proficiency");
	if (stats->get_physical() < 3) cprof[CPROF_P3].tip.colors[cprof[CPROF_P3].tip.num_lines] = FONT_RED;
	else cprof[CPROF_P3].tip.colors[cprof[CPROF_P3].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_P3].tip.lines[cprof[CPROF_P3].tip.num_lines++] = msg->get("Requires Physical %d", 3);

	cprof[CPROF_P4].tip.num_lines = 0;
	cprof[CPROF_P4].tip.lines[cprof[CPROF_P4].tip.num_lines++] = msg->get("Longsword Proficiency");
	if (stats->get_physical() < 4) cprof[CPROF_P4].tip.colors[cprof[CPROF_P4].tip.num_lines] = FONT_RED;
	else cprof[CPROF_P4].tip.colors[cprof[CPROF_P4].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_P4].tip.lines[cprof[CPROF_P4].tip.num_lines++] = msg->get("Requires Physical %d", 4);

	cprof[CPROF_P5].tip.num_lines = 0;
	cprof[CPROF_P5].tip.lines[cprof[CPROF_P5].tip.num_lines++] = msg->get("Greatsword Proficiency");
	if (stats->get_physical() < 5) cprof[CPROF_P5].tip.colors[cprof[CPROF_P5].tip.num_lines] = FONT_RED;
	else cprof[CPROF_P5].tip.colors[cprof[CPROF_P5].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_P5].tip.lines[cprof[CPROF_P5].tip.num_lines++] = msg->get("Requires Physical %d", 5);

	cprof[CPROF_M2].tip.num_lines = 0;
	cprof[CPROF_M2].tip.lines[cprof[CPROF_M2].tip.num_lines++] = msg->get("Wand Proficiency");
	if (stats->get_mental() < 2) cprof[CPROF_M2].tip.colors[cprof[CPROF_M2].tip.num_lines] = FONT_RED;
	else cprof[CPROF_M2].tip.colors[cprof[CPROF_M2].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_M2].tip.lines[cprof[CPROF_M2].tip.num_lines++] = msg->get("Requires Mental %d", 2);

	cprof[CPROF_M3].tip.num_lines = 0;
	cprof[CPROF_M3].tip.lines[cprof[CPROF_M3].tip.num_lines++] = msg->get("Rod Proficiency");
	if (stats->get_mental() < 3) cprof[CPROF_M3].tip.colors[cprof[CPROF_M3].tip.num_lines] = FONT_RED;
	else cprof[CPROF_M3].tip.colors[cprof[CPROF_M3].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_M3].tip.lines[cprof[CPROF_M3].tip.num_lines++] = msg->get("Requires Mental %d", 3);

	cprof[CPROF_M4].tip.num_lines = 0;
	cprof[CPROF_M4].tip.lines[cprof[CPROF_M4].tip.num_lines++] = msg->get("Staff Proficiency");
	if (stats->get_mental() < 4) cprof[CPROF_M4].tip.colors[cprof[CPROF_M4].tip.num_lines] = FONT_RED;
	else cprof[CPROF_M4].tip.colors[cprof[CPROF_M4].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_M4].tip.lines[cprof[CPROF_M4].tip.num_lines++] = msg->get("Requires Mental %d", 4);

	cprof[CPROF_M5].tip.num_lines = 0;
	cprof[CPROF_M5].tip.lines[cprof[CPROF_M5].tip.num_lines++] = msg->get("Greatstaff Proficiency");
	if (stats->get_mental() < 5) cprof[CPROF_M5].tip.colors[cprof[CPROF_M5].tip.num_lines] = FONT_RED;
	else cprof[CPROF_M5].tip.colors[cprof[CPROF_M5].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_M5].tip.lines[cprof[CPROF_M5].tip.num_lines++] = msg->get("Requires Mental %d", 5);

	cprof[CPROF_O2].tip.num_lines = 0;
	cprof[CPROF_O2].tip.lines[cprof[CPROF_O2].tip.num_lines++] = msg->get("Slingshot Proficiency");
	if (stats->get_offense() < 2) cprof[CPROF_O2].tip.colors[cprof[CPROF_O2].tip.num_lines] = FONT_RED;
	else cprof[CPROF_O2].tip.colors[cprof[CPROF_O2].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_O2].tip.lines[cprof[CPROF_O2].tip.num_lines++] = msg->get("Requires Offense %d", 2);

	cprof[CPROF_O3].tip.num_lines = 0;
	cprof[CPROF_O3].tip.lines[cprof[CPROF_O3].tip.num_lines++] = msg->get("Shortbow Proficiency");
	if (stats->get_offense() < 3) cprof[CPROF_O3].tip.colors[cprof[CPROF_O3].tip.num_lines] = FONT_RED;
	else cprof[CPROF_O3].tip.colors[cprof[CPROF_O3].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_O3].tip.lines[cprof[CPROF_O3].tip.num_lines++] = msg->get("Requires Offense %d", 3);

	cprof[CPROF_O4].tip.num_lines = 0;
	cprof[CPROF_O4].tip.lines[cprof[CPROF_O4].tip.num_lines++] = msg->get("Longbow Proficiency");
	if (stats->get_offense() < 4) cprof[CPROF_O4].tip.colors[cprof[CPROF_O4].tip.num_lines] = FONT_RED;
	else cprof[CPROF_O4].tip.colors[cprof[CPROF_O4].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_O4].tip.lines[cprof[CPROF_O4].tip.num_lines++] = msg->get("Requires Offense %d", 4);

	cprof[CPROF_O5].tip.num_lines = 0;
	cprof[CPROF_O5].tip.lines[cprof[CPROF_O5].tip.num_lines++] = msg->get("Greatbow Proficiency");
	if (stats->get_offense() < 5) cprof[CPROF_O5].tip.colors[cprof[CPROF_O5].tip.num_lines] = FONT_RED;
	else cprof[CPROF_O5].tip.colors[cprof[CPROF_O5].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_O5].tip.lines[cprof[CPROF_O5].tip.num_lines++] = msg->get("Requires Offense %d", 5);

	cprof[CPROF_D2].tip.num_lines = 0;
	cprof[CPROF_D2].tip.lines[cprof[CPROF_D2].tip.num_lines++] = msg->get("Light Armor Proficiency");
	if (stats->get_defense() < 2) cprof[CPROF_D2].tip.colors[cprof[CPROF_D2].tip.num_lines] = FONT_RED;
	else cprof[CPROF_D2].tip.colors[cprof[CPROF_D2].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_D2].tip.lines[cprof[CPROF_D2].tip.num_lines++] = msg->get("Requires Defense %d", 2);

	cprof[CPROF_D3].tip.num_lines = 0;
	cprof[CPROF_D3].tip.lines[cprof[CPROF_D3].tip.num_lines++] = msg->get("Light Shield Proficiency");
	if (stats->get_defense() < 3) cprof[CPROF_D3].tip.colors[cprof[CPROF_D3].tip.num_lines] = FONT_RED;
	else cprof[CPROF_D3].tip.colors[cprof[CPROF_D3].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_D3].tip.lines[cprof[CPROF_D3].tip.num_lines++] = msg->get("Requires Defense %d", 3);

	cprof[CPROF_D4].tip.num_lines = 0;
	cprof[CPROF_D4].tip.lines[cprof[CPROF_D4].tip.num_lines++] = msg->get("Heavy Armor Proficiency");
	if (stats->get_defense() < 4) cprof[CPROF_D4].tip.colors[cprof[CPROF_D4].tip.num_lines] = FONT_RED;
	else cprof[CPROF_D4].tip.colors[cprof[CPROF_D4].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_D4].tip.lines[cprof[CPROF_D4].tip.num_lines++] = msg->get("Requires Defense %d", 4);

	cprof[CPROF_D5].tip.num_lines = 0;
	cprof[CPROF_D5].tip.lines[cprof[CPROF_D5].tip.num_lines++] = msg->get("Heavy Shield Proficiency");
	if (stats->get_defense() < 5) cprof[CPROF_D5].tip.colors[cprof[CPROF_D5].tip.num_lines] = FONT_RED;
	else cprof[CPROF_D5].tip.colors[cprof[CPROF_D5].tip.num_lines] = FONT_WHITE;
	cprof[CPROF_D5].tip.lines[cprof[CPROF_D5].tip.num_lines++] = msg->get("Requires Defense %d", 5);

}


/**
 * Color-coding for positive/negative/no bonus
 */
int MenuCharacter::bonusColor(int stat) {
	if (stat > 0) return FONT_GREEN;
	if (stat < 0) return FONT_RED;
	return FONT_WHITE;
}

void MenuCharacter::logic() {
	if (!visible) return;

	if (closeButton->checkClick()) {
		visible = false;
	}

	// TODO: this doesn't need to be done every frame. Only call this when something has updated
	refreshStats();
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

	// labels and values
	for (int i=0; i<CSTAT_COUNT; i++) {
		cstat[i].label->render();
		cstat[i].value->render();
	}


	// highlight proficiencies
	displayProficiencies(stats->get_physical(), window_area.y+64);
	displayProficiencies(stats->get_mental(), window_area.y+128);
	displayProficiencies(stats->get_offense(), window_area.y+192);
	displayProficiencies(stats->get_defense(), window_area.y+256);

	// if points are available, show the upgrade buttons
	// TODO: replace with WidgetButton

	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	int max_spendable_stat_points = 16;
	skill_points = stats->level - spent;

	// check to see if there are skill points available
	if (spent < stats->level && spent < max_spendable_stat_points) {

		src.x = 0;
		src.y = 0;
		src.w = dest.w = 32;
		src.h = dest.h = 16;
		dest.x = window_area.x + 16;

		// physical
		if (stats->physical_character < 5) { // && mouse.x >= 16 && mouse.y >= window_area.y+96
			dest.y = window_area.y + 96;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}
		// mental
		if (stats->mental_character < 5) { // && mouse.x >= 16 && mouse.y >= window_area.y+160
			dest.y = window_area.y + 160;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}
		// offense
		if (stats->offense_character < 5) { // && mouse.x >= 16 && mouse.y >= window_area.y+224
			dest.y = window_area.y + 224;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}
		// defense
		if (stats->defense_character < 5) { // && mouse.x >= 16 && mouse.y >= window_area.y+288
			dest.y = window_area.y + 288;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}


	}
}

/**
 * Display an overlay graphic to highlight which weapon/armor proficiencies are unlocked.
 * Similar routine for each row of attribute
 *
 * @param value The current attribute level
 * @param y The y pixel coordinate of this proficiency row
 */
void MenuCharacter::displayProficiencies(int value, int y) {
	SDL_Rect src;
	SDL_Rect dest;
	src.x = 0;
	src.y = 0;
	src.w = dest.w = 48;
	src.h = dest.h = 32;
	dest.y = y;

	// save-game hackers could set their stats higher than normal.
	// make sure this display still works.
	int actual_value = min(value,5);

	for (int i=2; i<= actual_value; i++) {
		dest.x = window_area.x + 112 + (i-2) * 48;
		SDL_BlitSurface(proficiency, &src, screen, &dest);
	}
}

/**
 * Display various mouseovers tooltips depending on cursor location
 */
TooltipData MenuCharacter::checkTooltip() {

	for (int i=0; i<CSTAT_COUNT; i++) {
		if (isWithin(cstat[i].hover, inpt->mouse) && cstat[i].tip.num_lines > 0)
			return cstat[i].tip;
	}

	for (int i=0; i<CPROF_COUNT; i++) {
		if (isWithin(cprof[i].hover, inpt->mouse) && cprof[i].tip.num_lines > 0)
			return cprof[i].tip;
	}

	TooltipData tip;
	tip.num_lines = 0;
	return tip;
}

/**
 * User might click this menu to upgrade a stat.  Check for this situation.
 * Return true if a stat was upgraded.
 */
bool MenuCharacter::checkUpgrade() {

	Point mouse;
	mouse.x = inpt->mouse.x;
	mouse.y = inpt->mouse.y;

	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	int max_spendable_stat_points = 16;
	skill_points = stats->level - spent;

	// check to see if there are skill points available
	if (spent < stats->level && spent < max_spendable_stat_points) {

		// check mouse hotspots

		// physical
		if (stats->physical_character < 5 && mouse.x >= window_area.x+16 && mouse.x <= window_area.x+48 && mouse.y >= window_area.y+96 && mouse.y <= window_area.y+112) {
			stats->physical_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true; //TODO: Only show if a NEW power is unlocked...
			return true;
		}
		// mental
		else if (stats->mental_character < 5 && mouse.x >= window_area.x+16 && mouse.x <= window_area.x+48 && mouse.y >= window_area.y+160 && mouse.y <= window_area.y+176) {
			stats->mental_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			return true;
		}
		// offense
		else if (stats->offense_character < 5 && mouse.x >= window_area.x+16 && mouse.x <= window_area.x+48 && mouse.y >= window_area.y+224 && mouse.y <= window_area.y+240) {
			stats->offense_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			return true;
		}
		// defense
		else if (stats->defense_character < 5 && mouse.x >= window_area.x+16 && mouse.x <= window_area.x+48 && mouse.y >= window_area.y+288 && mouse.y <= window_area.y+304) {
			stats->defense_character++;
			stats->recalc(); // equipment applied by MenuManager
			newPowerNotification = true;
			return true;
		}
	}

	return false;
}

MenuCharacter::~MenuCharacter() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(proficiency);
	SDL_FreeSurface(upgrade);
	delete closeButton;

	delete labelCharacter;
	for (int i=0; i<CSTAT_COUNT; i++) {
		delete cstat[i].label;
		delete cstat[i].value;
	}
}
