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
	
	closeButton = new WidgetButton(screen, font, inp, "images/menus/buttons/button_x.png");
	closeButton->pos.x = 294;
	closeButton->pos.y = (VIEW_H - 480)/2 + 34;

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
	
	// labels
	font->render(msg->get("character"), 160, offset_y+8, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render(msg->get("name"), 72, offset_y+34, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("level"), 248, offset_y+34, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("physical"), 40, offset_y+74, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render(msg->get("mental"), 40, offset_y+138, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render(msg->get("offense"), 40, offset_y+202, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render(msg->get("defense"), 40, offset_y+266, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render(msg->get("total_hp"), 152, offset_y+106, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("regen"), 248, offset_y+106, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("total_mp"), 152, offset_y+170, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("regen"), 248, offset_y+170, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("accuracy_vs_def_1"), 152, offset_y+234, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("vs_def_5"), 248, offset_y+234, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("avoidance_vs_off_1"), 152, offset_y+298, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("vs_off_5"), 248, offset_y+298, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("main_weapon"), 120, offset_y+338, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("ranged_weapon"), 120, offset_y+354, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("crit_chance"), 120, offset_y+370, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("absorb"), 248, offset_y+338, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("fire_resist"), 248, offset_y+354, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render(msg->get("ice_resist"), 248, offset_y+370, JUSTIFY_RIGHT, screen, FONT_WHITE);

	// character data
	stringstream ss;
	font->render(stats->name, 83, offset_y+34, JUSTIFY_LEFT, screen, FONT_WHITE);
	ss.str("");
	ss << stats->level;
	font->render(ss.str(), 268, offset_y+34, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	ss.str("");
	ss << stats->get_physical();
	font->render(ss.str(), 24, offset_y+74, JUSTIFY_CENTER, screen, bonusColor(stats->physical_additional));
	ss.str("");
	ss << stats->get_mental();
	font->render(ss.str(), 24, offset_y+138, JUSTIFY_CENTER, screen, bonusColor(stats->mental_additional));
	ss.str("");
	ss << stats->get_offense();
	font->render(ss.str(), 24, offset_y+202, JUSTIFY_CENTER, screen, bonusColor(stats->offense_additional));
	ss.str("");
	ss << stats->get_defense();
	font->render(ss.str(), 24, offset_y+266, JUSTIFY_CENTER, screen, bonusColor(stats->defense_additional));
	
	ss.str("");
	ss << stats->maxhp;
	font->render(ss.str(), 172, offset_y+106, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->hp_per_minute;
	font->render(ss.str(), 268, offset_y+106, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->maxmp;
	font->render(ss.str(), 172, offset_y+170, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->mp_per_minute;
	font->render(ss.str(), 268, offset_y+170, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << (stats->accuracy) << "%";
	font->render(ss.str(), 172, offset_y+234, JUSTIFY_CENTER, screen, FONT_WHITE);	
	ss.str("");
	ss << (stats->accuracy - 20) << "%";
	font->render(ss.str(), 268, offset_y+234, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << (stats->avoidance) << "%";
	font->render(ss.str(), 172, offset_y+298, JUSTIFY_CENTER, screen, FONT_WHITE);	
	ss.str("");
	ss << (stats->avoidance - 20) << "%";
	font->render(ss.str(), 268, offset_y+298, JUSTIFY_CENTER, screen, FONT_WHITE);	
	ss.str("");
	if (stats->dmg_melee_max >= stats->dmg_ment_max)
		ss << stats->dmg_melee_min << "-" << stats->dmg_melee_max;
	else
		ss << stats->dmg_ment_min << "-" << stats->dmg_ment_max;
	font->render(ss.str(), 144, offset_y+338, JUSTIFY_CENTER, screen, FONT_WHITE);	
	ss.str("");
	if (stats->dmg_ranged_max > 0)
		ss << stats->dmg_ranged_min << "-" << stats->dmg_ranged_max;
	else
		ss << "-";
	font->render(ss.str(), 144, offset_y+354, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << stats->crit << "%";
	font->render(ss.str(), 144, offset_y+370, JUSTIFY_CENTER, screen, FONT_WHITE);	
	ss.str("");
	if (stats->absorb_min == stats->absorb_max)
		ss << stats->absorb_min;
	else
		ss << stats->absorb_min << "-" << stats->absorb_max;
	font->render(ss.str(), 272, offset_y+338, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << (100 - stats->attunement_fire) << "%";
	font->render(ss.str(), 272, offset_y+354, JUSTIFY_CENTER, screen, FONT_WHITE);
	ss.str("");
	ss << (100 - stats->attunement_ice) << "%";
	font->render(ss.str(), 272, offset_y+370, JUSTIFY_CENTER, screen, FONT_WHITE);
	
	// highlight proficiencies
	displayProficiencies(stats->get_physical(), offset_y+64);
	displayProficiencies(stats->get_mental(), offset_y+128);
	displayProficiencies(stats->get_offense(), offset_y+192);
	displayProficiencies(stats->get_defense(), offset_y+256);
	
	
	// if points are available, show the upgrade buttons
	
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

	TooltipData tip;
	Point mouse;
	mouse.x = inp->mouse.x;
	mouse.y = inp->mouse.y;
	
	int offset_y = (VIEW_H - 416)/2;

	if (mouse.x >= 256 && mouse.x <= 280 && mouse.y >= offset_y+32 && mouse.y <= offset_y+48) {
		tip.lines[tip.num_lines++] = msg->get("xp", stats->xp);
		if (stats->level < MAX_CHARACTER_LEVEL) {
			tip.lines[tip.num_lines++] = msg->get("next", stats->xp_table[stats->level]);
		}
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+72 && mouse.y <= offset_y+88) {
		tip.lines[tip.num_lines++] = msg->get("physical_description");
		tip.lines[tip.num_lines++] = msg->get("stat_display", stats->physical_character, stats->physical_additional);
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+136 && mouse.y <= offset_y+152) {
		tip.lines[tip.num_lines++] = msg->get("mental_description");
		tip.lines[tip.num_lines++] = msg->get("stat_display", stats->mental_character, stats->mental_additional);
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+200 && mouse.y <= offset_y+216) {
		tip.lines[tip.num_lines++] = msg->get("offense_description");
		tip.lines[tip.num_lines++] = msg->get("stat_display", stats->offense_character, stats->offense_additional);
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+264 && mouse.y <= offset_y+280) {
		tip.lines[tip.num_lines++] = msg->get("defense_description");
		tip.lines[tip.num_lines++] = msg->get("stat_display", stats->defense_character, stats->defense_additional);
		return tip;
	}

	// Physical
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = msg->get("physical_2_proficiency");
		if (stats->get_physical() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_physical", 2);
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = msg->get("physical_3_proficiency");
		if (stats->get_physical() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_physical", 3);
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = msg->get("physical_4_proficiency");
		if (stats->get_physical() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_physical", 4);
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = msg->get("physical_5_proficiency");
		if (stats->get_physical() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_physical", 5);
		return tip;
	}
	if (mouse.x >= 64 && mouse.x <= 184 && mouse.y >= offset_y+104 && mouse.y <= offset_y+120) {
		tip.lines[tip.num_lines++] = msg->get("physical_hp_bonus");
		tip.lines[tip.num_lines++] = msg->get("level_hp_bonus");
		return tip;
	}
	if (mouse.x >= 208 && mouse.x <= 280 && mouse.y >= offset_y+104 && mouse.y <= offset_y+120) {
		tip.lines[tip.num_lines++] = msg->get("hp_regen_description");
		tip.lines[tip.num_lines++] = msg->get("physical_hp_regen_bonus");
		tip.lines[tip.num_lines++] = msg->get("level_hp_regen_bonus");
		return tip;
	}

		
	// Mental
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = msg->get("mental_2_proficiency");
		if (stats->get_mental() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_mental", 2);
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = msg->get("mental_3_proficiency");
		if (stats->get_mental() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_mental", 3);
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = msg->get("mental_4_proficiency");
		if (stats->get_mental() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_mental", 4);
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = msg->get("mental_5_proficiency");
		if (stats->get_mental() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_mental", 5);
		return tip;
	}		
	if (mouse.x >= 64 && mouse.x <= 184 && mouse.y >= offset_y+168 && mouse.y <= offset_y+184) {
		tip.lines[tip.num_lines++] = msg->get("mental_mp_bonus");
		tip.lines[tip.num_lines++] = msg->get("level_mp_bonus");
		return tip;
	}
	if (mouse.x >= 208 && mouse.x <= 280 && mouse.y >= offset_y+168 && mouse.y <= offset_y+184) {
		tip.lines[tip.num_lines++] = msg->get("mp_regen_description");
		tip.lines[tip.num_lines++] = msg->get("mental_mp_regen_bonus");
		tip.lines[tip.num_lines++] = msg->get("level_mp_regen_bonus");
		return tip;
	}
		
		
	// Offense
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = msg->get("offense_2_proficiency");
		if (stats->get_offense() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_offense", 2);
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = msg->get("offense_3_proficiency");
		if (stats->get_offense() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_offense", 3);
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = msg->get("offense_4_proficiency");
		if (stats->get_offense() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_offense", 4);
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = msg->get("offense_5_proficiency");
		if (stats->get_offense() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_offense", 5);
		return tip;
	}
	if (mouse.x >= 64 && mouse.x <= 280 && mouse.y >= offset_y+232 && mouse.y <= offset_y+248) {
		tip.lines[tip.num_lines++] = msg->get("offense_accuracy_bonus");
		tip.lines[tip.num_lines++] = msg->get("level_accuracy_bonus");
		return tip;
	}
		
		
	// Defense
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = msg->get("defense_2_proficiency");
		if (stats->get_defense() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_defense", 2);
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = msg->get("defense_3_proficiency");
		if (stats->get_defense() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_defense", 3);
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = msg->get("defense_4_proficiency");
		if (stats->get_defense() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_defense", 4);
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = msg->get("defense_5_proficiency");
		if (stats->get_defense() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = msg->get("requires_defense", 5);
		return tip;
	}		
	if (mouse.x >= 64 && mouse.x <= 280 && mouse.y >= offset_y+296 && mouse.y <= offset_y+312) {
		tip.lines[tip.num_lines++] = msg->get("defense_avoidance_bonus");
		tip.lines[tip.num_lines++] = msg->get("level_avoidance_bonus");
		return tip;
	}

	
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
}
