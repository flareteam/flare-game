/**
 * class MenuCharacter
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuCharacter.h"

MenuCharacter::MenuCharacter(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, StatBlock *_stats, MessageEngine *_msg) {
	screen = _screen;
	inp = _inp;
	font = _font;
	stats = _stats;
	msg = _msg;
	
	visible = false;

	loadGraphics();

	int offset_y = (VIEW_H - 416)/2;
	
	// button setup
	closeButton = new WidgetButton(screen, font, inp, "images/menus/buttons/button_x.png");
	closeButton->pos.x = 294;
	closeButton->pos.y = offset_y + 2;

	// menu title
	labelCharacter = new WidgetLabel(screen, font);
	labelCharacter->set(160, offset_y+16, JUSTIFY_CENTER, VALIGN_CENTER, msg->get("character"), FONT_WHITE);
	
	for (int i=0; i<CSTAT_COUNT; i++) {
		cstat[i].label = new WidgetLabel(screen, font);
		cstat[i].value = new WidgetLabel(screen, font);
		cstat[i].hover.x = cstat[i].hover.y = 0;
		cstat[i].hover.w = cstat[i].hover.h = 0;
	}
	
	for (int i=0; i<CPROF_COUNT; i++) {
		cstat[i].hover.x = cstat[i].hover.y = 0;
		cstat[i].hover.w = cstat[i].hover.h = 0;	
	}
	
	// setup static labels
	cstat[CSTAT_NAME].label->set(72, offset_y+40, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("name"), FONT_WHITE);
	cstat[CSTAT_LEVEL].label->set(264, offset_y+40, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("level"), FONT_WHITE);
	cstat[CSTAT_PHYSICAL].label->set(40, offset_y+80, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("physical"), FONT_WHITE);
	cstat[CSTAT_MENTAL].label->set(40, offset_y+144, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("mental"), FONT_WHITE);
	cstat[CSTAT_OFFENSE].label->set(40, offset_y+208, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("offense"), FONT_WHITE);
	cstat[CSTAT_DEFENSE].label->set(40, offset_y+272, JUSTIFY_LEFT, VALIGN_CENTER, msg->get("defense"), FONT_WHITE);
	cstat[CSTAT_HP].label->set(152, offset_y+112, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("total_hp"), FONT_WHITE);
	cstat[CSTAT_HPREGEN].label->set(264, offset_y+112, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("regen"), FONT_WHITE);
	cstat[CSTAT_MP].label->set(152, offset_y+176, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("total_mp"), FONT_WHITE);
	cstat[CSTAT_MPREGEN].label->set(264, offset_y+176, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("regen"), FONT_WHITE);
	cstat[CSTAT_ACCURACYV1].label->set(152, offset_y+240, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("accuracy_vs_def_1"), FONT_WHITE);
	cstat[CSTAT_ACCURACYV5].label->set(264, offset_y+240, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("vs_def_5"), FONT_WHITE);
	cstat[CSTAT_AVOIDANCEV1].label->set(152, offset_y+304, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("avoidance_vs_off_1"), FONT_WHITE);
	cstat[CSTAT_AVOIDANCEV5].label->set(264, offset_y+304, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("vs_off_5"), FONT_WHITE);
	cstat[CSTAT_DMGMAIN].label->set(136, offset_y+344, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("main_weapon"), FONT_WHITE);
	cstat[CSTAT_DMGRANGED].label->set(136, offset_y+360, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("ranged_weapon"), FONT_WHITE);
	cstat[CSTAT_CRIT].label->set(136, offset_y+376, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("crit_chance"), FONT_WHITE);
	cstat[CSTAT_ABSORB].label->set(264, offset_y+344, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("absorb"), FONT_WHITE);
	cstat[CSTAT_FIRERESIST].label->set(264, offset_y+360, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("fire_resist"), FONT_WHITE);
	cstat[CSTAT_ICERESIST].label->set(264, offset_y+376, JUSTIFY_RIGHT, VALIGN_CENTER, msg->get("ice_resist"), FONT_WHITE);
	
	// setup hotspot locations
	cstat[CSTAT_NAME].setHover(80, offset_y+32, 104, 16);
	cstat[CSTAT_LEVEL].setHover(272, offset_y+32, 32, 16);
	cstat[CSTAT_PHYSICAL].setHover(16, offset_y+72, 16, 16);
	cstat[CSTAT_MENTAL].setHover(16, offset_y+136, 16, 16);
	cstat[CSTAT_OFFENSE].setHover(16, offset_y+200, 16, 16);
	cstat[CSTAT_DEFENSE].setHover(16, offset_y+264, 16, 16);
	cstat[CSTAT_HP].setHover(160, offset_y+104, 32, 16);
	cstat[CSTAT_HPREGEN].setHover(272, offset_y+104, 32, 16);
	cstat[CSTAT_MP].setHover(160, offset_y+168, 32, 16);
	cstat[CSTAT_MPREGEN].setHover(272, offset_y+168, 32, 16);
	cstat[CSTAT_ACCURACYV1].setHover(160, offset_y+232, 32, 16);
	cstat[CSTAT_ACCURACYV5].setHover(272, offset_y+232, 32, 16);
	cstat[CSTAT_AVOIDANCEV1].setHover(160, offset_y+296, 32, 16);
	cstat[CSTAT_AVOIDANCEV5].setHover(272, offset_y+296, 32, 16);
	cstat[CSTAT_DMGMAIN].setHover(144, offset_y+336, 32, 16);
	cstat[CSTAT_DMGRANGED].setHover(144, offset_y+352, 32, 16);
	cstat[CSTAT_CRIT].setHover(144, offset_y+368, 32, 16);
	cstat[CSTAT_ABSORB].setHover(272, offset_y+336, 32, 16);
	cstat[CSTAT_FIRERESIST].setHover(272, offset_y+352, 32, 16);
	cstat[CSTAT_ICERESIST].setHover(272, offset_y+368, 32, 16);
	
	cprof[CPROF_P2].setHover(128, offset_y+64, 32, 32);
	cprof[CPROF_P3].setHover(176, offset_y+64, 32, 32);
	cprof[CPROF_P4].setHover(224, offset_y+64, 32, 32);
	cprof[CPROF_P5].setHover(272, offset_y+64, 32, 32);
	cprof[CPROF_M2].setHover(128, offset_y+128, 32, 32);
	cprof[CPROF_M3].setHover(176, offset_y+128, 32, 32);
	cprof[CPROF_M4].setHover(224, offset_y+128, 32, 32);
	cprof[CPROF_M5].setHover(272, offset_y+128, 32, 32);
	cprof[CPROF_O2].setHover(128, offset_y+192, 32, 32);
	cprof[CPROF_O3].setHover(176, offset_y+192, 32, 32);
	cprof[CPROF_O4].setHover(224, offset_y+192, 32, 32);
	cprof[CPROF_O5].setHover(272, offset_y+192, 32, 32);
	cprof[CPROF_D2].setHover(128, offset_y+256, 32, 32);
	cprof[CPROF_D3].setHover(176, offset_y+256, 32, 32);
	cprof[CPROF_D4].setHover(224, offset_y+256, 32, 32);
	cprof[CPROF_D5].setHover(272, offset_y+256, 32, 32);
	
}

void MenuCharacter::loadGraphics() {

	background = IMG_Load((PATH_DATA + "images/menus/character.png").c_str());
	proficiency = IMG_Load((PATH_DATA + "images/menus/character_proficiency.png").c_str());
	upgrade = IMG_Load((PATH_DATA + "images/menus/upgrade.png").c_str());
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
	int offset_y = (VIEW_H - 416)/2;
	
	// update stat text
	cstat[CSTAT_NAME].value->set(84, offset_y+40, JUSTIFY_LEFT, VALIGN_CENTER, stats->name, FONT_WHITE);

	ss.str("");
	ss << stats->level;
	cstat[CSTAT_LEVEL].value->set(288, offset_y+40, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);
	
	ss.str("");
	ss << stats->get_physical();
	cstat[CSTAT_PHYSICAL].value->set(24, offset_y+80, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->physical_additional));

	ss.str("");
	ss << stats->get_mental();
	cstat[CSTAT_MENTAL].value->set(24, offset_y+144, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->mental_additional));

	ss.str("");
	ss << stats->get_offense();
	cstat[CSTAT_OFFENSE].value->set(24, offset_y+208, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->offense_additional));

	ss.str("");
	ss << stats->get_defense();
	cstat[CSTAT_DEFENSE].value->set(24, offset_y+272, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), bonusColor(stats->defense_additional));

	ss.str("");
	ss << stats->maxhp;
	cstat[CSTAT_HP].value->set(176, offset_y+112, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);
	
	ss.str("");
	ss << stats->hp_per_minute;
	cstat[CSTAT_HPREGEN].value->set(288, offset_y+112, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->maxmp;
	cstat[CSTAT_MP].value->set(176, offset_y+176, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->mp_per_minute;
	cstat[CSTAT_MPREGEN].value->set(288, offset_y+176, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->accuracy) << "%";
	cstat[CSTAT_ACCURACYV1].value->set(176, offset_y+240, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->accuracy - 20) << "%";
	cstat[CSTAT_ACCURACYV5].value->set(288, offset_y+240, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->avoidance) << "%";
	cstat[CSTAT_AVOIDANCEV1].value->set(176, offset_y+304, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (stats->avoidance - 20) << "%";
	cstat[CSTAT_AVOIDANCEV5].value->set(288, offset_y+304, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (stats->dmg_melee_max >= stats->dmg_ment_max)
		ss << stats->dmg_melee_min << "-" << stats->dmg_melee_max;
	else
		ss << stats->dmg_ment_min << "-" << stats->dmg_ment_max;
	cstat[CSTAT_DMGMAIN].value->set(160, offset_y+344, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (stats->dmg_ranged_max > 0)
		ss << stats->dmg_ranged_min << "-" << stats->dmg_ranged_max;
	else
		ss << "-";
	cstat[CSTAT_DMGRANGED].value->set(160, offset_y+360, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << stats->crit << "%";
	cstat[CSTAT_CRIT].value->set(160, offset_y+376, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	if (stats->absorb_min == stats->absorb_max)
		ss << stats->absorb_min;
	else
		ss << stats->absorb_min << "-" << stats->absorb_max;
	cstat[CSTAT_ABSORB].value->set(288, offset_y+344, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (100 - stats->attunement_fire) << "%";
	cstat[CSTAT_FIRERESIST].value->set(288, offset_y+360, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);

	ss.str("");
	ss << (100 - stats->attunement_ice) << "%";
	cstat[CSTAT_ICERESIST].value->set(288, offset_y+376, JUSTIFY_CENTER, VALIGN_CENTER, ss.str(), FONT_WHITE);


	// update tool tips
	cstat[CSTAT_NAME].tip.num_lines = 0;
	cstat[CSTAT_NAME].tip.lines[cstat[CSTAT_NAME].tip.num_lines++] = msg->get(stats->character_class);

	cstat[CSTAT_LEVEL].tip.num_lines = 0;
	cstat[CSTAT_LEVEL].tip.lines[cstat[CSTAT_LEVEL].tip.num_lines++] = msg->get("xp", stats->xp);
	if (stats->level < MAX_CHARACTER_LEVEL) {
		cstat[CSTAT_LEVEL].tip.lines[cstat[CSTAT_LEVEL].tip.num_lines++] = msg->get("next", stats->xp_table[stats->level]);
	}

	cstat[CSTAT_PHYSICAL].tip.num_lines = 0;
	cstat[CSTAT_PHYSICAL].tip.lines[cstat[CSTAT_PHYSICAL].tip.num_lines++] = msg->get("physical_description");
	cstat[CSTAT_PHYSICAL].tip.lines[cstat[CSTAT_PHYSICAL].tip.num_lines++] = msg->get("stat_display", stats->physical_character, stats->physical_additional);

	cstat[CSTAT_MENTAL].tip.num_lines = 0;
	cstat[CSTAT_MENTAL].tip.lines[cstat[CSTAT_MENTAL].tip.num_lines++] = msg->get("mental_description");
	cstat[CSTAT_MENTAL].tip.lines[cstat[CSTAT_MENTAL].tip.num_lines++] = msg->get("stat_display", stats->mental_character, stats->mental_additional);

	cstat[CSTAT_OFFENSE].tip.num_lines = 0;
	cstat[CSTAT_OFFENSE].tip.lines[cstat[CSTAT_OFFENSE].tip.num_lines++] = msg->get("offense_description");
	cstat[CSTAT_OFFENSE].tip.lines[cstat[CSTAT_OFFENSE].tip.num_lines++] = msg->get("stat_display", stats->offense_character, stats->offense_additional);

	cstat[CSTAT_DEFENSE].tip.num_lines = 0;
	cstat[CSTAT_DEFENSE].tip.lines[cstat[CSTAT_DEFENSE].tip.num_lines++] = msg->get("defense_description");
	cstat[CSTAT_DEFENSE].tip.lines[cstat[CSTAT_DEFENSE].tip.num_lines++] = msg->get("stat_display", stats->defense_character, stats->defense_additional);
	
	cstat[CSTAT_HP].tip.num_lines = 0;
	cstat[CSTAT_HP].tip.lines[cstat[CSTAT_HP].tip.num_lines++] = msg->get("physical_hp_bonus");
	cstat[CSTAT_HP].tip.lines[cstat[CSTAT_HP].tip.num_lines++] = msg->get("level_hp_bonus");

	cstat[CSTAT_HPREGEN].tip.num_lines = 0;
	cstat[CSTAT_HPREGEN].tip.lines[cstat[CSTAT_HPREGEN].tip.num_lines++] = msg->get("hp_regen_description");
	cstat[CSTAT_HPREGEN].tip.lines[cstat[CSTAT_HPREGEN].tip.num_lines++] = msg->get("physical_hp_regen_bonus");
	cstat[CSTAT_HPREGEN].tip.lines[cstat[CSTAT_HPREGEN].tip.num_lines++] = msg->get("level_hp_regen_bonus");

	cstat[CSTAT_MP].tip.num_lines = 0;
	cstat[CSTAT_MP].tip.lines[cstat[CSTAT_MP].tip.num_lines++] = msg->get("mental_mp_bonus");
	cstat[CSTAT_MP].tip.lines[cstat[CSTAT_MP].tip.num_lines++] = msg->get("level_mp_bonus");

	cstat[CSTAT_MPREGEN].tip.num_lines = 0;
	cstat[CSTAT_MPREGEN].tip.lines[cstat[CSTAT_MPREGEN].tip.num_lines++] = msg->get("mp_regen_description");
	cstat[CSTAT_MPREGEN].tip.lines[cstat[CSTAT_MPREGEN].tip.num_lines++] = msg->get("mental_mp_regen_bonus");
	cstat[CSTAT_MPREGEN].tip.lines[cstat[CSTAT_MPREGEN].tip.num_lines++] = msg->get("level_mp_regen_bonus");

	cstat[CSTAT_ACCURACYV1].tip.num_lines = 0;
	cstat[CSTAT_ACCURACYV1].tip.lines[cstat[CSTAT_ACCURACYV1].tip.num_lines++] = msg->get("offense_accuracy_bonus");
	cstat[CSTAT_ACCURACYV1].tip.lines[cstat[CSTAT_ACCURACYV1].tip.num_lines++] = msg->get("level_accuracy_bonus");

	cstat[CSTAT_ACCURACYV5].tip.num_lines = 0;
	cstat[CSTAT_ACCURACYV5].tip.lines[cstat[CSTAT_ACCURACYV5].tip.num_lines++] = msg->get("offense_accuracy_bonus");
	cstat[CSTAT_ACCURACYV5].tip.lines[cstat[CSTAT_ACCURACYV5].tip.num_lines++] = msg->get("level_accuracy_bonus");

	cstat[CSTAT_AVOIDANCEV1].tip.num_lines = 0;
	cstat[CSTAT_AVOIDANCEV1].tip.lines[cstat[CSTAT_AVOIDANCEV1].tip.num_lines++] = msg->get("defense_avoidance_bonus");
	cstat[CSTAT_AVOIDANCEV1].tip.lines[cstat[CSTAT_AVOIDANCEV1].tip.num_lines++] = msg->get("level_avoidance_bonus");

	cstat[CSTAT_AVOIDANCEV5].tip.num_lines = 0;
	cstat[CSTAT_AVOIDANCEV5].tip.lines[cstat[CSTAT_AVOIDANCEV5].tip.num_lines++] = msg->get("defense_avoidance_bonus");
	cstat[CSTAT_AVOIDANCEV5].tip.lines[cstat[CSTAT_AVOIDANCEV5].tip.num_lines++] = msg->get("level_avoidance_bonus");
	
	// proficiency tooltips
	cprof[CPROF_P2].tip.num_lines = 0;
	cprof[CPROF_P2].tip.lines[cprof[CPROF_P2].tip.num_lines++] = msg->get("physical_2_proficiency");
	if (stats->get_physical() < 2) cprof[CPROF_P2].tip.lines[cprof[CPROF_P2].tip.num_lines] = FONT_RED;
	cprof[CPROF_P2].tip.lines[cprof[CPROF_P2].tip.num_lines++] = msg->get("requires_physical", 2);

	cprof[CPROF_P3].tip.num_lines = 0;
	cprof[CPROF_P3].tip.lines[cprof[CPROF_P3].tip.num_lines++] = msg->get("physical_3_proficiency");
	if (stats->get_physical() < 3) cprof[CPROF_P3].tip.lines[cprof[CPROF_P3].tip.num_lines] = FONT_RED;
	cprof[CPROF_P3].tip.lines[cprof[CPROF_P3].tip.num_lines++] = msg->get("requires_physical", 3);

	cprof[CPROF_P4].tip.num_lines = 0;
	cprof[CPROF_P4].tip.lines[cprof[CPROF_P4].tip.num_lines++] = msg->get("physical_4_proficiency");
	if (stats->get_physical() < 4) cprof[CPROF_P4].tip.lines[cprof[CPROF_P4].tip.num_lines] = FONT_RED;
	cprof[CPROF_P4].tip.lines[cprof[CPROF_P4].tip.num_lines++] = msg->get("requires_physical", 4);

	cprof[CPROF_P5].tip.num_lines = 0;
	cprof[CPROF_P5].tip.lines[cprof[CPROF_P5].tip.num_lines++] = msg->get("physical_5_proficiency");
	if (stats->get_physical() < 5) cprof[CPROF_P5].tip.lines[cprof[CPROF_P5].tip.num_lines] = FONT_RED;
	cprof[CPROF_P5].tip.lines[cprof[CPROF_P5].tip.num_lines++] = msg->get("requires_physical", 5);

	cprof[CPROF_M2].tip.num_lines = 0;
	cprof[CPROF_M2].tip.lines[cprof[CPROF_M2].tip.num_lines++] = msg->get("mental_2_proficiency");
	if (stats->get_mental() < 2) cprof[CPROF_M2].tip.lines[cprof[CPROF_M2].tip.num_lines] = FONT_RED;
	cprof[CPROF_M2].tip.lines[cprof[CPROF_M2].tip.num_lines++] = msg->get("requires_mental", 2);

	cprof[CPROF_M3].tip.num_lines = 0;
	cprof[CPROF_M3].tip.lines[cprof[CPROF_M3].tip.num_lines++] = msg->get("mental_3_proficiency");
	if (stats->get_mental() < 3) cprof[CPROF_M3].tip.lines[cprof[CPROF_M3].tip.num_lines] = FONT_RED;
	cprof[CPROF_M3].tip.lines[cprof[CPROF_M3].tip.num_lines++] = msg->get("requires_mental", 3);

	cprof[CPROF_M4].tip.num_lines = 0;
	cprof[CPROF_M4].tip.lines[cprof[CPROF_M4].tip.num_lines++] = msg->get("mental_4_proficiency");
	if (stats->get_mental() < 4) cprof[CPROF_M4].tip.lines[cprof[CPROF_M4].tip.num_lines] = FONT_RED;
	cprof[CPROF_M4].tip.lines[cprof[CPROF_M4].tip.num_lines++] = msg->get("requires_mental", 4);

	cprof[CPROF_M5].tip.num_lines = 0;
	cprof[CPROF_M5].tip.lines[cprof[CPROF_M5].tip.num_lines++] = msg->get("mental_5_proficiency");
	if (stats->get_mental() < 5) cprof[CPROF_M5].tip.lines[cprof[CPROF_M5].tip.num_lines] = FONT_RED;
	cprof[CPROF_M5].tip.lines[cprof[CPROF_M5].tip.num_lines++] = msg->get("requires_mental", 5);
	
	cprof[CPROF_O2].tip.num_lines = 0;
	cprof[CPROF_O2].tip.lines[cprof[CPROF_O2].tip.num_lines++] = msg->get("offense_2_proficiency");
	if (stats->get_offense() < 2) cprof[CPROF_O2].tip.lines[cprof[CPROF_O2].tip.num_lines] = FONT_RED;
	cprof[CPROF_O2].tip.lines[cprof[CPROF_O2].tip.num_lines++] = msg->get("requires_offense", 2);

	cprof[CPROF_O3].tip.num_lines = 0;
	cprof[CPROF_O3].tip.lines[cprof[CPROF_O3].tip.num_lines++] = msg->get("offense_3_proficiency");
	if (stats->get_offense() < 3) cprof[CPROF_O3].tip.lines[cprof[CPROF_O3].tip.num_lines] = FONT_RED;
	cprof[CPROF_O3].tip.lines[cprof[CPROF_O3].tip.num_lines++] = msg->get("requires_offense", 3);

	cprof[CPROF_O4].tip.num_lines = 0;
	cprof[CPROF_O4].tip.lines[cprof[CPROF_O4].tip.num_lines++] = msg->get("offense_4_proficiency");
	if (stats->get_offense() < 4) cprof[CPROF_O4].tip.lines[cprof[CPROF_O4].tip.num_lines] = FONT_RED;
	cprof[CPROF_O4].tip.lines[cprof[CPROF_O4].tip.num_lines++] = msg->get("requires_offense", 4);

	cprof[CPROF_O5].tip.num_lines = 0;
	cprof[CPROF_O5].tip.lines[cprof[CPROF_O5].tip.num_lines++] = msg->get("offense_5_proficiency");
	if (stats->get_offense() < 5) cprof[CPROF_O5].tip.lines[cprof[CPROF_O5].tip.num_lines] = FONT_RED;
	cprof[CPROF_O5].tip.lines[cprof[CPROF_O5].tip.num_lines++] = msg->get("requires_offense", 5);

	cprof[CPROF_D2].tip.num_lines = 0;
	cprof[CPROF_D2].tip.lines[cprof[CPROF_D2].tip.num_lines++] = msg->get("defense_2_proficiency");
	if (stats->get_defense() < 2) cprof[CPROF_D2].tip.lines[cprof[CPROF_D2].tip.num_lines] = FONT_RED;
	cprof[CPROF_D2].tip.lines[cprof[CPROF_D2].tip.num_lines++] = msg->get("requires_defense", 2);

	cprof[CPROF_D3].tip.num_lines = 0;
	cprof[CPROF_D3].tip.lines[cprof[CPROF_D3].tip.num_lines++] = msg->get("defense_3_proficiency");
	if (stats->get_defense() < 3) cprof[CPROF_D3].tip.lines[cprof[CPROF_D3].tip.num_lines] = FONT_RED;
	cprof[CPROF_D3].tip.lines[cprof[CPROF_D3].tip.num_lines++] = msg->get("requires_defense", 3);

	cprof[CPROF_D4].tip.num_lines = 0;
	cprof[CPROF_D4].tip.lines[cprof[CPROF_D4].tip.num_lines++] = msg->get("defense_4_proficiency");
	if (stats->get_defense() < 4) cprof[CPROF_D4].tip.lines[cprof[CPROF_D4].tip.num_lines] = FONT_RED;
	cprof[CPROF_D4].tip.lines[cprof[CPROF_D4].tip.num_lines++] = msg->get("requires_defense", 4);

	cprof[CPROF_D5].tip.num_lines = 0;
	cprof[CPROF_D5].tip.lines[cprof[CPROF_D5].tip.num_lines++] = msg->get("defense_5_proficiency");
	if (stats->get_defense() < 5) cprof[CPROF_D5].tip.lines[cprof[CPROF_D5].tip.num_lines] = FONT_RED;
	cprof[CPROF_D5].tip.lines[cprof[CPROF_D5].tip.num_lines++] = msg->get("requires_defense", 5);
	
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
	int offset_y = (VIEW_H - 416)/2;
	
	// background
	src.x = 0;
	src.y = 0;
	dest.x = 0;
	dest.y = offset_y;
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
	displayProficiencies(stats->get_physical(), offset_y+64);
	displayProficiencies(stats->get_mental(), offset_y+128);
	displayProficiencies(stats->get_offense(), offset_y+192);
	displayProficiencies(stats->get_defense(), offset_y+256);
	
	// if points are available, show the upgrade buttons
	// TODO: replace with WidgetButton
	
	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	int max_spendable_stat_points = 16;
	
	// check to see if there are skill points available
	if (spent < stats->level && spent < max_spendable_stat_points) {

		src.x = 0;
		src.y = 0;
		src.w = dest.w = 32;
		src.h = dest.h = 16;
		dest.x = 16;

		// physical
		if (stats->physical_character < 5) { // && mouse.x >= 16 && mouse.y >= offset_y+96
			dest.y = offset_y + 96;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}
		// mental
		if (stats->mental_character < 5) { // && mouse.x >= 16 && mouse.y >= offset_y+160
			dest.y = offset_y + 160;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}
		// offense
		if (stats->offense_character < 5) { // && mouse.x >= 16 && mouse.y >= offset_y+224
			dest.y = offset_y + 224;
			SDL_BlitSurface(upgrade, &src, screen, &dest);
		}
		// defense
		if (stats->defense_character < 5) { // && mouse.x >= 16 && mouse.y >= offset_y+288
			dest.y = offset_y + 288;
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
		dest.x = 112 + (i-2) * 48;
		SDL_BlitSurface(proficiency, &src, screen, &dest);
	}
}

/**
 * Display various mouseovers tooltips depending on cursor location
 */
TooltipData MenuCharacter::checkTooltip() {

	for (int i=0; i<CSTAT_COUNT; i++) {
		if (isWithin(cstat[i].hover, inp->mouse) && cstat[i].tip.num_lines > 0)
			return cstat[i].tip;
	}

	for (int i=0; i<CPROF_COUNT; i++) {
		if (isWithin(cprof[i].hover, inp->mouse) && cprof[i].tip.num_lines > 0)
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
	mouse.x = inp->mouse.x;
	mouse.y = inp->mouse.y;

	int spent = stats->physical_character + stats->mental_character + stats->offense_character + stats->defense_character -4;
	int max_spendable_stat_points = 16;
	
	// check to see if there are skill points available
	if (spent < stats->level && spent < max_spendable_stat_points) {
		
		// check mouse hotspots
		int offset_y = (VIEW_H - 416)/2;
		
		// physical
		if (stats->physical_character < 5 && mouse.x >= 16 && mouse.x <= 48 && mouse.y >= offset_y+96 && mouse.y <= offset_y+112) {
			stats->physical_character++;
			stats->recalc(); // equipment applied by MenuManager
			return true;
		}
		// mental
		else if (stats->mental_character < 5 && mouse.x >= 16 && mouse.x <= 48 && mouse.y >= offset_y+160 && mouse.y <= offset_y+176) {
			stats->mental_character++;
			stats->recalc(); // equipment applied by MenuManager
			return true;		
		}
		// offense
		else if (stats->offense_character < 5 && mouse.x >= 16 && mouse.x <= 48 && mouse.y >= offset_y+224 && mouse.y <= offset_y+240) {
			stats->offense_character++;
			stats->recalc(); // equipment applied by MenuManager
			return true;		
		}
		// defense
		else if (stats->defense_character < 5 && mouse.x >= 16 && mouse.x <= 48 && mouse.y >= offset_y+288 && mouse.y <= offset_y+304) {
			stats->defense_character++;
			stats->recalc(); // equipment applied by MenuManager
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
