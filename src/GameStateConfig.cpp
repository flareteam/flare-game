/*
Copyright © 2012 Clint Bellanger
Copyright © 2012 davidriod
Copyright © 2012 Igor Paliychuk

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
 * GameStateConfig
 * 
 * Handle game Settings Menu
 */

#include "GameStateConfig.h"
#include "GameStateTitle.h"
#include "SharedResources.h"
#include "Settings.h"
#include <sstream>


GameStateConfig::GameStateConfig ()
		: GameState(),
		  child_widget(),
		  ok_button(NULL),
		  cancel_button(NULL)

{
	// Initialize Widgets
	tabControl = new WidgetTabControl(4);
	ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	for (unsigned int i = 0; i < 36; i++) {
		 settings_lb[i] = new WidgetLabel();
	}

	for (unsigned int i = 0; i < 2; i++) {
		 settings_sl[i] = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
	}

	for (unsigned int i = 0; i < 6; i++) {
		 settings_cb[i] = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	}

	for (unsigned int i = 0; i < 25; i++) {
		 settings_key[i] = new WidgetInput();
	}

	// Allocate Joycticks ComboBox
	settings_cmb[0] = new WidgetComboBox(5, mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate Resolution ComboBox
	int resolutions = getVideoModes();
	if (resolutions < 1) fprintf(stderr, "Unable to get resolutions list!\n");

	settings_cmb[1] = new WidgetComboBox(resolutions, mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate Languages ComboBox
	int langCount = getLanguagesNumber();
	language_ISO = new std::string[langCount];
	language_full = new std::string[langCount];
	settings_cmb[2] = new WidgetComboBox(langCount, mods->locate("images/menus/buttons/combobox_default.png"));

	//Load the menu configuration from file
	int x1;
	int y1;
	int x2;
	int y2;
	int setting_num;

	FileParser infile;
	if (infile.open(mods->locate("menus/config.txt"))) {
		while (infile.next()) {

			infile.val = infile.val + ',';
			x1 = eatFirstInt(infile.val, ',');
			y1 = eatFirstInt(infile.val, ',');
			x2 = eatFirstInt(infile.val, ',');
			y2 = eatFirstInt(infile.val, ',');

			setting_num = -1;

			if (infile.key == "fullscreen") setting_num = 1;
			else if (infile.key == "mouse_move") setting_num = 2;
			else if (infile.key == "combat_text") setting_num = 3;
			else if (infile.key == "hwsurface") setting_num = 4;
			else if (infile.key == "doublebuf") setting_num = 5;
			else if (infile.key == "enable_joystick") setting_num = 6;
			else if (infile.key == "music_volume") setting_num = 7;
			else if (infile.key == "sound_volume") setting_num = 8;
			else if (infile.key == "joystick_device") setting_num = 9;
			else if (infile.key == "resolution") setting_num = 10;
			else if (infile.key == "language")setting_num = 11;

			else if (infile.key == "cancel") setting_num = 12 + CANCEL;
			else if (infile.key == "accept") setting_num = 12 + ACCEPT;
			else if (infile.key == "up") setting_num = 12 + UP;
			else if (infile.key == "down") setting_num = 12 + DOWN;
			else if (infile.key == "left") setting_num = 12 + LEFT;
			else if (infile.key == "right") setting_num = 12 + RIGHT;
			else if (infile.key == "bar1") setting_num = 12 + BAR_1;
			else if (infile.key == "bar2") setting_num = 12 + BAR_2;
			else if (infile.key == "bar3") setting_num = 12 + BAR_3;
			else if (infile.key == "bar4") setting_num = 12 + BAR_4;
			else if (infile.key == "bar5") setting_num = 12 + BAR_5;
			else if (infile.key == "bar6") setting_num = 12 + BAR_6;
			else if (infile.key == "bar7") setting_num = 12 + BAR_7;
			else if (infile.key == "bar8") setting_num = 12 + BAR_8;
			else if (infile.key == "bar9") setting_num = 12 + BAR_9;
			else if (infile.key == "bar0") setting_num = 12 + BAR_0;
			else if (infile.key == "main1") setting_num = 12 + MAIN1;
			else if (infile.key == "main2") setting_num = 12 + MAIN2;
			else if (infile.key == "character") setting_num = 12 + CHARACTER;
			else if (infile.key == "inventory") setting_num = 12 + INVENTORY;
			else if (infile.key == "powers") setting_num = 12 + POWERS;
			else if (infile.key == "log") setting_num = 12 + LOG;
			else if (infile.key == "ctrl") setting_num = 12 + CTRL;
			else if (infile.key == "shift") setting_num = 12 + SHIFT;
			else if (infile.key == "delete") setting_num = 12 + DEL;

			if (setting_num != -1) {
					settings_lb[setting_num-1]->setX((VIEW_W - 640)/2 + x1);
					settings_lb[setting_num-1]->setY((VIEW_H - 480)/2 + y1);

				if (setting_num < 7) {
					settings_cb[setting_num-1]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_cb[setting_num-1]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if ((setting_num > 6) && (setting_num < 9)) {
					settings_sl[setting_num-7]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_sl[setting_num-7]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if ((setting_num > 8) && (setting_num < 12)) {
					settings_cmb[setting_num-9]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_cmb[setting_num-9]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if (setting_num > 11) {
					settings_key[setting_num-12]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_key[setting_num-12]->pos.y = (VIEW_H - 480)/2 + y2;
				}
			}

		  }
		} else fprintf(stderr, "Unable to open config.txt!\n");
		infile.close();

	// Initialize the tab control.
	tabControl->setMainArea((VIEW_W - 640)/2, (VIEW_H - 480)/2, 640, 480);

	// Define the header.
	tabControl->setTabTitle(0, msg->get("Video"));
	tabControl->setTabTitle(1, msg->get("Audio"));
	tabControl->setTabTitle(2, msg->get("Interface"));
	tabControl->setTabTitle(3, msg->get("Input"));
	tabControl->updateHeader();

	// Define widgets
	ok_button->label = msg->get("Ok");
	ok_button->pos.x = VIEW_W_HALF - ok_button->pos.w/2;
	ok_button->pos.y = VIEW_H - (cancel_button->pos.h*2);
	ok_button->refresh();
	child_widget.push_back(ok_button);

	cancel_button->label = msg->get("Cancel");
	cancel_button->pos.x = VIEW_W_HALF - cancel_button->pos.w/2;
	cancel_button->pos.y = VIEW_H - (cancel_button->pos.h);
	cancel_button->refresh();
	child_widget.push_back(cancel_button);

	settings_lb[0]->set(msg->get("Full Screen Mode"));
	child_widget.push_back(settings_lb[0]);
	optiontab[child_widget.size()-1] = 0;

	if (FULLSCREEN == true) settings_cb[0]->Check();
	child_widget.push_back(settings_cb[0]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[6]->set(msg->get("Music Volume"));
	child_widget.push_back(settings_lb[6]);
	optiontab[child_widget.size()-1] = 1;

	settings_sl[0]->set(0,128,MUSIC_VOLUME);
	child_widget.push_back(settings_sl[0]);
	optiontab[child_widget.size()-1] = 1;

	settings_lb[7]->set(msg->get("Sound Volume"));
	child_widget.push_back(settings_lb[7]);
	optiontab[child_widget.size()-1] = 1;

	settings_sl[1]->set(0,128,SOUND_VOLUME);
	child_widget.push_back(settings_sl[1]);
	optiontab[child_widget.size()-1] = 1;

	settings_lb[1]->set(msg->get("Move hero using mouse"));
	child_widget.push_back(settings_lb[1]);
	optiontab[child_widget.size()-1] = 3;

	if (MOUSE_MOVE == true) settings_cb[1]->Check();
	child_widget.push_back(settings_cb[1]);
	optiontab[child_widget.size()-1] = 3;

	settings_lb[2]->set(msg->get("Show combat text"));
	child_widget.push_back(settings_lb[2]);
	optiontab[child_widget.size()-1] = 2;

	if (COMBAT_TEXT == true) settings_cb[2]->Check();
	child_widget.push_back(settings_cb[2]);
	optiontab[child_widget.size()-1] = 2;

	settings_lb[3]->set(msg->get("Hardware surfaces"));
	child_widget.push_back(settings_lb[3]);
	optiontab[child_widget.size()-1] = 0;

	if (HWSURFACE == true) settings_cb[3]->Check();
	child_widget.push_back(settings_cb[3]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[4]->set(msg->get("Double buffering"));
	child_widget.push_back(settings_lb[4]);
	optiontab[child_widget.size()-1] = 0;

	if (DOUBLEBUF == true) settings_cb[4]->Check();
	child_widget.push_back(settings_cb[4]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[5]->set(msg->get("Use joystick"));
	child_widget.push_back(settings_lb[5]);
	optiontab[child_widget.size()-1] = 3;

	if (ENABLE_JOYSTICK == true) settings_cb[5]->Check();
	child_widget.push_back(settings_cb[5]);
	optiontab[child_widget.size()-1] = 3;

	//Define ComboBoxes and their Labels

	settings_lb[9]->set(msg->get("Resolution"));
	child_widget.push_back(settings_lb[9]);
	optiontab[child_widget.size()-1] = 0;

	std::stringstream list_mode;

	for (unsigned int i=0; video_modes[i]; ++i) {
		 list_mode << video_modes[i]->w << "x" << video_modes[i]->h;
		 settings_cmb[1]->set(i, list_mode.str());
		 if (video_modes[i]->w == VIEW_W && video_modes[i]->h == VIEW_H) settings_cmb[1]->selected = i;
		 list_mode.str("");
		}

	settings_cmb[1]->refresh();
	child_widget.push_back(settings_cmb[1]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[10]->set(msg->get("Language"));
	child_widget.push_back(settings_lb[10]);
	optiontab[child_widget.size()-1] = 2;

	if (!getLanguagesList()) fprintf(stderr, "Unable to get languages list!\n");
	for (int i=0; i < langCount; i++) {
		 settings_cmb[2]->set(i, language_full[i]);
		 if (language_ISO[i] == LANGUAGE) settings_cmb[2]->selected = i;
		}

	settings_cmb[2]->refresh();
	child_widget.push_back(settings_cmb[2]);
	optiontab[child_widget.size()-1] = 2;

	settings_lb[8]->set(msg->get("Joystick"));
	child_widget.push_back(settings_lb[8]);
	optiontab[child_widget.size()-1] = 3;

	//if (JOYSTICK_DEVICE > -1) settings_cmb[0]->selected = JOYSTICK_DEVICE;
	settings_cmb[0]->refresh();
	child_widget.push_back(settings_cmb[0]);
	optiontab[child_widget.size()-1] = 3;

	// Add Key Binding objects
	for (unsigned int i = 11; i < 36; i++) {
		 settings_lb[i]->set("Key");
		 child_widget.push_back(settings_lb[i]);
		 optiontab[child_widget.size()-1] = 3;
	}
	for (unsigned int i = 0; i < 25; i++) {
		 child_widget.push_back(settings_key[i]);
		 optiontab[child_widget.size()-1] = 3;
	}
}


GameStateConfig::~GameStateConfig()
{
	delete tabControl;

	for (unsigned int i = 0; i < child_widget.size(); i++) {
		delete child_widget[i];
	}
}


void GameStateConfig::logic ()
{
	int active;
	tabControl->logic();

	// Ok/Cancel Buttons
	if (ok_button->checkClick()) {
		saveSettings();
		requestedGameState = new GameStateTitle();
	} else if (cancel_button->checkClick()) {
		requestedGameState = new GameStateTitle();
	}

	int active_tab = tabControl->getActiveTab();

	// tab 0 (video)
	if (active_tab == 0) {
		if (settings_cb[0]->checkClick()) {
			if (settings_cb[0]->isChecked()) FULLSCREEN=true;
			else FULLSCREEN=false;
		} else if (settings_cb[3]->checkClick()) {
			if (settings_cb[3]->isChecked()) HWSURFACE=true;
			else HWSURFACE=false;
		} else if (settings_cb[4]->checkClick()) {
			if (settings_cb[4]->isChecked()) DOUBLEBUF=true;
			else DOUBLEBUF=false;
		} else if (settings_cmb[1]->checkClick()) {
			std::string value;

			// TODO We need to change resolution only in settings file, not at runtime
			active = settings_cmb[1]->getSelected();
			value = settings_cmb[1]->get(active) + 'x';
			VIEW_W = eatFirstInt(value, 'x');
			VIEW_H = eatFirstInt(value, 'x');
		}
	}
	// tab 1 (audio)
	else if (active_tab == 1) {
		if (settings_sl[0]->checkClick()) {
			MUSIC_VOLUME=settings_sl[0]->getValue();
		} else if (settings_sl[1]->checkClick()) {
			SOUND_VOLUME=settings_sl[1]->getValue();
		}
	}
	// tab 2 (interface)
	else if (active_tab == 2) {
		if (settings_cb[2]->checkClick()) {
			if (settings_cb[2]->isChecked()) COMBAT_TEXT=true;
			else COMBAT_TEXT=false;
		} else if (settings_cmb[2]->checkClick()) {
			active = settings_cmb[2]->getSelected();
			LANGUAGE = language_ISO[active];
		}
	}
	// tab 3 (input)
	else if (active_tab == 3) {
		if (settings_cb[1]->checkClick()) {
			if (settings_cb[1]->isChecked()) MOUSE_MOVE=true;
			else MOUSE_MOVE=false;
		} else if (settings_cb[5]->checkClick()) {
			if (settings_cb[5]->isChecked()) ENABLE_JOYSTICK=true;
			else ENABLE_JOYSTICK=false;
		}
	}
}

void GameStateConfig::render ()
{
	tabControl->render();

	for (unsigned int i = 0; i < 2; i++) {
		child_widget[i]->render();
	}

	int active_tab = tabControl->getActiveTab();

	for (unsigned int i = 2; i < child_widget.size(); i++) {
		 if (optiontab[i] == active_tab) child_widget[i]->render();
	}
}

int GameStateConfig::getVideoModes()
{
	int modes = 0;

	/* Get available fullscreen/hardware modes */
	video_modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

	/* Check if there are any modes available */
	if (video_modes == (SDL_Rect**)0) {
		fprintf(stderr, "No modes available!\n");
		return 0;
	}

	/* Check if our resolution is restricted */
	if (video_modes == (SDL_Rect**)-1) {
		fprintf(stderr, "All resolutions available.\n");
	}

	for (unsigned int i=0; video_modes[i]; ++i) {
		modes += 1;
	}
	//TODO Exclude resolutions smaller than 640x480

	return modes;
}

bool GameStateConfig::getLanguagesList()
{
	FileParser infile;
	if (infile.open(mods->locate("engine/languages.txt"))) {
		unsigned int i=0;
		while (infile.next()) {
			   language_ISO[i] = infile.key;
			   language_full[i] = infile.val.c_str();
			   i += 1;
			}
		} else fprintf(stderr, "Unable to open languages.txt!\n");
		infile.close();

	return true;
}

int GameStateConfig::getLanguagesNumber()
{
	int languages_num = 0;
	FileParser infile;
	if (infile.open(mods->locate("engine/languages.txt"))) {
		while (infile.next()) {
			   languages_num += 1;
			}
		} else fprintf(stderr, "Unable to open languages.txt!\n");
		infile.close();

	return languages_num;
}
