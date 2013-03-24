/*
Copyright © 2012 Clint Bellanger
Copyright © 2012 davidriod
Copyright © 2012 Igor Paliychuk
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
#include "WidgetListBox.h"
#include "WidgetScrollBox.h"
#include "WidgetSlider.h"
#include "WidgetTabControl.h"

#include <sstream>

using namespace std;

GameStateConfig::GameStateConfig ()
	: GameState()
	, video_modes(NULL)
	, child_widget()
	, ok_button(NULL)
	, defaults_button(NULL)
	, cancel_button(NULL)
	, imgFileName(mods->locate("images/menus/config.png"))
	, tip_buf()
	, input_key(0)
	, check_resolution(true)
{
	// Load background image
	SDL_Surface * tmp = IMG_Load(imgFileName.c_str());
	if (!tmp) {
		fprintf(stderr, "Could not load image \"%s\"\n", imgFileName.c_str());
	} else {
		background = SDL_DisplayFormatAlpha(tmp);
		SDL_FreeSurface(tmp);
	}

	init();
	update();
}

void GameStateConfig::init() {

	tip = new WidgetTooltip();

	ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	defaults_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	ok_button->label = msg->get("OK");
	ok_button->pos.x = VIEW_W_HALF - ok_button->pos.w/2;
	ok_button->pos.y = VIEW_H - (cancel_button->pos.h*3);
	ok_button->refresh();

	defaults_button->label = msg->get("Defaults");
	defaults_button->pos.x = VIEW_W_HALF - defaults_button->pos.w/2;
	defaults_button->pos.y = VIEW_H - (cancel_button->pos.h*2);
	defaults_button->refresh();

	cancel_button->label = msg->get("Cancel");
	cancel_button->pos.x = VIEW_W_HALF - cancel_button->pos.w/2;
	cancel_button->pos.y = VIEW_H - (cancel_button->pos.h);
	cancel_button->refresh();

	vector<string> mod_dirs;
	getDirList(PATH_DATA + "mods", mod_dirs);
	mods_total = mod_dirs.size();
	// Remove active mods from the available mods list
	for (unsigned int i = 0; i<mods->mod_list.size(); i++) {
		for (unsigned int j = 0; j<mod_dirs.size(); j++) {
			if (mods->mod_list[i] == mod_dirs[j] || FALLBACK_MOD == mod_dirs[j]) mod_dirs[j].erase();
		}
	}

	fullscreen_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	fullscreen_lb = new WidgetLabel();
	mouse_move_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	mouse_move_lb = new WidgetLabel();
	combat_text_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	combat_text_lb = new WidgetLabel();
	hwsurface_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	hwsurface_lb = new WidgetLabel();
	doublebuf_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	doublebuf_lb = new WidgetLabel();
	enable_joystick_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	enable_joystick_lb = new WidgetLabel();
	texture_quality_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	texture_quality_lb = new WidgetLabel();
	change_gamma_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	change_gamma_lb = new WidgetLabel();
	animated_tiles_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	animated_tiles_lb = new WidgetLabel();
	mouse_aim_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	mouse_aim_lb = new WidgetLabel();
	show_fps_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
	show_fps_lb = new WidgetLabel();
	music_volume_sl = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
	music_volume_lb = new WidgetLabel();
	sound_volume_sl = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
	sound_volume_lb = new WidgetLabel();
	gamma_sl = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
	gamma_lb = new WidgetLabel();
	resolution_lb = new WidgetLabel();
	activemods_lstb = new WidgetListBox(mods_total, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	activemods_lb = new WidgetLabel();
	inactivemods_lstb = new WidgetListBox(mods_total, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	inactivemods_lb = new WidgetLabel();
	joystick_device_lstb = new WidgetListBox(SDL_NumJoysticks(), 10, mods->locate("images/menus/buttons/listbox_default.png"));
	joystick_device_lb = new WidgetLabel();
	language_lb = new WidgetLabel();
	hws_note_lb = new WidgetLabel();
	dbuf_note_lb = new WidgetLabel();
	anim_tiles_note_lb = new WidgetLabel();
	test_note_lb = new WidgetLabel();
	activemods_shiftup_btn = new WidgetButton(mods->locate("images/menus/buttons/up.png"));
	activemods_shiftdown_btn = new WidgetButton(mods->locate("images/menus/buttons/down.png"));
	activemods_deactivate_btn = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	inactivemods_activate_btn = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

	tabControl = new WidgetTabControl(6);
	tabControl->setMainArea(((VIEW_W - FRAME_W)/2)+3, (VIEW_H - FRAME_H)/2, FRAME_W, FRAME_H);
	frame = tabControl->getContentArea();

	// Define the header.
	tabControl->setTabTitle(0, msg->get("Video"));
	tabControl->setTabTitle(1, msg->get("Audio"));
	tabControl->setTabTitle(2, msg->get("Interface"));
	tabControl->setTabTitle(3, msg->get("Input"));
	tabControl->setTabTitle(4, msg->get("Keybindings"));
	tabControl->setTabTitle(5, msg->get("Mods"));
	tabControl->updateHeader();

	input_confirm = new MenuConfirm("",msg->get("Assign: "));
	defaults_confirm = new MenuConfirm(msg->get("Defaults"),msg->get("Reset ALL settings?"));
	resolution_confirm = new MenuConfirm(msg->get("OK"),msg->get("Use this resolution?"));

	// Allocate KeyBindings
	for (unsigned int i = 0; i < 25; i++) {
		 settings_lb[i] = new WidgetLabel();
		 settings_lb[i]->set(inpt->binding_name[i]);
		 settings_lb[i]->setJustify(JUSTIFY_RIGHT);
	}
	for (unsigned int i = 0; i < 50; i++) {
		 settings_key[i] = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
	}

	// Allocate resolution list box
	int resolutions = getVideoModes();
	if (resolutions < 1) fprintf(stderr, "Unable to get resolutions list!\n");
	resolution_lstb = new WidgetListBox(resolutions, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	resolution_lstb->can_deselect = false;

	// Allocate Languages ListBox
	int langCount = getLanguagesNumber();
	language_ISO = std::vector<std::string>();
	language_full = std::vector<std::string>();
	language_ISO.resize(langCount);
	language_full.resize(langCount);
	language_lstb = new WidgetListBox(langCount, 10, mods->locate("images/menus/buttons/listbox_default.png"));
	language_lstb->can_deselect = false;

	readConfig();

	// Finish Mods ListBoxes setup
	activemods_lstb->multi_select = true;
	for (unsigned int i = 0; i < mods->mod_list.size() ; i++) {
		if (mods->mod_list[i] != FALLBACK_MOD)
			activemods_lstb->append(mods->mod_list[i],"");
	}
	child_widget.push_back(activemods_lstb);
	optiontab[child_widget.size()-1] = 5;

	inactivemods_lstb->multi_select = true;
	for (unsigned int i = 0; i < mod_dirs.size(); i++) {
		inactivemods_lstb->append(mod_dirs[i],"");
	}
	child_widget.push_back(inactivemods_lstb);
	optiontab[child_widget.size()-1] = 5;

	// Save the current resolution in case we want to revert back to it
	old_view_w = VIEW_W;
	old_view_h = VIEW_H;

	resolution_confirm_ticks = 0;
}

void GameStateConfig::readConfig () {
	//Load the menu configuration from file

	int offset_x = 0;
	int offset_y = 0;

	FileParser infile;
	if (infile.open(mods->locate("menus/config.txt"))) {
		while (infile.next()) {

			infile.val = infile.val + ',';
			int x1 = eatFirstInt(infile.val, ',');
			int y1 = eatFirstInt(infile.val, ',');
			int x2 = eatFirstInt(infile.val, ',');
			int y2 = eatFirstInt(infile.val, ',');

			int setting_num = -1;

			if (infile.key == "listbox_scrollbar_offset") {
				activemods_lstb->scrollbar_offset = x1;
				inactivemods_lstb->scrollbar_offset = x1;
				joystick_device_lstb->scrollbar_offset = x1;
				resolution_lstb->scrollbar_offset = x1;
				language_lstb->scrollbar_offset = x1;
			}
			//checkboxes
			else if (infile.key == "fullscreen") {
				fullscreen_cb->pos.x = frame.x + x2;
				fullscreen_cb->pos.y = frame.y + y2;
				child_widget.push_back(fullscreen_cb);
				optiontab[child_widget.size()-1] = 0;

				fullscreen_lb->setX(frame.x + x1);
				fullscreen_lb->setY(frame.y + y1);
				fullscreen_lb->set(msg->get("Full Screen Mode"));
				fullscreen_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(fullscreen_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "mouse_move") {
				mouse_move_cb->pos.x = frame.x + x2;
				mouse_move_cb->pos.y = frame.y + y2;
				child_widget.push_back(mouse_move_cb);
				optiontab[child_widget.size()-1] = 3;

				mouse_move_lb->setX(frame.x + x1);
				mouse_move_lb->setY(frame.y + y1);
				mouse_move_lb->set(msg->get("Move hero using mouse"));
				mouse_move_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(mouse_move_lb);
				optiontab[child_widget.size()-1] = 3;
			}
			else if (infile.key == "combat_text") {
				combat_text_cb->pos.x = frame.x + x2;
				combat_text_cb->pos.y = frame.y + y2;
				child_widget.push_back(combat_text_cb);
				optiontab[child_widget.size()-1] = 2;

				combat_text_lb->setX(frame.x + x1);
				combat_text_lb->setY(frame.y + y1);
				combat_text_lb->set(msg->get("Show combat text"));
				combat_text_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(combat_text_lb);
				optiontab[child_widget.size()-1] = 2;

			}
			else if (infile.key == "hwsurface") {
				hwsurface_cb->pos.x = frame.x + x2;
				hwsurface_cb->pos.y = frame.y + y2;
				child_widget.push_back(hwsurface_cb);
				optiontab[child_widget.size()-1] = 0;

				hwsurface_lb->setX(frame.x + x1);
				hwsurface_lb->setY(frame.y + y1);
				hwsurface_lb->set(msg->get("Hardware surfaces"));
				hwsurface_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(hwsurface_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "doublebuf") {
				doublebuf_cb->pos.x = frame.x + x2;
				doublebuf_cb->pos.y = frame.y + y2;
				child_widget.push_back(doublebuf_cb);
				optiontab[child_widget.size()-1] = 0;

				doublebuf_lb->setX(frame.x + x1);
				doublebuf_lb->setY(frame.y + y1);
				doublebuf_lb->set(msg->get("Double buffering"));
				doublebuf_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(doublebuf_lb);
				optiontab[child_widget.size()-1] = 0;

			}
			else if (infile.key == "enable_joystick") {
				enable_joystick_cb->pos.x = frame.x + x2;
				enable_joystick_cb->pos.y = frame.y + y2;
				child_widget.push_back(enable_joystick_cb);
				optiontab[child_widget.size()-1] = 3;

				enable_joystick_lb->setX(frame.x + x1);
				enable_joystick_lb->setY(frame.y + y1);
				enable_joystick_lb->set(msg->get("Use joystick"));
				enable_joystick_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(enable_joystick_lb);
				optiontab[child_widget.size()-1] = 3;
			}
			else if (infile.key == "texture_quality") {
				texture_quality_cb->pos.x = frame.x + x2;
				texture_quality_cb->pos.y = frame.y + y2;
				child_widget.push_back(texture_quality_cb);
				optiontab[child_widget.size()-1] = 0;

				texture_quality_lb->setX(frame.x + x1);
				texture_quality_lb->setY(frame.y + y1);
				texture_quality_lb->set(msg->get("High Quality Textures"));
				texture_quality_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(texture_quality_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "change_gamma") {
				change_gamma_cb->pos.x = frame.x + x2;
				change_gamma_cb->pos.y = frame.y + y2;
				child_widget.push_back(change_gamma_cb);
				optiontab[child_widget.size()-1] = 0;

				change_gamma_lb->setX(frame.x + x1);
				change_gamma_lb->setY(frame.y + y1);
				change_gamma_lb->set(msg->get("Allow changing gamma"));
				change_gamma_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(change_gamma_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "animated_tiles") {
				animated_tiles_cb->pos.x = frame.x + x2;
				animated_tiles_cb->pos.y = frame.y + y2;
				child_widget.push_back(animated_tiles_cb);
				optiontab[child_widget.size()-1] = 0;

				animated_tiles_lb->setX(frame.x + x1);
				animated_tiles_lb->setY(frame.y + y1);
				animated_tiles_lb->set(msg->get("Animated tiles"));
				animated_tiles_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(animated_tiles_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "mouse_aim") {
				mouse_aim_cb->pos.x = frame.x + x2;
				mouse_aim_cb->pos.y = frame.y + y2;
				child_widget.push_back(mouse_aim_cb);
				optiontab[child_widget.size()-1] = 3;

				mouse_aim_lb->setX(frame.x + x1);
				mouse_aim_lb->setY(frame.y + y1);
				mouse_aim_lb->set(msg->get("Mouse aim"));
				mouse_aim_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(mouse_aim_lb);
				optiontab[child_widget.size()-1] = 3;
			}
			else if (infile.key == "show_fps") {
				show_fps_cb->pos.x = frame.x + x2;
				show_fps_cb->pos.y = frame.y + y2;
				child_widget.push_back(show_fps_cb);
				optiontab[child_widget.size()-1] = 2;

				show_fps_lb->setX(frame.x + x1);
				show_fps_lb->setY(frame.y + y1);
				show_fps_lb->set(msg->get("Show FPS"));
				show_fps_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(show_fps_lb);
				optiontab[child_widget.size()-1] = 2;
			}
			//sliders
			else if (infile.key == "music_volume") {
				music_volume_sl->pos.x = frame.x + x2;
				music_volume_sl->pos.y = frame.y + y2;
				child_widget.push_back(music_volume_sl);
				optiontab[child_widget.size()-1] = 1;

				music_volume_lb->setX(frame.x + x1);
				music_volume_lb->setY(frame.y + y1);
				music_volume_lb->set(msg->get("Music Volume"));
				music_volume_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(music_volume_lb);
				optiontab[child_widget.size()-1] = 1;
			}
			else if (infile.key == "sound_volume") {
				sound_volume_sl->pos.x = frame.x + x2;
				sound_volume_sl->pos.y = frame.y + y2;
				child_widget.push_back(sound_volume_sl);
				optiontab[child_widget.size()-1] = 1;

				sound_volume_lb->setX(frame.x + x1);
				sound_volume_lb->setY(frame.y + y1);
				sound_volume_lb->set(msg->get("Sound Volume"));
				sound_volume_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(sound_volume_lb);
				optiontab[child_widget.size()-1] = 1;
			}
			else if (infile.key == "gamma") {
				gamma_sl->pos.x = frame.x + x2;
				gamma_sl->pos.y = frame.y + y2;
				child_widget.push_back(gamma_sl);
				optiontab[child_widget.size()-1] = 0;

				gamma_lb->setX(frame.x + x1);
				gamma_lb->setY(frame.y + y1);
				gamma_lb->set(msg->get("Gamma"));
				gamma_lb->setJustify(JUSTIFY_RIGHT);
				child_widget.push_back(gamma_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			//listboxes
			else if (infile.key == "resolution") {
				resolution_lstb->pos.x = frame.x + x2;
				resolution_lstb->pos.y = frame.y + y2;
				child_widget.push_back(resolution_lstb);
				optiontab[child_widget.size()-1] = 0;

				resolution_lb->setX(frame.x + x1);
				resolution_lb->setY(frame.y + y1);
				resolution_lb->set(msg->get("Resolution"));
				child_widget.push_back(resolution_lb);
				optiontab[child_widget.size()-1] = 0;

			}
			else if (infile.key == "activemods") {
				activemods_lstb->pos.x = frame.x + x2;
				activemods_lstb->pos.y = frame.y + y2;

				activemods_lb->setX(frame.x + x1);
				activemods_lb->setY(frame.y + y1);
				activemods_lb->set(msg->get("Active Mods"));
				child_widget.push_back(activemods_lb);
				optiontab[child_widget.size()-1] = 5;
			}
			else if (infile.key == "inactivemods") {
				inactivemods_lstb->pos.x = frame.x + x2;
				inactivemods_lstb->pos.y = frame.y + y2;

				inactivemods_lb->setX(frame.x + x1);
				inactivemods_lb->setY(frame.y + y1);
				inactivemods_lb->set(msg->get("Available Mods"));
				child_widget.push_back(inactivemods_lb);
				optiontab[child_widget.size()-1] = 5;
			}
			else if (infile.key == "joystick_device") {
				joystick_device_lstb->pos.x = frame.x + x2;
				joystick_device_lstb->pos.y = frame.y + y2;
				for(int i = 0; i < SDL_NumJoysticks(); i++) {
					if (SDL_JoystickName(i) != NULL)
						joystick_device_lstb->append(SDL_JoystickName(i),SDL_JoystickName(i));
				}
				child_widget.push_back(joystick_device_lstb);
				optiontab[child_widget.size()-1] = 3;

				joystick_device_lb->setX(frame.x + x1);
				joystick_device_lb->setY(frame.y + y1);
				joystick_device_lb->set(msg->get("Joystick"));
				child_widget.push_back(joystick_device_lb);
				optiontab[child_widget.size()-1] = 3;
			}
			else if (infile.key == "language") {
				language_lstb->pos.x = frame.x + x2;
				language_lstb->pos.y = frame.y + y2;
				child_widget.push_back(language_lstb);
				optiontab[child_widget.size()-1] = 2;

				language_lb->setX(frame.x + x1);
				language_lb->setY(frame.y + y1);
				language_lb->set(msg->get("Language"));
				child_widget.push_back(language_lb);
				optiontab[child_widget.size()-1] = 2;
			}
			// buttons begin
			else if (infile.key == "cancel") setting_num = CANCEL;
			else if (infile.key == "accept") setting_num = ACCEPT;
			else if (infile.key == "up") setting_num = UP;
			else if (infile.key == "down") setting_num = DOWN;
			else if (infile.key == "left") setting_num = LEFT;
			else if (infile.key == "right") setting_num = RIGHT;
			else if (infile.key == "bar1") setting_num = BAR_1;
			else if (infile.key == "bar2") setting_num = BAR_2;
			else if (infile.key == "bar3") setting_num = BAR_3;
			else if (infile.key == "bar4") setting_num = BAR_4;
			else if (infile.key == "bar5") setting_num = BAR_5;
			else if (infile.key == "bar6") setting_num = BAR_6;
			else if (infile.key == "bar7") setting_num = BAR_7;
			else if (infile.key == "bar8") setting_num = BAR_8;
			else if (infile.key == "bar9") setting_num = BAR_9;
			else if (infile.key == "bar0") setting_num = BAR_0;
			else if (infile.key == "main1") setting_num = MAIN1;
			else if (infile.key == "main2") setting_num = MAIN2;
			else if (infile.key == "character") setting_num = CHARACTER;
			else if (infile.key == "inventory") setting_num = INVENTORY;
			else if (infile.key == "powers") setting_num = POWERS;
			else if (infile.key == "log") setting_num = LOG;
			else if (infile.key == "ctrl") setting_num = CTRL;
			else if (infile.key == "shift") setting_num = SHIFT;
			else if (infile.key == "delete") setting_num = DEL;
			// buttons end

			else if (infile.key == "hws_note") {
				hws_note_lb->setX(frame.x + x1);
				hws_note_lb->setY(frame.y + y1);
				hws_note_lb->set(msg->get("Disable for performance"));
				child_widget.push_back(hws_note_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "dbuf_note") {
				dbuf_note_lb->setX(frame.x + x1);
				dbuf_note_lb->setY(frame.y + y1);
				dbuf_note_lb->set(msg->get("Disable for performance"));
				child_widget.push_back(dbuf_note_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "anim_tiles_note") {
				anim_tiles_note_lb->setX(frame.x + x1);
				anim_tiles_note_lb->setY(frame.y + y1);
				anim_tiles_note_lb->set(msg->get("Disable for performance"));
				child_widget.push_back(anim_tiles_note_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			else if (infile.key == "test_note") {
				test_note_lb->setX(frame.x + x1);
				test_note_lb->setY(frame.y + y1);
				test_note_lb->set(msg->get("Experimental"));
				child_widget.push_back(test_note_lb);
				optiontab[child_widget.size()-1] = 0;
			}
			//buttons
			else if (infile.key == "activemods_shiftup") {
				activemods_shiftup_btn->pos.x = frame.x + x1;
				activemods_shiftup_btn->pos.y = frame.y + y1;
				activemods_shiftup_btn->refresh();
				child_widget.push_back(activemods_shiftup_btn);
				optiontab[child_widget.size()-1] = 5;
			}
			else if (infile.key == "activemods_shiftdown") {
				activemods_shiftdown_btn->pos.x = frame.x + x1;
				activemods_shiftdown_btn->pos.y = frame.y + y1;
				activemods_shiftdown_btn->refresh();
				child_widget.push_back(activemods_shiftdown_btn);
				optiontab[child_widget.size()-1] = 5;
			}
			else if (infile.key == "activemods_deactivate") {
				activemods_deactivate_btn->label = msg->get("<< Disable");
				activemods_deactivate_btn->pos.x = frame.x + x1;
				activemods_deactivate_btn->pos.y = frame.y + y1;
				activemods_deactivate_btn->refresh();
				child_widget.push_back(activemods_deactivate_btn);
				optiontab[child_widget.size()-1] = 5;
			}
			else if (infile.key == "inactivemods_activate") {
				inactivemods_activate_btn->label = msg->get("Enable >>");
				inactivemods_activate_btn->pos.x = frame.x + x1;
				inactivemods_activate_btn->pos.y = frame.y + y1;
				inactivemods_activate_btn->refresh();
				child_widget.push_back(inactivemods_activate_btn);
				optiontab[child_widget.size()-1] = 5;
			}
			else if (infile.key == "secondary_offset") {
				offset_x = x1;
				offset_y = y1;
			}
			else if (infile.key == "keybinds_bg_color") {
				// background color for keybinds scrollbox
				scrollpane_color.r = x1;
				scrollpane_color.g = y1;
				scrollpane_color.b = x2;
			}
			else if (infile.key == "scrollpane") {
				scrollpane.x = x1;
				scrollpane.y = y1;
				scrollpane.w = x2;
				scrollpane.h = y2;
			}
			else if (infile.key == "scrollpane_contents") {
				scrollpane_contents = x1;
			}

			if (setting_num > -1 && setting_num < 25) {
					//keybindings
					settings_lb[setting_num]->setX(x1);
					settings_lb[setting_num]->setY(y1);
					settings_key[setting_num]->pos.x = x2;
					settings_key[setting_num]->pos.y = y2;
			}

		  }
		  infile.close();
		} else fprintf(stderr, "Unable to open menus/config.txt!\n");

	// Load the MenuConfirm positions and alignments from menus/menus.txt
	if (infile.open(mods->locate("menus/menus.txt"))) {
		int menu_index = -1;
		while (infile.next()) {
			if (infile.key == "id") {
				if (infile.val == "confirm") menu_index = 0;
				else menu_index = -1;
			}

			if (menu_index == -1)
				continue;

			if (infile.key == "layout") {
				infile.val = infile.val + ',';
				menuConfirm_area.x = eatFirstInt(infile.val, ',');
				menuConfirm_area.y = eatFirstInt(infile.val, ',');
				menuConfirm_area.w = eatFirstInt(infile.val, ',');
				menuConfirm_area.h = eatFirstInt(infile.val, ',');
			}

			if (infile.key == "align") {
				menuConfirm_align = infile.val;
			}
		}
		infile.close();
	} else fprintf(stderr, "Unable to open menus/menus.txt!\n");

	defaults_confirm->window_area = menuConfirm_area;
	defaults_confirm->alignment = menuConfirm_align;
	defaults_confirm->align();
	defaults_confirm->update();

	resolution_confirm->window_area = menuConfirm_area;
	resolution_confirm->alignment = menuConfirm_align;
	resolution_confirm->align();
	resolution_confirm->update();

	// Allocate KeyBindings ScrollBox
	input_scrollbox = new WidgetScrollBox(scrollpane.w, scrollpane.h);
	input_scrollbox->pos.x = scrollpane.x + frame.x;
	input_scrollbox->pos.y = scrollpane.y + frame.y;
	input_scrollbox->resize(scrollpane_contents);
	input_scrollbox->bg.r = scrollpane_color.r;
	input_scrollbox->bg.g = scrollpane_color.g;
	input_scrollbox->bg.b = scrollpane_color.b;

	// Set positions of secondary key bindings
	for (unsigned int i = 25; i < 50; i++) {
		settings_key[i]->pos.x = settings_key[i-25]->pos.x + offset_x;
		settings_key[i]->pos.y = settings_key[i-25]->pos.y + offset_y;
	}
}

void GameStateConfig::update () {
	if (FULLSCREEN) fullscreen_cb->Check();
	else fullscreen_cb->unCheck();
	if (AUDIO) {
		music_volume_sl->set(0,128,MUSIC_VOLUME);
		Mix_VolumeMusic(MUSIC_VOLUME);
		sound_volume_sl->set(0,128,SOUND_VOLUME);
		Mix_Volume(-1, SOUND_VOLUME);
	} else {
		music_volume_sl->set(0,128,0);
		sound_volume_sl->set(0,128,0);
	}
	if (MOUSE_MOVE) mouse_move_cb->Check();
	else mouse_move_cb->unCheck();
	if (COMBAT_TEXT) combat_text_cb->Check();
	else combat_text_cb->unCheck();
	if (HWSURFACE) hwsurface_cb->Check();
	else hwsurface_cb->unCheck();
	if (DOUBLEBUF) doublebuf_cb->Check();
	else doublebuf_cb->unCheck();
	if (ENABLE_JOYSTICK) enable_joystick_cb->Check();
	else enable_joystick_cb->unCheck();
	if (TEXTURE_QUALITY) texture_quality_cb->Check();
	else texture_quality_cb->unCheck();
	if (CHANGE_GAMMA) change_gamma_cb->Check();
	else {
		change_gamma_cb->unCheck();
		GAMMA = 1.0;
	}
	gamma_sl->set(5,20,(int)(GAMMA*10.0));
	SDL_SetGamma(GAMMA,GAMMA,GAMMA);

	if (ANIMATED_TILES) animated_tiles_cb->Check();
	else animated_tiles_cb->unCheck();
	if (MOUSE_AIM) mouse_aim_cb->Check();
	else mouse_aim_cb->unCheck();
	if (SHOW_FPS) show_fps_cb->Check();
	else show_fps_cb->unCheck();

	std::stringstream list_mode;
	unsigned int resolutions = getVideoModes();
	for (unsigned int i=0; i<resolutions; ++i) {
		list_mode << video_modes[i].w << "x" << video_modes[i].h;
		resolution_lstb->append(list_mode.str(),"");
		if (video_modes[i].w == VIEW_W && video_modes[i].h == VIEW_H) resolution_lstb->selected[i] = true;
		else resolution_lstb->selected[i] = false;
		list_mode.str("");
	}
	resolution_lstb->refresh();

	SDL_Init(SDL_INIT_JOYSTICK);
	if (ENABLE_JOYSTICK && SDL_NumJoysticks() > 0) {
		SDL_JoystickClose(joy);
		joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
		joystick_device_lstb->selected[JOYSTICK_DEVICE] = true;
	}
	joystick_device_lstb->refresh();

	if (!getLanguagesList()) fprintf(stderr, "Unable to get languages list!\n");
	for (int i=0; i < getLanguagesNumber(); i++) {
		language_lstb->append(language_full[i],"");
		if (language_ISO[i] == LANGUAGE) language_lstb->selected[i] = true;
	}
	language_lstb->refresh();

	activemods_lstb->refresh();
	inactivemods_lstb->refresh();

	for (unsigned int i = 0; i < 25; i++) {
		if (inpt->binding[i] < 8) {
			settings_key[i]->label = inpt->mouse_button[inpt->binding[i]-1];
		} else {
			settings_key[i]->label = SDL_GetKeyName((SDLKey)inpt->binding[i]);
		}
		settings_key[i]->refresh();
	}
	for (unsigned int i = 25; i < 50; i++) {
		if (inpt->binding_alt[i-25] < 8) {
			settings_key[i]->label = inpt->mouse_button[inpt->binding_alt[i-25]-1];
		} else {
			settings_key[i]->label = SDL_GetKeyName((SDLKey)inpt->binding_alt[i-25]);
		}
		settings_key[i]->refresh();
	}
	input_scrollbox->refresh();
}

void GameStateConfig::logic ()
{
	check_resolution = true;

	std::string resolution_value;
	resolution_value = resolution_lstb->getValue() + 'x'; // add x to have last element readable
	int width = eatFirstInt(resolution_value, 'x');
	int height = eatFirstInt(resolution_value, 'x');

	// In case of a custom resolution, the listbox might have nothing selected
	// So we just use whatever the current view area is
	if (width == 0 || height == 0) {
		width = VIEW_W;
		height = VIEW_H;
	}

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

	if (resolution_confirm->visible || resolution_confirm->cancelClicked) {
		resolution_confirm->logic();
		resolution_confirm_ticks--;
		if (resolution_confirm->confirmClicked) {
			saveSettings();
			delete requestedGameState;
			requestedGameState = new GameStateTitle();
		} else if (resolution_confirm->cancelClicked || resolution_confirm_ticks == 0) {
			applyVideoSettings(screen, old_view_w, old_view_h);
			saveSettings();
			delete requestedGameState;
			requestedGameState = new GameStateConfig();
		}
	}

	if (!input_confirm->visible && !defaults_confirm->visible && !resolution_confirm->visible) {
		tabControl->logic();

		// Ok/Cancel Buttons
		if (ok_button->checkClick()) {
			inpt->saveKeyBindings();
			inpt->setKeybindNames();
			if (setMods()) {
				reload_music = true;
				delete mods;
				mods = new ModManager();
				loadTilesetSettings();
			}
			loadMiscSettings();
			refreshFont();
			if ((ENABLE_JOYSTICK) && (SDL_NumJoysticks() > 0)) {
				SDL_JoystickClose(joy);
				joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
			}
			applyVideoSettings(screen, width, height);
			if (width != old_view_w || height != old_view_h) {
				resolution_confirm->window_area = menuConfirm_area;
				resolution_confirm->align();
				resolution_confirm->update();
				resolution_confirm_ticks = MAX_FRAMES_PER_SEC * 10; // 10 seconds
			} else {
				saveSettings();
				delete requestedGameState;
				requestedGameState = new GameStateTitle();
			}
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
		if (fullscreen_cb->checkClick()) {
			if (fullscreen_cb->isChecked()) FULLSCREEN=true;
			else FULLSCREEN=false;
		} else if (hwsurface_cb->checkClick()) {
			if (hwsurface_cb->isChecked()) HWSURFACE=true;
			else HWSURFACE=false;
		} else if (doublebuf_cb->checkClick()) {
			if (doublebuf_cb->isChecked()) DOUBLEBUF=true;
			else DOUBLEBUF=false;
		} else if (texture_quality_cb->checkClick()) {
			if (texture_quality_cb->isChecked()) TEXTURE_QUALITY=true;
			else TEXTURE_QUALITY=false;
		} else if (change_gamma_cb->checkClick()) {
			if (change_gamma_cb->isChecked()) CHANGE_GAMMA=true;
			else {
				CHANGE_GAMMA=false;
				GAMMA = 1.0;
				gamma_sl->set(5,20,(int)(GAMMA*10.0));
				SDL_SetGamma(GAMMA,GAMMA,GAMMA);
			}
		} else if (animated_tiles_cb->checkClick()) {
			if (animated_tiles_cb->isChecked()) ANIMATED_TILES=true;
			else ANIMATED_TILES=false;
		} else if (resolution_lstb->checkClick()) {
			; // nothing to do here: resolution value changes next frame.
		} else if (CHANGE_GAMMA) {
			if (gamma_sl->checkClick()) {
					GAMMA=(gamma_sl->getValue())*0.1f;
					SDL_SetGamma(GAMMA,GAMMA,GAMMA);
			}
		}
	}
	// tab 1 (audio)
	else if (active_tab == 1 && !defaults_confirm->visible) {
		if (AUDIO) {
			if (music_volume_sl->checkClick()) {
				if (MUSIC_VOLUME == 0)
					reload_music = true;
				MUSIC_VOLUME=music_volume_sl->getValue();
				Mix_VolumeMusic(MUSIC_VOLUME);
			} else if (sound_volume_sl->checkClick()) {
				SOUND_VOLUME=sound_volume_sl->getValue();
				Mix_Volume(-1, SOUND_VOLUME);
			}
		}
	}
	// tab 2 (interface)
	else if (active_tab == 2 && !defaults_confirm->visible) {
		if (combat_text_cb->checkClick()) {
			if (combat_text_cb->isChecked()) COMBAT_TEXT=true;
			else COMBAT_TEXT=false;
		} else if (language_lstb->checkClick()) {
			LANGUAGE = language_ISO[language_lstb->getSelected()];
			delete msg;
			msg = new MessageEngine();
		} else if (show_fps_cb->checkClick()) {
			if (show_fps_cb->isChecked()) SHOW_FPS=true;
			else SHOW_FPS=false;
		}
	}
	// tab 3 (input)
	else if (active_tab == 3 && !defaults_confirm->visible) {
		if (mouse_move_cb->checkClick()) {
			if (mouse_move_cb->isChecked()) MOUSE_MOVE=true;
			else MOUSE_MOVE=false;
		} else if (mouse_aim_cb->checkClick()) {
			if (mouse_aim_cb->isChecked()) MOUSE_AIM=true;
			else MOUSE_AIM=false;
		} else if (enable_joystick_cb->checkClick()) {
			if (enable_joystick_cb->isChecked()) {
				ENABLE_JOYSTICK=true;
				inpt->mouse_emulation = true;
				if (SDL_NumJoysticks() > 0) {
					JOYSTICK_DEVICE = 0;
					SDL_JoystickClose(joy);
					joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
					joystick_device_lstb->selected[JOYSTICK_DEVICE] = true;
				}
			} else {
				ENABLE_JOYSTICK=false;
				inpt->mouse_emulation = false;
				for (int i=0; i<joystick_device_lstb->getSize(); i++)
					joystick_device_lstb->selected[i] = false;
			}
			if (SDL_NumJoysticks() > 0) joystick_device_lstb->refresh();
		} else if (joystick_device_lstb->checkClick()) {
			JOYSTICK_DEVICE = joystick_device_lstb->getSelected();
			if (JOYSTICK_DEVICE != -1) {
				enable_joystick_cb->Check();
				ENABLE_JOYSTICK=true;
				inpt->mouse_emulation = true;
				if (SDL_NumJoysticks() > 0) {
					SDL_JoystickClose(joy);
					joy = SDL_JoystickOpen(JOYSTICK_DEVICE);
				}
			} else {
				enable_joystick_cb->unCheck();
				ENABLE_JOYSTICK = false;
				inpt->mouse_emulation = false;
			}
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
							confirm_msg = msg->get("Assign: ") + inpt->binding_name[i];
						else
							confirm_msg = msg->get("Assign: ") + inpt->binding_name[i-25];
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
		if (activemods_lstb->checkClick()) {
			//do nothing
		} else if (inactivemods_lstb->checkClick()) {
			//do nothing
		} else if (activemods_shiftup_btn->checkClick()) {
			activemods_lstb->shiftUp();
		} else if (activemods_shiftdown_btn->checkClick()) {
			activemods_lstb->shiftDown();
		} else if (activemods_deactivate_btn->checkClick()) {
			disableMods();
		} else if (inactivemods_activate_btn->checkClick()) {
			enableMods();
		}
	}
}

void GameStateConfig::render ()
{
	if (resolution_confirm->visible) {
		resolution_confirm->render();
		return;
	}

	int tabheight = tabControl->getTabHeight();
	SDL_Rect	pos;
	pos.x = (VIEW_W-FRAME_W)/2;
	pos.y = (VIEW_H-FRAME_H)/2 + tabheight - tabheight/16;

	SDL_BlitSurface(background,NULL,screen,&pos);

	tabControl->render();

	// render OK/Defaults/Cancel buttons
	ok_button->render();
	cancel_button->render();
	defaults_button->render();

	int active_tab = tabControl->getActiveTab();

	// render keybindings tab
	if (active_tab == 4) {
		if (input_scrollbox->update) input_scrollbox->refresh();
		for (unsigned int i = 0; i < 25; i++) {
			if (input_scrollbox->update) settings_lb[i]->render(input_scrollbox->contents);
		}
		for (unsigned int i = 0; i < 50; i++) {
			if (input_scrollbox->update) settings_key[i]->render(input_scrollbox->contents);
		}
		input_scrollbox->render();
	}

	for (unsigned int i = 0; i < child_widget.size(); i++) {
		 if (optiontab[i] == active_tab) child_widget[i]->render();
	}

	if (input_confirm->visible) input_confirm->render();
	if (defaults_confirm->visible) defaults_confirm->render();

	// Get tooltips for listboxes
	// This isn't very elegant right now
	// In the future, we'll want to get tooltips for all widget types
	TooltipData tip_new;
	if (active_tab == 0 && tip_new.isEmpty()) tip_new = resolution_lstb->checkTooltip(inpt->mouse);
	if (active_tab == 2 && tip_new.isEmpty()) tip_new = language_lstb->checkTooltip(inpt->mouse);
	if (active_tab == 3 && tip_new.isEmpty()) tip_new = joystick_device_lstb->checkTooltip(inpt->mouse);
	if (active_tab == 5 && tip_new.isEmpty()) tip_new = activemods_lstb->checkTooltip(inpt->mouse);
	if (active_tab == 5 && tip_new.isEmpty()) tip_new = inactivemods_lstb->checkTooltip(inpt->mouse);

	if (!tip_new.isEmpty()) {
		if (!tip_new.compare(&tip_buf)) {
			tip_buf.clear();
			tip_buf = tip_new;
		}
		tip->render(tip_buf, inpt->mouse, STYLE_FLOAT);
	}

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
		free(video_modes);
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
			infile.close();
		} else fprintf(stderr, "Unable to open engine/languages.txt!\n");

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
			infile.close();
		} else fprintf(stderr, "Unable to open engine/languages.txt!\n");

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
	if (MIN_VIEW_W > width && MIN_VIEW_H > height) {
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

		SDL_SetVideoMode (tmp_w, tmp_h, 0, flags);

		return false;
	} else {

		// If the new settings succeed, adjust the view area
		VIEW_W = width;
		VIEW_W_HALF = width/2;
		VIEW_H = height;
		VIEW_H_HALF = height/2;

		resolution_confirm->visible = true;

		return true;
	}
}

/**
 * Activate mods
 */
void GameStateConfig::enableMods() {
	for (int i=0; i<inactivemods_lstb->getSize(); i++) {
		if (inactivemods_lstb->selected[i]) {
			activemods_lstb->append(inactivemods_lstb->getValue(i),inactivemods_lstb->getTooltip(i));
			inactivemods_lstb->remove(i);
			i--;
		}
	}
}

/**
 * Deactivate mods
 */
void GameStateConfig::disableMods() {
	for (int i=0; i<activemods_lstb->getSize(); i++) {
		if (activemods_lstb->selected[i] && activemods_lstb->getValue(i) != FALLBACK_MOD) {
			inactivemods_lstb->append(activemods_lstb->getValue(i),activemods_lstb->getTooltip(i));
			activemods_lstb->remove(i);
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
	for (int i=0; i<activemods_lstb->getSize(); i++) {
		if (activemods_lstb->getValue(i) != "") mods->mod_list.push_back(activemods_lstb->getValue(i));
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
	if (outfile.bad()) fprintf(stderr, "Unable to save mod list into file. No write access or disk is full!\n");
	outfile.close();
	outfile.clear();
	if (mods->mod_list != temp_list) return true;
	else return false;
}

/**
 * Scan key binding
 */
void GameStateConfig::scanKey(int button) {
  if (input_confirm->visible) {
	if (inpt->last_button != -1 && inpt->last_button < 8) {
		if (button < 25) inpt->binding[button] = inpt->last_button;
		else inpt->binding_alt[button-25] = inpt->last_button;

		settings_key[button]->label = inpt->mouse_button[inpt->last_button-1];
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

GameStateConfig::~GameStateConfig()
{
	tip_buf.clear();
	delete tip;
	delete tabControl;
	delete ok_button;
	delete defaults_button;
	delete cancel_button;
	delete input_scrollbox;
	delete input_confirm;
	delete defaults_confirm;
	delete resolution_confirm;

	SDL_FreeSurface(background);

	for (std::vector<Widget*>::iterator iter = child_widget.begin(); iter != child_widget.end(); ++iter)
	{
		delete (*iter);
	}
	child_widget.clear();

	for (unsigned int i = 0; i < 25; i++) {
		 delete settings_lb[i];
	}
	for (unsigned int i = 0; i < 50; i++) {
		 delete settings_key[i];
	}

	language_ISO.clear();
	language_full.clear();

	if (video_modes)
		free(video_modes);
}

