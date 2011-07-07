/**
 * class MenuCharacter
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "MenuCharacter.h"

MenuCharacter::MenuCharacter(SDL_Surface *_screen, InputState *_inp, FontEngine *_font, StatBlock *_stats) {
	screen = _screen;
	inp = _inp;
	font = _font;
	stats = _stats;
	
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
	// TODO: translate()
	font->render("Character", 160, offset_y+8, JUSTIFY_CENTER, screen, FONT_WHITE);
	font->render("Name", 72, offset_y+34, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Level", 248, offset_y+34, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Physical", 40, offset_y+74, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render("Mental", 40, offset_y+138, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render("Offense", 40, offset_y+202, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render("Defense", 40, offset_y+266, JUSTIFY_LEFT, screen, FONT_WHITE);
	font->render("Total HP", 152, offset_y+106, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Regen", 248, offset_y+106, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Total MP", 152, offset_y+170, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Regen", 248, offset_y+170, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Accuracy vs. Def 1", 152, offset_y+234, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("vs. Def 5", 248, offset_y+234, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Avoidance vs. Off 1", 152, offset_y+298, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("vs. Off 5", 248, offset_y+298, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Main Weapon", 120, offset_y+338, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Ranged Weapon", 120, offset_y+354, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Crit Chance", 120, offset_y+370, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Absorb", 248, offset_y+338, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Fire Resist", 248, offset_y+354, JUSTIFY_RIGHT, screen, FONT_WHITE);
	font->render("Ice Resist", 248, offset_y+370, JUSTIFY_RIGHT, screen, FONT_WHITE);

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
	
	// check to see if there are skill points available
	if (spent < stats->level-1) {

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
	stringstream ss;

	if (mouse.x >= 256 && mouse.x <= 280 && mouse.y >= offset_y+32 && mouse.y <= offset_y+48) {
		ss << "XP: " << stats->xp;
		tip.lines[tip.num_lines++] = ss.str();
		ss.str("");
		if (stats->level < 17) {
			ss << "Next: " << stats->xp_table[stats->level];
			tip.lines[tip.num_lines++] = ss.str();
		}
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+72 && mouse.y <= offset_y+88) {
		tip.lines[tip.num_lines++] = "Physical (P) increases melee weapon proficiency and total HP.";
		ss.str("");
		ss << "base (" << stats->physical_character << "), bonus (" << stats->physical_additional << ")";
		tip.lines[tip.num_lines++] = ss.str();
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+136 && mouse.y <= offset_y+152) {
		tip.lines[tip.num_lines++] = "Mental (M) increases mental weapon proficiency and total MP.";
		ss.str("");
		ss << "base (" << stats->mental_character << "), bonus (" << stats->mental_additional << ")";
		tip.lines[tip.num_lines++] = ss.str();
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+200 && mouse.y <= offset_y+216) {
		tip.lines[tip.num_lines++] = "Offense (O) increases ranged weapon proficiency and accuracy.";
		ss.str("");
		ss << "base (" << stats->offense_character << "), bonus (" << stats->offense_additional << ")";
		tip.lines[tip.num_lines++] = ss.str();
		return tip;
	}
	if (mouse.x >= 16 && mouse.x <= 80 && mouse.y >= offset_y+264 && mouse.y <= offset_y+280) {
		tip.lines[tip.num_lines++] = "Defense (D) increases armor proficiency and avoidance.";
		ss.str("");
		ss << "base (" << stats->defense_character << "), bonus (" << stats->defense_additional << ")";
		tip.lines[tip.num_lines++] = ss.str();
		return tip;
	}

	// Physical
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = "Dagger Proficiency";
		if (stats->get_physical() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Physical 2";
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = "Shortsword Proficiency";
		if (stats->get_physical() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Physical 3";
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = "Longsword Proficiency";
		if (stats->get_physical() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Physical 4";
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+64 && mouse.y <= offset_y+96) {
		tip.lines[tip.num_lines++] = "Greatsword Proficiency";
		if (stats->get_physical() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Physical 5";
		return tip;
	}
	if (mouse.x >= 64 && mouse.x <= 184 && mouse.y >= offset_y+104 && mouse.y <= offset_y+120) {
		tip.lines[tip.num_lines++] = "Each point of Physical grants +8 HP";
		tip.lines[tip.num_lines++] = "Each level grants +2 HP";
		return tip;
	}
	if (mouse.x >= 208 && mouse.x <= 280 && mouse.y >= offset_y+104 && mouse.y <= offset_y+120) {
		tip.lines[tip.num_lines++] = "Ticks of HP regen per minute";
		tip.lines[tip.num_lines++] = "Each point of Physical grants +4 HP regen";
		tip.lines[tip.num_lines++] = "Each level grants +1 HP regen";
		return tip;
	}

		
	// Mental
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = "Wand Proficiency";
		if (stats->get_mental() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Mental 2";
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = "Rod Proficiency";
		if (stats->get_mental() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Mental 3";
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = "Staff Proficiency";
		if (stats->get_mental() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Mental 4";
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+128 && mouse.y <= offset_y+160) {
		tip.lines[tip.num_lines++] = "Greatstaff Proficiency";
		if (stats->get_mental() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Mental 5";
		return tip;
	}		
	if (mouse.x >= 64 && mouse.x <= 184 && mouse.y >= offset_y+168 && mouse.y <= offset_y+184) {
		tip.lines[tip.num_lines++] = "Each point of Mental grants +8 MP";
		tip.lines[tip.num_lines++] = "Each level grants +2 MP";		
		return tip;
	}
	if (mouse.x >= 208 && mouse.x <= 280 && mouse.y >= offset_y+168 && mouse.y <= offset_y+184) {
		tip.lines[tip.num_lines++] = "Ticks of MP regen per minute";
		tip.lines[tip.num_lines++] = "Each point of Mental grants +4 MP regen";
		tip.lines[tip.num_lines++] = "Each level grants +1 MP regen";		
		return tip;
	}
		
		
	// Offense
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = "Slingshot Proficiency";
		if (stats->get_offense() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Offense 2";
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = "Shortbow Proficiency";
		if (stats->get_offense() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Offense 3";
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = "Longbow Proficiency";
		if (stats->get_offense() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Offense 4";
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+192 && mouse.y <= offset_y+224) {
		tip.lines[tip.num_lines++] = "Greatbow Proficiency";
		if (stats->get_offense() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Offense 5";
		return tip;
	}
	if (mouse.x >= 64 && mouse.x <= 280 && mouse.y >= offset_y+232 && mouse.y <= offset_y+248) {
		tip.lines[tip.num_lines++] = "Each point of Offense grants +5 accuracy";
		tip.lines[tip.num_lines++] = "Each level grants +2 accuracy";
		return tip;
	}
		
		
	// Defense
	if (mouse.x >= 128 && mouse.x <= 160 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = "Light Armor Proficiency";
		if (stats->get_defense() < 2) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Defense 2";
		return tip;
	}
	if (mouse.x >= 176 && mouse.x <= 208 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = "Light Shield Proficiency";
		if (stats->get_defense() < 3) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Defense 3";
		return tip;
	}
	if (mouse.x >= 224 && mouse.x <= 256 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = "Heavy Armor Proficiency";
		if (stats->get_defense() < 4) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Defense 4";
		return tip;
	}
	if (mouse.x >= 272 && mouse.x <= 304 && mouse.y >= offset_y+256 && mouse.y <= offset_y+288) {
		tip.lines[tip.num_lines++] = "Heavy Shield Proficiency";
		if (stats->get_defense() < 5) tip.colors[tip.num_lines] = FONT_RED;
		tip.lines[tip.num_lines++] = "Requires Defense 5";
		return tip;
	}		
	if (mouse.x >= 64 && mouse.x <= 280 && mouse.y >= offset_y+296 && mouse.y <= offset_y+312) {
		tip.lines[tip.num_lines++] = "Each point of Defense grants +5 avoidance";
		tip.lines[tip.num_lines++] = "Each level grants +2 avoidance";
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
	
	// check to see if there are skill points available
	if (spent < stats->level-1) {
		
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
