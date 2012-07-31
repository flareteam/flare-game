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
		  video_modes(NULL),
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
	tabControl->setMainArea(((VIEW_W - FRAME_W)/2)+3, (VIEW_H - FRAME_H)/2, FRAME_W, FRAME_H);
	SDL_Rect frame = tabControl->getContentArea();

	ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	defaults_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	input_confirm = new MenuConfirm("",msg->get("Assign: "));
	defaults_confirm = new MenuConfirm(msg->get("Defaults"),msg->get("Reset ALL settings?"));

	// Allocate Labels
	for (unsigned int i = 0; i < 46; i++) {
		 settings_lb[i] = new WidgetLabel();
	}

	// Allocate Sliders
	for (unsigned int i = 0; i < 3; i++) {
		 settings_sl[i] = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
	}

	// Allocate CheckBoxes
	for (unsigned int i = 0; i < 9; i++) {
		 settings_cb[i] = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	}

	// Allocate KeyBindingButtons
	for (unsigned int i = 0; i < 50; i++) {
		 settings_key[i] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	}

	// Allocate Mods Configuration Buttons
	settings_btn[0] = new WidgetButton(mods->locate("images/menus/buttons/up.png"));
	settings_btn[1] = new WidgetButton(mods->locate("images/menus/buttons/down.png"));
	settings_btn[2] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	settings_btn[3] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	// Allocate Joycticks ComboBox
	settings_cmb[0] = new WidgetComboBox(SDL_NumJoysticks(), mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate Languages ComboBox
	int langCount = getLanguagesNumber();
	language_ISO = std::vector<std::string>();
	language_full = std::vector<std::string>();
	language_ISO.resize(langCount);
	language_full.resize(langCount);
	settings_cmb[1] = new WidgetComboBox(langCount, mods->locate("images/menus/buttons/combobox_default.png"));

	// Allocate resolution list box
	int resolutions = getVideoModes();
	if (resolutions < 1) fprintf(stderr, "Unable to get resolutions list!\n");
	settings_lstb[0] = new WidgetListBox(resolutions, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	settings_lstb[0]->can_deselect = false;

	// Allocate Mods ListBoxes
	vector<string> mod_dirs;
	getDirList(PATH_DATA + "mods", mod_dirs);
	mods_total = mod_dirs.size();
	// Remove active mods from the available mods list
	for (unsigned int i = 0; i<mods->mod_list.size(); i++) {
		for (unsigned int j = 0; j<mod_dirs.size(); j++) {
			if (mods->mod_list[i] == mod_dirs[j] || FALLBACK_MOD == mod_dirs[j]) mod_dirs[j].erase();
		}
	}
	settings_lstb[1] = new WidgetListBox(mods_total, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	settings_lstb[2] = new WidgetListBox(mods_total, 10, mods->locate("images/menus/buttons/listbox_default.png"));

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
			//labels with num 1--15, 16--40, 41--46
			if (infile.key == "fullscreen") setting_num = 1;//checkbox
			else if (infile.key == "mouse_move") setting_num = 2;//checkbox
			else if (infile.key == "combat_text") setting_num = 3;//checkbox
			else if (infile.key == "hwsurface") setting_num = 4;//checkbox
			else if (infile.key == "doublebuf") setting_num = 5;//checkbox
			else if (infile.key == "enable_joystick") setting_num = 6;//checkbox
			else if (infile.key == "texture_quality") setting_num = 7;//checkbox
			else if (infile.key == "change_gamma") setting_num = 8;//checkbox
			else if (infile.key == "animated_tiles") setting_num = 9;//checkbox

			else if (infile.key == "music_volume") setting_num = 10;//slider
			else if (infile.key == "sound_volume") setting_num = 11;//slider
			else if (infile.key == "gamma") setting_num = 12;//slider

			else if (infile.key == "joystick_device") setting_num = 13;//combobox
			else if (infile.key == "language")setting_num = 14;//combobox
			// buttons begin
			else if (infile.key == "cancel") setting_num = 15 + CANCEL;
			else if (infile.key == "accept") setting_num = 15 + ACCEPT;
			else if (infile.key == "up") setting_num = 15 + UP;
			else if (infile.key == "down") setting_num = 15 + DOWN;
			else if (infile.key == "left") setting_num = 15 + LEFT;
			else if (infile.key == "right") setting_num = 15 + RIGHT;
			else if (infile.key == "bar1") setting_num = 15 + BAR_1;
			else if (infile.key == "bar2") setting_num = 15 + BAR_2;
			else if (infile.key == "bar3") setting_num = 15 + BAR_3;
			else if (infile.key == "bar4") setting_num = 15 + BAR_4;
			else if (infile.key == "bar5") setting_num = 15 + BAR_5;
			else if (infile.key == "bar6") setting_num = 15 + BAR_6;
			else if (infile.key == "bar7") setting_num = 15 + BAR_7;
			else if (infile.key == "bar8") setting_num = 15 + BAR_8;
			else if (infile.key == "bar9") setting_num = 15 + BAR_9;
			else if (infile.key == "bar0") setting_num = 15 + BAR_0;
			else if (infile.key == "main1") setting_num = 15 + MAIN1;
			else if (infile.key == "main2") setting_num = 15 + MAIN2;
			else if (infile.key == "character") setting_num = 15 + CHARACTER;
			else if (infile.key == "inventory") setting_num = 15 + INVENTORY;
			else if (infile.key == "powers") setting_num = 15 + POWERS;
			else if (infile.key == "log") setting_num = 15 + LOG;
			else if (infile.key == "ctrl") setting_num = 15 + CTRL;
			else if (infile.key == "shift") setting_num = 15 + SHIFT;
			else if (infile.key == "delete") setting_num = 15 + DEL;
			// buttons end
			else if (infile.key == "hws_note") setting_num = 40;
			else if (infile.key == "dbuf_note") setting_num = 41;
			else if (infile.key == "anim_tiles_note") setting_num = 42;
			else if (infile.key == "test_note") setting_num = 43;

			else if (infile.key == "resolution") setting_num = 44;//listbox
			else if (infile.key == "activemods") setting_num = 45;//listbox
			else if (infile.key == "inactivemods") setting_num = 46;//listbox

			else if (infile.key == "activemods_shiftup") setting_num = 47;//button
			else if (infile.key == "activemods_shiftdown") setting_num = 48;//button
			else if (infile.key == "activemods_deactivate") setting_num = 49;//button
			else if (infile.key == "inactivemods_activate") setting_num = 50;//button
			else if (infile.key == "secondary_offset") {
				offset_x = x1;
				offset_y = y1;
			}
			else if (infile.key == "keybinds_bg_color") {
				// background color for keybinds scrollbox
				scrollpane_color.x = x1;
				scrollpane_color.y = y1;
				scrollpane_color.w = x2;
			}
			else if (infile.key == "scrollpane") {
				scrollpane.x = x1;
				scrollpane.y = y1;
				scrollpane.w = x2;
				scrollpane.h = y2;
			}

			if (setting_num != -1) {
				if (setting_num > 14 && setting_num < 40) {
					// keybinding labels inside scrollbox
					settings_lb[setting_num-1]->setX(x1);
					settings_lb[setting_num-1]->setY(y1);
				} else if (setting_num < 47) {
					// all labels except keybinding labels
					settings_lb[setting_num-1]->setX(frame.x + x1);
					settings_lb[setting_num-1]->setY(frame.y + y1);
				}

				if (setting_num < 10) {
					//checkboxes positions
					settings_cb[setting_num-1]->pos.x = frame.x + x2;
					settings_cb[setting_num-1]->pos.y = frame.y + y2;
				} else if ((setting_num > 9) && (setting_num < 13)) {
					//sliders positions
					settings_sl[setting_num-10]->pos.x = frame.x + x2;
					settings_sl[setting_num-10]->pos.y = frame.y + y2;
				} else if ((setting_num > 12) && (setting_num < 15)) {
					//comboboxes positions
					settings_cmb[setting_num-13]->pos.x = frame.x + x2;
					settings_cmb[setting_num-13]->pos.y = frame.y + y2;
				} else if (setting_num > 14 && setting_num < 40) {
					//keybinding buttons positions
					settings_key[setting_num-15]->pos.x = x2;
					settings_key[setting_num-15]->pos.y = y2;
				} else if (setting_num > 43 && setting_num < 47) {
					//listboxes positions
					settings_lstb[setting_num-44]->pos.x = frame.x + x2;
					settings_lstb[setting_num-44]->pos.y = frame.y + y2;
				} else if (setting_num > 46 && setting_num < 51) {
					//mods config buttons positions
					settings_btn[setting_num-47]->pos.x = frame.x + x1;
					settings_btn[setting_num-47]->pos.y = frame.y + y1;
				}
			}

		  }
		} else fprintf(stderr, "Unable to open config.txt!\n");
		infile.close();

	// Load the MenuConfirm positions and alignments from menus/menus.txt
	if (infile.open(mods->locate("menus/menus.txt"))) {
		while (infile.next()) {
			infile.val = infile.val + ',';

			if (infile.key == "confirm") {
				menuConfirm_area.x = eatFirstInt(infile.val, ',');
				menuConfirm_area.y = eatFirstInt(infile.val, ',');
				menuConfirm_area.w = eatFirstInt(infile.val, ',');
				menuConfirm_area.h = eatFirstInt(infile.val, ',');
				menuConfirm_align = eatFirstString(infile.val, ',');
				break;
			}
		}
	} else {
		fprintf(stderr, "Unable to open menus.txt!\n");
	}
	infile.close();

	defaults_confirm->window_area = menuConfirm_area;
	defaults_confirm->alignment = menuConfirm_align;
	defaults_confirm->align();
	defaults_confirm->update();

	// Allocate KeyBindings ScrollBox
	input_scrollbox = new WidgetScrollBox(scrollpane.w, scrollpane.h);
	input_scrollbox->pos.x = scrollpane.x + frame.x;
	input_scrollbox->pos.y = scrollpane.y + frame.y;
	input_scrollbox->resize(780);
	input_scrollbox->bg.r = scrollpane_color.x;
	input_scrollbox->bg.g = scrollpane_color.y;
	input_scrollbox->bg.b = scrollpane_color.w;

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

	//Define CheckBoxes and their Labels
	settings_lb[0]->set(msg->get("Full Screen Mode"));
	settings_lb[0]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[0]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[0]);
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

	settings_lb[7]->set(msg->get("Allow changing gamma"));
	settings_lb[7]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[7]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[7]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[8]->set(msg->get("Animated tiles"));
	settings_lb[8]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[8]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_cb[8]);
	optiontab[child_widget.size()-1] = 0;

	//Define Sliders and their Labels
	settings_lb[9]->set(msg->get("Music Volume"));
	settings_lb[9]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[9]);
	optiontab[child_widget.size()-1] = 1;

	child_widget.push_back(settings_sl[0]);
	optiontab[child_widget.size()-1] = 1;

	settings_lb[10]->set(msg->get("Sound Volume"));
	settings_lb[10]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[10]);
	optiontab[child_widget.size()-1] = 1;

	child_widget.push_back(settings_sl[1]);
	optiontab[child_widget.size()-1] = 1;

	settings_lb[11]->set(msg->get("Gamma"));
	settings_lb[11]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[11]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_sl[2]);
	optiontab[child_widget.size()-1] = 0;

	// Add just some Labels
	settings_lb[39]->set(msg->get("Disable for performance"));
	child_widget.push_back(settings_lb[39]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[40]->set(msg->get("Disable for performance"));
	child_widget.push_back(settings_lb[40]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[41]->set(msg->get("Disable for performance"));
	child_widget.push_back(settings_lb[41]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[42]->set(msg->get("Experimental"));
	child_widget.push_back(settings_lb[42]);
	optiontab[child_widget.size()-1] = 0;

	//Define ComboBoxes and their Labels
	settings_lb[12]->set(msg->get("Joystick"));
	settings_lb[12]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[12]);
	optiontab[child_widget.size()-1] = 3;

	for(int i = 0; i < SDL_NumJoysticks(); i++)
	{
		settings_cmb[0]->set(i, SDL_JoystickName(i));
	}
	child_widget.push_back(settings_cmb[0]);
	optiontab[child_widget.size()-1] = 3;

	settings_lb[43]->set(msg->get("Resolution"));
	child_widget.push_back(settings_lb[43]);
	optiontab[child_widget.size()-1] = 0;

	child_widget.push_back(settings_lstb[0]);
	optiontab[child_widget.size()-1] = 0;

	settings_lb[13]->set(msg->get("Language"));
	settings_lb[13]->setJustify(JUSTIFY_RIGHT);
	child_widget.push_back(settings_lb[13]);
	optiontab[child_widget.size()-1] = 2;

	child_widget.push_back(settings_cmb[1]);
	optiontab[child_widget.size()-1] = 2;


	// Add Key Binding objects
	for (unsigned int i = 14; i < 39; i++) {
		 settings_lb[i]->set(binding_name[i-14]);
		 settings_lb[i]->setJustify(JUSTIFY_RIGHT);
		 child_widget.push_back(settings_lb[i]);
		 optiontab[child_widget.size()-1] = 4;
	}
	for (unsigned int i = 0; i < 50; i++) {
		 child_widget.push_back(settings_key[i]);
		 optiontab[child_widget.size()-1] = 4;
	}

	// Add ListBoxes and their Labels
	settings_lb[44]->set(msg->get("Active Mods"));
	child_widget.push_back(settings_lb[44]);
	optiontab[child_widget.size()-1] = 5;

	settings_lstb[1]->multi_select = true;
	for (unsigned int i = 0; i < mods->mod_list.size() ; i++) {
		if (mods->mod_list[i] != FALLBACK_MOD)
			settings_lstb[1]->append(mods->mod_list[i],"");
	}
	child_widget.push_back(settings_lstb[1]);
	optiontab[child_widget.size()-1] = 5;

	settings_lb[45]->set(msg->get("Available Mods"));
	child_widget.push_back(settings_lb[45]);
	optiontab[child_widget.size()-1] = 5;

	settings_lstb[2]->multi_select = true;
	for (unsigned int i = 0; i < mod_dirs.size(); i++) {
		settings_lstb[2]->append(mod_dirs[i],"");
	}
	child_widget.push_back(settings_lstb[2]);
	optiontab[child_widget.size()-1] = 5;

	// Add Button labels for Mods ListBoxes
	settings_btn[2]->label = msg->get("<< Disable");
	settings_btn[3]->label = msg->get("Enable >>");

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

	for (unsigned int i = 0; i < 46; i++) {
		 delete settings_lb[i];
	}

	for (unsigned int i = 0; i < 3; i++) {
		 delete settings_sl[i];
	}

	for (unsigned int i = 0; i < 9; i++) {
		 delete settings_cb[i];
	}

	for (unsigned int i = 0; i < 50; i++) {
		 delete settings_key[i];
	}

	for (unsigned int i = 0; i < 2; i++) {
		 delete settings_cmb[i];
	}

	for (unsigned int i = 0; i < 3; i++) {
		 delete settings_lstb[i];
	}

	for (unsigned int i = 0; i < 4; i++) {
		 delete settings_btn[i];
	}

	language_ISO.clear();
	language_full.clear();

	if (video_modes)
		delete video_modes;
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
	//if (CHANGE_GAMMA) settings_cb[7]->Check();
	//else settings_cb[7]->unCheck();
	if (ANIMATED_TILES) settings_cb[8]->Check();
	else settings_cb[8]->unCheck();

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
		settings_lstb[0]->append(list_mode.str(),"");
		if (video_modes[i].w == VIEW_W && video_modes[i].h == VIEW_H) settings_lstb[0]->selected[i] = true;
		else settings_lstb[0]->selected[i] = false;
		list_mode.str("");
	}

	// Check if resolution was selected correctly
	if (check_resolution) {
		int w,h;
		if (MIN_VIEW_W != -1) w = MIN_VIEW_W;
		else w = 640;
		if (MIN_VIEW_H != -1) h = MIN_VIEW_H;
		else h = 480;
		list_mode << VIEW_W << "x" << VIEW_H;
		if (settings_lstb[0]->getValue() != list_mode.str()) {
			fprintf(stderr, "Resolution is not supported!\n");
			fprintf(stderr, "Using %dx%d instead!\n",w,h);
			for (unsigned int i=0; i<resolutions; ++i) {
				if (video_modes[i].w == w && video_modes[i].h == h) settings_lstb[0]->selected[i] = true;
				else settings_lstb[0]->selected[i] = false;
			}
			VIEW_W = w;
			VIEW_H = h;
		}
	}

	settings_lstb[0]->refresh();

	if (!getLanguagesList()) fprintf(stderr, "Unable to get languages list!\n");
	for (int i=0; i < getLanguagesNumber(); i++) {
		 settings_cmb[1]->set(i, language_full[i]);
		 if (language_ISO[i] == LANGUAGE) settings_cmb[1]->selected = i;
		}

	settings_cmb[1]->refresh();

	settings_lstb[1]->refresh();
	settings_lstb[2]->refresh();

	for (unsigned int i = 0; i < 25; i++) {
		if (inpt->binding[i] < 8) {
			settings_key[i]->label = mouse_button[inpt->binding[i]-1];
		} else {
			settings_key[i]->label = SDL_GetKeyName((SDLKey)inpt->binding[i]);
		}
		settings_key[i]->refresh();
	}
	for (unsigned int i = 25; i < 50; i++) {
		if (inpt->binding_alt[i-25] < 8) {
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
	check_resolution = true;

	// Initialize resolution value
	std::string value;
	value = settings_lstb[0]->getValue() + 'x';
	int width = eatFirstInt(value, 'x');
	int height = eatFirstInt(value, 'x');

	if (defaults_confirm->visible) {
		defaults_confirm->logic();
		if (defaults_confirm->confirmClicked) {
			check_resolution = false;
			FULLSCREEN = 0;
			loadDefaults();
			loadMiscSettings();
			inpt->defaultQwertyKeyBindings();
			delete msg;
			msg = new MessageEngine();
			update();
			defaults_confirm->visible = false;
			defaults_confirm->confirmClicked = false;
		}
	}

	if (!input_confirm->visible && !defaults_confirm->visible) {
		tabControl->logic();

		// Ok/Cancel Buttons
		if (ok_button->checkClick()) {
			inpt->saveKeyBindings();
			if (setMods()) {
				reload_music = true;
				delete mods;
				mods = new ModManager();
				loadTilesetSettings();
			}
			loadMiscSettings();
			refreshFont();
			applyVideoSettings(screen, width, height);
			saveSettings();
			if ((ENABLE_JOYSTICK) && (SDL_NumJoysticks() > 0)) {
				SDL_JoystickClose(joy);
				joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
			}
			delete requestedGameState;
			requestedGameState = new GameStateTitle();
		} else if (defaults_button->checkClick()) {
			defaults_confirm->visible = true;
		} else if (cancel_button->checkClick()) {
			check_resolution = false;
			loadSettings();
			loadMiscSettings();
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
		//} else if (settings_cb[7]->checkClick()) {
		//	if (settings_cb[7]->isChecked()) CHANGE_GAMMA=true;
		//	else CHANGE_GAMMA=false;
		} else if (settings_cb[8]->checkClick()) {
			if (settings_cb[8]->isChecked()) ANIMATED_TILES=true;
			else ANIMATED_TILES=false;
		} else if (settings_lstb[0]->checkClick()) {
			value = settings_lstb[0]->getValue() + 'x';
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
		} else if (settings_cmb[1]->checkClick()) {
			active = settings_cmb[1]->selected;
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
							confirm_msg = msg->get("Assign: ") + binding_name[i];
						else
							confirm_msg = msg->get("Assign: ") + binding_name[i-25];
						delete input_confirm;
						input_confirm = new MenuConfirm("",confirm_msg);
						input_confirm->window_area = menuConfirm_area;
						input_confirm->alignment = menuConfirm_align;
						input_confirm->align();
						input_confirm->update();
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
		if (settings_lstb[1]->checkClick()) {
			//do nothing
		} else if (settings_lstb[2]->checkClick()) {
			//do nothing
		} else if (settings_btn[0]->checkClick()) {
			settings_lstb[1]->shiftUp();
		} else if (settings_btn[1]->checkClick()) {
			settings_lstb[1]->shiftDown();
		} else if (settings_btn[2]->checkClick()) {
			disableMods();
		} else if (settings_btn[3]->checkClick()) {
			enableMods();
		}
	}
}

void GameStateConfig::render ()
{
	int tabheight = tabControl->getTabHeight();
	SDL_Rect	pos;
	pos.x = (VIEW_W-FRAME_W)/2;
	pos.y = (VIEW_H-FRAME_H)/2 + tabheight - tabheight/16;

	SDL_BlitSurface(background,NULL,screen,&pos);

	tabControl->render();

	// render OK/Defaults/Cancel buttons
	for (unsigned int i = 0; i < 3; i++) {
		child_widget[i]->render();
	}

	int active_tab = tabControl->getActiveTab();

	// render keybindings tab
	if (active_tab == 4) {
		if (input_scrollbox->update) input_scrollbox->refresh();
		for (unsigned int i = 14; i < 39; i++) {
			if (input_scrollbox->update) settings_lb[i]->render(input_scrollbox->contents);
		}
		for (unsigned int i = 0; i < 50; i++) {
			if (input_scrollbox->update) settings_key[i]->render(input_scrollbox->contents);
		}
		input_scrollbox->render();
	}

	// render widgets in vector before keybindings
	for (unsigned int i = 3; i < 37; i++) {
		 if (optiontab[i] == active_tab) child_widget[i]->render();
	}
	// render widgets in vector after keybindings
	for (unsigned int i = 112; i < child_widget.size(); i++) {
		 if (optiontab[i] == active_tab) child_widget[i]->render();
	}

	if (input_confirm->visible) input_confirm->render();
	if (defaults_confirm->visible) defaults_confirm->render();
}

int GameStateConfig::getVideoModes()
{
	int w,h;
	if (MIN_VIEW_W != -1) w = MIN_VIEW_W;
	else w = 640;
	if (MIN_VIEW_H != -1) h = MIN_VIEW_H;
	else h = 480;

	/* Set predefined modes */
	const unsigned int cm_count = 4;
	SDL_Rect common_modes[cm_count];
	common_modes[0].w = 640;
	common_modes[0].h = 480;
	common_modes[1].w = 800;
	common_modes[1].h = 600;
	common_modes[2].w = 1024;
	common_modes[2].h = 768;
	common_modes[3].w = w;
	common_modes[3].h = h;

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
		if (detect_modes[i]->w >= w && detect_modes[i]->h >= h) {
			modes++;
		}
	}
	for (unsigned int j=0; j<cm_count; ++j) {
		for (unsigned int i=0; detect_modes[i]; i++) {
			if(common_modes[j].w != 0) {
				if (detect_modes[i]->w >= w && detect_modes[i]->h >= h) {
					if ((common_modes[j].w == detect_modes[i]->w && common_modes[j].h == detect_modes[i]->h) || (common_modes[j].w*common_modes[j].h < w*h)) {
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

	if (video_modes)
		delete video_modes;
	/* Combine the detected modes and the common modes */
	video_modes = (SDL_Rect*)calloc(modes,sizeof(SDL_Rect));
	int k = 0;

	for (unsigned int i=0; detect_modes[i]; ++i) {
		if (detect_modes[i]->w >= w && detect_modes[i]->h >= h) {
			video_modes[k].w = detect_modes[i]->w;
			video_modes[k].h = detect_modes[i]->h;
			k++;
		}
	}
	for (unsigned int j=0; j<cm_count; ++j) {
		for (unsigned int i=0; detect_modes[i]; i++) {
			if(common_modes[j].w != 0) {
				if (detect_modes[i]->w >= w && detect_modes[i]->h >= h) {
					if ((common_modes[j].w == detect_modes[i]->w && common_modes[j].h == detect_modes[i]->h) || (common_modes[j].w*common_modes[j].h < w*h)) {
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
			if (video_modes[index_of_min].w*video_modes[index_of_min].h < video_modes[y].w*video_modes[y].h) {
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

void GameStateConfig::refreshFont() {
	delete font;
	font = new FontEngine();
}

/**
 * Tries to apply the selected video settings, reverting back to the old settings upon failure
 */
bool GameStateConfig::applyVideoSettings(SDL_Surface *src, int width, int height) {
	if (MIN_VIEW_W != -1 && MIN_VIEW_H != -1) {
		fprintf (stderr, "A mod is requiring a minimum resolution of %dx%d\n", MIN_VIEW_W, MIN_VIEW_H);
		if (width < MIN_VIEW_W) width = MIN_VIEW_W;
		if (height < MIN_VIEW_H) height = MIN_VIEW_H;
	}

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
	for (int i=0; i<settings_lstb[2]->getSize(); i++) {
		if (settings_lstb[2]->selected[i]) {
			settings_lstb[1]->append(settings_lstb[2]->getValue(i),settings_lstb[2]->getTooltip(i));
			settings_lstb[2]->remove(i);
			i--;
		}
	}
}

/**
 * Deactivate mods
 */
void GameStateConfig::disableMods() {
	for (int i=0; i<settings_lstb[1]->getSize(); i++) {
		if (settings_lstb[1]->selected[i] && settings_lstb[1]->getValue(i) != FALLBACK_MOD) {
			settings_lstb[2]->append(settings_lstb[1]->getValue(i),settings_lstb[1]->getTooltip(i));
			settings_lstb[1]->remove(i);
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
	for (int i=0; i<settings_lstb[1]->getSize(); i++) {
		if (settings_lstb[1]->getValue(i) != "") mods->mod_list.push_back(settings_lstb[1]->getValue(i));
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

