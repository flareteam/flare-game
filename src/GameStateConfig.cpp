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

#include "FileParser.h"
#include "GameStateConfig.h"
#include "GameStateTitle.h"
#include "MenuConfirm.h"
#include "Settings.h"
#include "SharedResources.h"
#include "UtilsParsing.h"
#include "UtilsFileSystem.h"
#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetComboBox.h"
#include "WidgetListBox.h"
#include "WidgetScrollBox.h"
#include "WidgetSlider.h"
#include "WidgetTabControl.h"

#include <sstream>

using namespace std;

GameStateConfig::GameStateConfig ()
		: GameState(),
		  child_widget(),
		  ok_button(NULL),
		  defaults_button(NULL),
		  cancel_button(NULL),
		  imgFileName(mods->locate("images/menus/config.png"))

{
	// Load background image
	SDL_Surface * tmp = IMG_Load(imgFileName.c_str());
	if (NULL == tmp) {
		fprintf(stderr, "Could not load image \"%s\" error \"%s\"\n",
				imgFileName.c_str(), IMG_GetError());
		SDL_Quit();
		exit(1);
	}
	background = SDL_DisplayFormatAlpha(tmp);
	SDL_FreeSurface(tmp);

	// Initialize Widgets
	tabControl = new WidgetTabControl(6);
	ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	defaults_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	input_confirm = new MenuConfirm("",msg->get("Press a key to assign: "));
	defaults_confirm = new MenuConfirm(msg->get("Defaults"),msg->get("Set ALL settings to default?"));

	for (unsigned int i = 0; i < 42; i++) {
		 settings_lb[i] = new WidgetLabel();
	}

	for (unsigned int i = 0; i < 3; i++) {
		 settings_sl[i] = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
	}

	for (unsigned int i = 0; i < 7; i++) {
		 settings_cb[i] = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	}

	for (unsigned int i = 0; i < 50; i++) {
		 settings_key[i] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	}

	input_scrollbox = new WidgetScrollBox(600, 350);
	input_scrollbox->pos.x = (VIEW_W - 640)/2 + 10;
	input_scrollbox->pos.y = (VIEW_H - 480)/2 + 30;
	input_scrollbox->resize(780);

	settings_btn[0] = new WidgetButton(mods->locate("images/menus/buttons/up.png"));
	settings_btn[1] = new WidgetButton(mods->locate("images/menus/buttons/down.png"));
	settings_btn[2] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	settings_btn[3] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	// Allocate Joycticks ComboBox
	settings_cmb[0] = new WidgetComboBox(SDL_NumJoysticks(), mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate Resolution ComboBox
	int resolutions = getVideoModes();
	if (resolutions < 1) fprintf(stderr, "Unable to get resolutions list!\n");

	settings_cmb[1] = new WidgetComboBox(resolutions, mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate Languages ComboBox
	int langCount = getLanguagesNumber();
	language_ISO = new std::string[langCount];
	language_full = new std::string[langCount];
	settings_cmb[2] = new WidgetComboBox(langCount, mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate Mods ListBoxes
	vector<string> mod_dirs;
	getDirList(PATH_DATA + "mods", mod_dirs);
	mods_total = mod_dirs.size();
	// Remove active mods from the available mods list
	for (unsigned int i = 0; i<mods->mod_list.size(); i++) {
		for (unsigned int j = 0; j<mod_dirs.size(); j++) {
			if (mods->mod_list[i] == mod_dirs[j]) mod_dirs[j].erase();
		}
	}

	settings_lstb[0] = new WidgetListBox(mods_total, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	settings_lstb[1] = new WidgetListBox(mods_total, 10, mods->locate("images/menus/buttons/listbox_default.png"));

	//Load the menu configuration from file
	int x1;
	int y1;
	int x2;
	int y2;
	int setting_num;
	int offset_x;
	int offset_y;

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
			else if (infile.key == "texture_quality") setting_num = 7;
			else if (infile.key == "music_volume") setting_num = 8;
			else if (infile.key == "sound_volume") setting_num = 9;
			else if (infile.key == "gamma") setting_num = 10;
			else if (infile.key == "joystick_device") setting_num = 11;
			else if (infile.key == "resolution") setting_num = 12;
			else if (infile.key == "language")setting_num = 13;

			else if (infile.key == "cancel") setting_num = 14 + CANCEL;
			else if (infile.key == "accept") setting_num = 14 + ACCEPT;
			else if (infile.key == "up") setting_num = 14 + UP;
			else if (infile.key == "down") setting_num = 14 + DOWN;
			else if (infile.key == "left") setting_num = 14 + LEFT;
			else if (infile.key == "right") setting_num = 14 + RIGHT;
			else if (infile.key == "bar1") setting_num = 14 + BAR_1;
			else if (infile.key == "bar2") setting_num = 14 + BAR_2;
			else if (infile.key == "bar3") setting_num = 14 + BAR_3;
			else if (infile.key == "bar4") setting_num = 14 + BAR_4;
			else if (infile.key == "bar5") setting_num = 14 + BAR_5;
			else if (infile.key == "bar6") setting_num = 14 + BAR_6;
			else if (infile.key == "bar7") setting_num = 14 + BAR_7;
			else if (infile.key == "bar8") setting_num = 14 + BAR_8;
			else if (infile.key == "bar9") setting_num = 14 + BAR_9;
			else if (infile.key == "bar0") setting_num = 14 + BAR_0;
			else if (infile.key == "main1") setting_num = 14 + MAIN1;
			else if (infile.key == "main2") setting_num = 14 + MAIN2;
			else if (infile.key == "character") setting_num = 14 + CHARACTER;
			else if (infile.key == "inventory") setting_num = 14 + INVENTORY;
			else if (infile.key == "powers") setting_num = 14 + POWERS;
			else if (infile.key == "log") setting_num = 14 + LOG;
			else if (infile.key == "ctrl") setting_num = 14 + CTRL;
			else if (infile.key == "shift") setting_num = 14 + SHIFT;
			else if (infile.key == "delete") setting_num = 14 + DEL;
			else if (infile.key == "hws_note") setting_num = 39;
			else if (infile.key == "dbuf_note") setting_num = 40;
			else if (infile.key == "activemods") setting_num = 41;
			else if (infile.key == "inactivemods") setting_num = 42;
			else if (infile.key == "activemods_shiftup") setting_num = 43;
			else if (infile.key == "activemods_shiftdown") setting_num = 44;
			else if (infile.key == "activemods_deactivate") setting_num = 45;
			else if (infile.key == "inactivemods_activate") setting_num = 46;
			else if (infile.key == "secondary_offset") {offset_x = x1; offset_y = y1;}

			if (setting_num != -1) {
				if (setting_num > 13 && setting_num < 39) {
					settings_lb[setting_num-1]->setX(x1);
					settings_lb[setting_num-1]->setY(y1);
				} else if (setting_num < 43) {
					settings_lb[setting_num-1]->setX((VIEW_W - 640)/2 + x1);
					settings_lb[setting_num-1]->setY((VIEW_H - 480)/2 + y1);
				}

				if (setting_num < 8) {
					settings_cb[setting_num-1]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_cb[setting_num-1]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if ((setting_num > 7) && (setting_num < 11)) {
					settings_sl[setting_num-8]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_sl[setting_num-8]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if ((setting_num > 10) && (setting_num < 14)) {
					settings_cmb[setting_num-11]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_cmb[setting_num-11]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if (setting_num > 13 && setting_num < 40) {
					settings_key[setting_num-14]->pos.x = x2;
					settings_key[setting_num-14]->pos.y = y2;
				} else if (setting_num > 40 && setting_num < 43) {
					settings_lstb[setting_num-41]->pos.x = (VIEW_W - 640)/2 + x2;
					settings_lstb[setting_num-41]->pos.y = (VIEW_H - 480)/2 + y2;
				} else if (setting_num > 42 && setting_num < 47) {
					settings_btn[setting_num-43]->pos.x = (VIEW_W - 640)/2 + x1;
					settings_btn[setting_num-43]->pos.y = (VIEW_H - 480)/2 + y1;
				}
			}

		  }
		} else fprintf(stderr, "Unable to open config.txt!\n");
		infile.close();

	// Initialize the tab control.
	tabControl->setMainArea(((VIEW_W - 640)/2)+3, (VIEW_H - 480)/2, 640, 480);

	// Set positions of secondary key bindings
	for (unsigned int i = 25; i < 50; i++) {
		settings_key[i]->pos.x = settings_key[i-25]->pos.x + offset_x;
		settings_key[i]->pos.y = settings_key[i-25]->pos.y + offset_y;
	}

	// Define the header.
	tabControl->setTabTitle(0, msg->get("Video"));
	tabControl->setTabTitle(1, msg->get("Audio"));
	tabControl->setTabTitle(2, msg->get("Interface"));
	tabControl->setTabTitle(3, msg->get("Input"));
	tabControl->setTabTitle(4, msg->get("Keybindings"));
	tabControl->setTabTitle(5, msg->get("Mods"));
	tabControl->updateHeader();

	// Define widgets
	ok_button->label = msg->get("Ok");
	ok_button->pos.x = VIEW_W_HALF - ok_button->pos.w/2;
	ok_button->pos.y = VIEW_H - (cancel_button->pos.h*3);
	ok_button->refresh();
	child_widget.push_back(ok_button);

	defaults_button->label = msg->get("Defaults");
	defaults_button->pos.x = VIEW_W_HALF - defaults_button->pos.w/2;
	defaults_button->pos.y = VIEW_H - (cancel_button->pos.h*2);
	defaults_button->refresh();
	child_widget.push_back(defaults_button);

	cancel_button->label = msg->get("Cancel");
	cancel_button->pos.x = VIEW_W_HALF - cancel_button->pos.w/2;
	cancel_button->pos.y = VIEW_H - (cancel_button->pos.h);
	cancel_button->refresh();
	child_widget.push_back(cancel_button);

	settings_lb[0]->set(msg->get("Full Screen Mode"));
	settings_lb[0]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[0]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[0]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[7]->set(msg->get("Music Volume"));
	settings_lb[7]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[7]);
	optiontab[child_widget.size()-1] = 1;

	child_widget.push_back(settings_sl[0]);
	optiontab[child_widget.size()-1] = 1;

	settings_lb[8]->set(msg->get("Sound Volume"));
	settings_lb[8]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[8]);
	optiontab[child_widget.size()-1] = 1;

	child_widget.push_back(settings_sl[1]);
	optiontab[child_widget.size()-1] = 1;

	settings_lb[9]->set(msg->get("Gamma"));
	settings_lb[9]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[9]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_sl[2]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[1]->set(msg->get("Move hero using mouse"));
	settings_lb[1]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[1]);
	optiontab[child_widget.size()-1] = 3;

	child_widget.push_back(settings_cb[1]);
	optiontab[child_widget.size()-1] = 3;

	settings_lb[2]->set(msg->get("Show combat text"));
	settings_lb[2]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[2]);
	optiontab[child_widget.size()-1] = 2;

	child_widget.push_back(settings_cb[2]);
	optiontab[child_widget.size()-1] = 2;

	settings_lb[3]->set(msg->get("Hardware surfaces"));
	settings_lb[3]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[3]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[3]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[4]->set(msg->get("Double buffering"));
	settings_lb[4]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[4]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[4]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[5]->set(msg->get("Use joystick"));
	settings_lb[5]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[5]);
	optiontab[child_widget.size()-1] = 3;

	child_widget.push_back(settings_cb[5]);
	optiontab[child_widget.size()-1] = 3;

	settings_lb[6]->set(msg->get("High Quality Textures"));
	settings_lb[6]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[6]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[6]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[38]->set(msg->get("Try disabling for performance"));
	child_widget.push_back(settings_lb[38]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[39]->set(msg->get("Try disabling for performance"));
	child_widget.push_back(settings_lb[39]);
	optiontab[child_widget.size()-1] = 0;

	//Define ComboBoxes and their Labels

	settings_lb[11]->set(msg->get("Resolution"));
	settings_lb[11]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[11]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cmb[1]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[12]->set(msg->get("Language"));
	settings_lb[12]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[12]);
	optiontab[child_widget.size()-1] = 2;

	child_widget.push_back(settings_cmb[2]);
	optiontab[child_widget.size()-1] = 2;

	settings_lb[10]->set(msg->get("Joystick"));
	settings_lb[10]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[10]);
	optiontab[child_widget.size()-1] = 3;

	for(int i = 0; i < SDL_NumJoysticks(); i++)
	{
		settings_cmb[0]->set(i, SDL_JoystickName(i));
	}
	child_widget.push_back(settings_cmb[0]);
	optiontab[child_widget.size()-1] = 3;

	// Add Key Binding objects
	for (unsigned int i = 13; i < 38; i++) {
		 settings_lb[i]->set(binding_name[i-13]);
		 settings_lb[i]->setJustify(JUSTIFY_RIGHT);
		 child_widget.push_back(settings_lb[i]);
		 optiontab[child_widget.size()-1] = 4;
	}
	for (unsigned int i = 0; i < 50; i++) {
		 child_widget.push_back(settings_key[i]);
		 optiontab[child_widget.size()-1] = 4;
	}

	// Add ListBoxes
	settings_lb[40]->set(msg->get("Active Mods"));
	child_widget.push_back(settings_lb[40]);
	optiontab[child_widget.size()-1] = 5;

	settings_lstb[0]->multi_select = true;
	for (unsigned int i = 0; i < mods->mod_list.size() ; i++) {
		settings_lstb[0]->append(mods->mod_list[i],"");
	}
	child_widget.push_back(settings_lstb[0]);
	optiontab[child_widget.size()-1] = 5;

	settings_lb[41]->set(msg->get("Avaliable Mods"));
	child_widget.push_back(settings_lb[41]);
	optiontab[child_widget.size()-1] = 5;

	settings_lstb[1]->multi_select = true;
	for (unsigned int i = 0; i < mod_dirs.size(); i++) {
		settings_lstb[1]->append(mod_dirs[i],"");
	}
	child_widget.push_back(settings_lstb[1]);
	optiontab[child_widget.size()-1] = 5;

	// Add Button labels
	settings_btn[2]->label = msg->get("<< Deactivate");
	settings_btn[3]->label = msg->get("Activate >>");

	for (unsigned int i=0; i<4; i++) {
		settings_btn[i]->refresh();
		child_widget.push_back(settings_btn[i]);
		optiontab[child_widget.size()-1] = 5;
	}


	update();
}


GameStateConfig::~GameStateConfig()
{
	delete tabControl;
	delete ok_button;
	delete defaults_button;
	delete cancel_button;
	delete input_scrollbox;
	delete input_confirm;
	delete defaults_confirm;

	SDL_FreeSurface(background);

	child_widget.clear();

	for (unsigned int i = 0; i < 42; i++) {
		 delete settings_lb[i];
	}

	for (unsigned int i = 0; i < 3; i++) {
		 delete settings_sl[i];
	}

	for (unsigned int i = 0; i < 7; i++) {
		 delete settings_cb[i];
	}

	for (unsigned int i = 0; i < 50; i++) {
		 delete settings_key[i];
	}

	for (unsigned int i = 0; i < 3; i++) {
		 delete settings_cmb[i];
	}

	for (unsigned int i = 0; i < 2; i++) {
		 delete settings_lstb[i];
	}

	for (unsigned int i = 0; i < 4; i++) {
		 delete settings_btn[i];
	}
}

void GameStateConfig::update () {
	if (FULLSCREEN) settings_cb[0]->Check();
	else settings_cb[0]->unCheck();
	if (audio) {
		settings_sl[0]->set(0,128,MUSIC_VOLUME);
		Mix_VolumeMusic(MUSIC_VOLUME);
		settings_sl[1]->set(0,128,SOUND_VOLUME);
		Mix_Volume(-1, SOUND_VOLUME);
	} else {
		settings_sl[0]->set(0,128,0);
		settings_sl[1]->set(0,128,0);
	}
	if (MOUSE_MOVE) settings_cb[1]->Check();
	else settings_cb[1]->unCheck();
	if (COMBAT_TEXT) settings_cb[2]->Check();
	else settings_cb[2]->unCheck();
	if (HWSURFACE) settings_cb[3]->Check();
	else settings_cb[3]->unCheck();
	if (DOUBLEBUF) settings_cb[4]->Check();
	else settings_cb[4]->unCheck();
	if (ENABLE_JOYSTICK) settings_cb[5]->Check();
	else settings_cb[5]->unCheck();
	if (TEXTURE_QUALITY) settings_cb[6]->Check();
	else settings_cb[6]->unCheck();

	if ((ENABLE_JOYSTICK) && (SDL_NumJoysticks() > 0)) {
		SDL_JoystickClose(joy);
		joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
		settings_cmb[0]->selected = JOYSTICK_DEVICE;
		settings_cmb[0]->refresh();
	}

	settings_sl[2]->set(5,20,(int)(GAMMA*10.0));
	SDL_SetGamma(GAMMA,GAMMA,GAMMA);

	std::stringstream list_mode;
	unsigned int resolutions = getVideoModes();
	for (unsigned int i=0; i<resolutions; ++i) {
		 list_mode << video_modes[i].w << "x" << video_modes[i].h;
		 settings_cmb[1]->set(i, list_mode.str());
		 if (video_modes[i].w == VIEW_W && video_modes[i].h == VIEW_H) settings_cmb[1]->selected = i;
		 list_mode.str("");
		}
	int active = settings_cmb[1]->selected;

	// Check if resolution was selected correctly
	list_mode << VIEW_W << "x" << VIEW_H;
	if (settings_cmb[1]->get(active) != list_mode.str()) {
		fprintf(stderr, "Resolution is not supported!\n");
		fprintf(stderr, "Using 640x480 instead!\n");
		for (unsigned int i=0; i<resolutions; ++i) {
			if (video_modes[i].w == 640 && video_modes[i].h == 480) settings_cmb[1]->selected = i;
		}
		VIEW_W = 640;
	}

	settings_cmb[1]->refresh();

	if (!getLanguagesList()) fprintf(stderr, "Unable to get languages list!\n");
	for (int i=0; i < getLanguagesNumber(); i++) {
		 settings_cmb[2]->set(i, language_full[i]);
		 if (language_ISO[i] == LANGUAGE) settings_cmb[2]->selected = i;
		}

	settings_cmb[2]->refresh();

	settings_lstb[0]->refresh();
	settings_lstb[1]->refresh();

	for (unsigned int i = 0; i < 25; i++) {
		if (inpt->binding[i] < 8) {
			settings_key[i]->label = mouse_button[inpt->binding[i]-1];
		} else {
			settings_key[i]->label = SDL_GetKeyName((SDLKey)inpt->binding[i]);
		}
		settings_key[i]->refresh();
	}
	for (unsigned int i = 25; i < 50; i++) {
		if (inpt->binding[i] < 8) {
			settings_key[i]->label = mouse_button[inpt->binding_alt[i-25]-1];
		} else {
			settings_key[i]->label = SDL_GetKeyName((SDLKey)inpt->binding_alt[i-25]);
		}
		settings_key[i]->refresh();
	}
	input_scrollbox->refresh();
}

void GameStateConfig::logic ()
{
	int active;

	// Initialize resolution value
	std::string value;
	active = settings_cmb[1]->selected;
	value = settings_cmb[1]->get(active) + 'x';
	int width = eatFirstInt(value, 'x');
	int height = eatFirstInt(value, 'x');

	if (defaults_confirm->visible) {
		defaults_confirm->logic();
		if (defaults_confirm->confirmClicked) {
			FULLSCREEN = 0;
			loadDefaults();
			delete msg;
			msg = new MessageEngine();
			update();
			setDefaultResolution();
			defaults_confirm->visible = false;
			defaults_confirm->confirmClicked = false;
		}
	}

	if (!input_confirm->visible && !defaults_confirm->visible) {
		tabControl->logic();

		// Ok/Cancel Buttons
		if (ok_button->checkClick()) {
			refreshFont();
			applyVideoSettings(screen, width, height);
			saveSettings();
			inpt->saveKeyBindings();
			if (setMods()) {
				delete mods;
				mods = new ModManager();
				loadTilesetSettings();
			}
			if ((ENABLE_JOYSTICK) && (SDL_NumJoysticks() > 0)) {
				SDL_JoystickClose(joy);
				joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
			}
			delete requestedGameState;
			requestedGameState = new GameStateTitle();
		} else if (defaults_button->checkClick()) {
			defaults_confirm->visible = true;
		} else if (cancel_button->checkClick()) {
			loadSettings();
			inpt->loadKeyBindings();
			delete msg;
			msg = new MessageEngine();
			update();
			delete requestedGameState;
			requestedGameState = new GameStateTitle();
		}
	}

	int active_tab = tabControl->getActiveTab();

	// tab 0 (video)
	if (active_tab == 0 && !defaults_confirm->visible) {
		if (settings_cb[0]->checkClick()) {
			if (settings_cb[0]->isChecked()) FULLSCREEN=true;
			else FULLSCREEN=false;
		} else if (settings_cb[3]->checkClick()) {
			if (settings_cb[3]->isChecked()) HWSURFACE=true;
			else HWSURFACE=false;
		} else if (settings_cb[4]->checkClick()) {
			if (settings_cb[4]->isChecked()) DOUBLEBUF=true;
			else DOUBLEBUF=false;
		} else if (settings_cb[6]->checkClick()) {
			if (settings_cb[6]->isChecked()) TEXTURE_QUALITY=true;
			else TEXTURE_QUALITY=false;
		} else if (settings_cmb[1]->checkClick()) {
			active = settings_cmb[1]->selected;
			value = settings_cmb[1]->get(active) + 'x';
		} else if (settings_sl[2]->checkClick()) {
			GAMMA=(float)(settings_sl[2]->getValue())*0.1;
			SDL_SetGamma(GAMMA,GAMMA,GAMMA);
		}
	}
	// tab 1 (audio)
	else if (active_tab == 1 && !defaults_confirm->visible) {
		if (audio) {
			if (settings_sl[0]->checkClick()) {
				MUSIC_VOLUME=settings_sl[0]->getValue();
				Mix_VolumeMusic(MUSIC_VOLUME);
			} else if (settings_sl[1]->checkClick()) {
				SOUND_VOLUME=settings_sl[1]->getValue();
				Mix_Volume(-1, SOUND_VOLUME);
			}
		}
	}
	// tab 2 (interface)
	else if (active_tab == 2 && !defaults_confirm->visible) {
		if (settings_cb[2]->checkClick()) {
			if (settings_cb[2]->isChecked()) COMBAT_TEXT=true;
			else COMBAT_TEXT=false;
		} else if (settings_cmb[2]->checkClick()) {
			active = settings_cmb[2]->selected;
			LANGUAGE = language_ISO[active];
			delete msg;
			msg = new MessageEngine();
		}
	}
	// tab 3 (input)
	else if (active_tab == 3 && !defaults_confirm->visible) {
		if (settings_cb[1]->checkClick()) {
			if (settings_cb[1]->isChecked()) MOUSE_MOVE=true;
			else MOUSE_MOVE=false;
		} else if (settings_cb[5]->checkClick()) {
			if (settings_cb[5]->isChecked()) ENABLE_JOYSTICK=true;
			else ENABLE_JOYSTICK=false;
			if (SDL_NumJoysticks() > 0) settings_cmb[0]->refresh();
		} else if (settings_cmb[0]->checkClick()) {
			JOYSTICK_DEVICE = settings_cmb[0]->selected;
		}
	}
	// tab 4 (keybindings)
	else if (active_tab == 4 && !defaults_confirm->visible) {
		if (input_confirm->visible) {
			input_confirm->logic();
			scanKey(input_key);
		} else {
			input_scrollbox->logic();
			if (isWithin(input_scrollbox->pos,inpt->mouse)) {
				for (unsigned int i = 0; i < 50; i++) {
					if (settings_key[i]->pressed || settings_key[i]->hover) input_scrollbox->update = true;
					Point mouse = input_scrollbox->input_assist(inpt->mouse);
					if (settings_key[i]->checkClick(mouse.x,mouse.y)) {
						std::string confirm_msg;
						if (i < 25)
							confirm_msg = msg->get("Press a key to assign: ") + binding_name[i];
						else
							confirm_msg = msg->get("Press a key to assign: ") + binding_name[i-25];
						delete input_confirm;
						input_confirm = new MenuConfirm("",confirm_msg);
						input_confirm->visible = true;
						input_key = i;
						inpt->last_button = -1;
						inpt->last_key = -1;
					}
				}
			}
		}
	}
	// tab 5 (mods)
	else if (active_tab == 5 && !defaults_confirm->visible) {
		if (settings_lstb[0]->checkClick()) {
			//do nothing
		} else if (settings_lstb[1]->checkClick()) {
			//do nothing
		} else if (settings_btn[0]->checkClick()) {
			settings_lstb[0]->shiftUp();
		} else if (settings_btn[1]->checkClick()) {
			settings_lstb[0]->shiftDown();
		} else if (settings_btn[2]->checkClick()) {
			disableMods();
		} else if (settings_btn[3]->checkClick()) {
			enableMods();
		}
	}
}

void GameStateConfig::render ()
{
	SDL_Rect	pos = tabControl->getContentArea();
	pos.x = pos.x-12;
	pos.y = pos.y-12;
	pos.w = 640;
	pos.h = 380;

	SDL_BlitSurface(background,NULL,screen,&pos);

	tabControl->render();

	for (unsigned int i = 0; i < 3; i++) {
		child_widget[i]->render();
	}

	int active_tab = tabControl->getActiveTab();

	if (active_tab == 4) {
		for (unsigned int i = 13; i < 38; i++) {
			if (input_scrollbox->update) settings_lb[i]->render(input_scrollbox->contents);
		}
		for (unsigned int i = 0; i < 50; i++) {
			if (input_scrollbox->update) settings_key[i]->render(input_scrollbox->contents);
		}
		input_scrollbox->render();
	}

	for (unsigned int i = 3; i < 31; i++) {
		 if (optiontab[i] == active_tab) child_widget[i]->render();
	}
	for (unsigned int i = 106; i < child_widget.size(); i++) {
		 if (optiontab[i] == active_tab) child_widget[i]->render();
	}

	if (input_confirm->visible) input_confirm->render();
	if (defaults_confirm->visible) defaults_confirm->render();
}

int GameStateConfig::getVideoModes()
{
	/* Set predefined modes */
	const unsigned int cm_count = 4;
	SDL_Rect common_modes[cm_count];
	common_modes[0].w = 640;
	common_modes[0].h = 480;
	common_modes[1].w = 800;
	common_modes[1].h = 600;
	common_modes[2].w = 1024;
	common_modes[2].h = 768;
	common_modes[3].w = 1280;
	common_modes[3].h = 1024;

	int modes = 0;

	/* Get available fullscreen/hardware modes */
	SDL_Rect** detect_modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

	/* Check if there are any modes available */
	if (detect_modes == (SDL_Rect**)0) {
		fprintf(stderr, "No modes available!\n");
		return 0;
	}

	/* Check if our resolution is restricted */
	if (detect_modes == (SDL_Rect**)-1) {
		fprintf(stderr, "All resolutions available.\n");
	}

	/* Determine the number of valid modes */
	for (unsigned int i=0; detect_modes[i]; ++i) {
		if (detect_modes[i]->w >= 640 && detect_modes[i]->h >= 480) {
			modes++;
		}
	}
	for (unsigned int j=0; j<cm_count; ++j) {
		for (unsigned int i=0; detect_modes[i]; i++) {
			if(common_modes[j].w != 0) {
				if (detect_modes[i]->w >= 640 && detect_modes[i]->h >= 480) {
					if (common_modes[j].w == detect_modes[i]->w && common_modes[j].h == detect_modes[i]->h) {
						common_modes[j].w = 0;
						break;
					}
				}
			}
		}
		if (common_modes[j].w != 0) {
			modes++;
		}
	}

	/* Combine the detected modes and the common modes */
	video_modes = (SDL_Rect*)calloc(modes,sizeof(SDL_Rect));
	int k = 0;

	for (unsigned int i=0; detect_modes[i]; ++i) {
		if (detect_modes[i]->w >= 640 && detect_modes[i]->h >= 480) {
			video_modes[k].w = detect_modes[i]->w;
			video_modes[k].h = detect_modes[i]->h;
			k++;
		}
	}
	for (unsigned int j=0; j<cm_count; ++j) {
		for (unsigned int i=0; detect_modes[i]; i++) {
			if(common_modes[j].w != 0) {
				if (detect_modes[i]->w >= 640 && detect_modes[i]->h >= 480) {
					if (common_modes[j].w == detect_modes[i]->w && common_modes[j].h == detect_modes[i]->h) {
						common_modes[j].w = 0;
						break;
					}
				}
			}
		}
		if (common_modes[j].w != 0) {
			video_modes[k].w = common_modes[j].w;
			video_modes[k].h = common_modes[j].h;
			k++;
		}
	}

	/* Sort the new list */
	for (int x=0; x<modes; x++) {
		int index_of_min = x;
		for (int y=x; y<modes; y++) {
			if (video_modes[index_of_min].w < video_modes[y].w) {
				index_of_min = y;
			}
		}
		SDL_Rect temp = video_modes[x];
		video_modes[x] = video_modes[index_of_min];
		video_modes[index_of_min] = temp;
	}

	return modes;
}

bool GameStateConfig::getLanguagesList()
{
	FileParser infile;
	if (infile.open(mods->locate("engine/languages.txt"))) {
		unsigned int i=0;
		while (infile.next()) {
			   language_ISO[i] = infile.key;
			   language_full[i] = infile.nextValue();
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

/**
 * This function is a HACK to set combobox to default resolution without changing it at runtime
 */
void GameStateConfig::setDefaultResolution()
{
	unsigned int resolutions = getVideoModes();

	for (unsigned int i=0; i<resolutions; ++i) {
		if (video_modes[i].w == 640 && video_modes[i].h == 480) settings_cmb[1]->selected = i;
	}
	settings_cmb[1]->refresh();

}

void GameStateConfig::refreshFont() {
	delete font;
	font = new FontEngine();
}

/**
 * Tries to apply the selected video settings, reverting back to the old settings upon failure
 */
bool GameStateConfig::applyVideoSettings(SDL_Surface *src, int width, int height) {
	// Temporarily save previous settings
	int tmp_fs = FULLSCREEN;
	int tmp_w = VIEW_W;
	int tmp_h = VIEW_H;

	// Attempt to apply the new settings
	Uint32 flags = 0;
	if (FULLSCREEN) flags = flags | SDL_FULLSCREEN;
	if (DOUBLEBUF) flags = flags | SDL_DOUBLEBUF;
	if (HWSURFACE)
		flags = flags | SDL_HWSURFACE | SDL_HWACCEL;
	else
		flags = flags | SDL_SWSURFACE;

	src = SDL_SetVideoMode (width, height, 0, flags);

	// If the new settings fail, revert to the old ones
	if (src == NULL) {
		fprintf (stderr, "Error during SDL_SetVideoMode: %s\n", SDL_GetError());

		flags = 0;
		if (tmp_fs) flags = flags | SDL_FULLSCREEN;
		if (DOUBLEBUF) flags = flags | SDL_DOUBLEBUF;
		if (HWSURFACE)
			flags = flags | SDL_HWSURFACE | SDL_HWACCEL;
		else
			flags = flags | SDL_SWSURFACE;

		src = SDL_SetVideoMode (tmp_w, tmp_h, 0, flags);

		return false;
	}

	// If the new settings succeed, adjust the view area
	VIEW_W = width;
	VIEW_W_HALF = width/2;
	VIEW_H = height;
	VIEW_H_HALF = height/2;

	return true;
}

/**
 * Activate mods
 */
void GameStateConfig::enableMods() {
	for (int i=0; i<settings_lstb[1]->getSize(); i++) {
		if (settings_lstb[1]->selected[i]) {
			settings_lstb[0]->append(settings_lstb[1]->getValue(i),settings_lstb[1]->getTooltip(i));
			settings_lstb[1]->remove(i);
			i--;
		}
	}
}

/**
 * Deactivate mods
 */
void GameStateConfig::disableMods() {
	for (int i=0; i<settings_lstb[0]->getSize(); i++) {
		if (settings_lstb[0]->selected[i] && settings_lstb[0]->getValue(i) != FALLBACK_MOD) {
			settings_lstb[1]->append(settings_lstb[0]->getValue(i),settings_lstb[0]->getTooltip(i));
			settings_lstb[0]->remove(i);
			i--;
		}
	}
}

/**
 * Save new mods list. Return true if modlist was changed. Else return false
 */
bool GameStateConfig::setMods() {
	vector<string> temp_list = mods->mod_list;
	mods->mod_list.clear();
	for (int i=0; i<settings_lstb[0]->getSize(); i++) {
		if (settings_lstb[0]->getValue(i) != "") mods->mod_list.push_back(settings_lstb[0]->getValue(i));
	}
	ofstream outfile;
	outfile.open((PATH_CONF + "mods.txt").c_str(), ios::out);

	if (outfile.is_open()) {

		outfile<<"# Mods lower on the list will overwrite data in the entries higher on the list"<<"\n";
		outfile<<"\n";

		for (unsigned int i = 0; i < mods->mod_list.size(); i++) {
			outfile<<mods->mod_list[i]<<"\n";
		}
	}
	outfile.close();
	if (mods->mod_list != temp_list) return true;
	else return false;
}

/**
 * Scan key binding
 */
void GameStateConfig::scanKey(int button) {
	if (input_confirm->visible) {
	if (inpt->last_button != -1) {
		if (button < 25) inpt->binding[button] = inpt->last_button;
		else inpt->binding_alt[button-25] = inpt->last_button;

		settings_key[button]->label = mouse_button[inpt->last_button-1];
		input_confirm->visible = false;
		settings_key[button]->refresh();
		return;
	}
	if (inpt->last_key != -1) {
		if (button < 25) inpt->binding[button] = inpt->last_key;
		else inpt->binding_alt[button-25] = inpt->last_key;

		settings_key[button]->label = SDL_GetKeyName((SDLKey)inpt->last_key);
		input_confirm->visible = false;
		settings_key[button]->refresh();
		return;
	}
	}
 }

