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


#pragma once
#ifndef GAMESTATECONFIG_H
#define GAMESTATECONFIG_H

#include <vector>
#include "GameState.h"
#include "WidgetTooltip.h"
#include <string>

class MenuConfirm;
class Widget;
class WidgetButton;
class WidgetCheckBox;
class WidgetInput;
class WidgetLabel;
class WidgetListBox;
class WidgetScrollBox;
class WidgetSlider;
class WidgetTabControl;
class WidgetTooltip;

class GameStateConfig : public GameState {
public:
	GameStateConfig    ();
	~GameStateConfig   ();

	void    logic   ();
	void    render  ();

private:
	int optiontab[124];
	SDL_Rect frame;
	SDL_Rect* video_modes;

	SDL_Rect menuConfirm_area;
	std::string menuConfirm_align;

	std::vector<std::string> language_ISO;
	std::vector<std::string> language_full;

	int getVideoModes(void);
	bool getLanguagesList(void);
	int getLanguagesNumber(void);
	void init();
	void readConfig();
	void update();
	void setDefaultResolution();
	void refreshFont();
	bool applyVideoSettings(SDL_Surface *src, int width, int height);
	void enableMods();
	void disableMods();
	bool setMods();
	void scanKey(int button);

	std::vector<Widget*>      child_widget;
	WidgetTabControl    * tabControl;
	WidgetButton        * ok_button;
	WidgetButton        * defaults_button;
	WidgetButton        * cancel_button;
	SDL_Surface         * background;

	WidgetCheckBox      * fullscreen_cb;
	WidgetLabel         * fullscreen_lb;
	WidgetCheckBox      * mouse_move_cb;
	WidgetLabel         * mouse_move_lb;
	WidgetCheckBox      * combat_text_cb;
	WidgetLabel         * combat_text_lb;
	WidgetCheckBox      * hwsurface_cb;
	WidgetLabel         * hwsurface_lb;
	WidgetCheckBox      * doublebuf_cb;
	WidgetLabel         * doublebuf_lb;
	WidgetCheckBox      * enable_joystick_cb;
	WidgetLabel         * enable_joystick_lb;
	WidgetCheckBox      * texture_quality_cb;
	WidgetLabel         * texture_quality_lb;
	WidgetCheckBox      * change_gamma_cb;
	WidgetLabel         * change_gamma_lb;
	WidgetCheckBox      * animated_tiles_cb;
	WidgetLabel         * animated_tiles_lb;
	WidgetCheckBox      * mouse_aim_cb;
	WidgetLabel         * mouse_aim_lb;
	WidgetCheckBox      * show_fps_cb;
	WidgetLabel         * show_fps_lb;
	WidgetSlider        * music_volume_sl;
	WidgetLabel         * music_volume_lb;
	WidgetSlider        * sound_volume_sl;
	WidgetLabel         * sound_volume_lb;
	WidgetSlider        * gamma_sl;
	WidgetLabel         * gamma_lb;
	WidgetListBox       * resolution_lstb;
	WidgetLabel         * resolution_lb;
	WidgetListBox       * activemods_lstb;
	WidgetLabel         * activemods_lb;
	WidgetListBox       * inactivemods_lstb;
	WidgetLabel         * inactivemods_lb;
	WidgetListBox       * joystick_device_lstb;
	WidgetLabel         * joystick_device_lb;
	WidgetListBox       * language_lstb;
	WidgetLabel         * language_lb;
	WidgetLabel         * hws_note_lb;
	WidgetLabel         * dbuf_note_lb;
	WidgetLabel         * anim_tiles_note_lb;
	WidgetLabel         * test_note_lb;
	WidgetButton        * activemods_shiftup_btn;
	WidgetButton        * activemods_shiftdown_btn;
	WidgetButton        * activemods_deactivate_btn;
	WidgetButton        * inactivemods_activate_btn;

	WidgetLabel         * settings_lb[25];
	WidgetButton        * settings_key[50];

	WidgetScrollBox     * input_scrollbox;
	MenuConfirm         * input_confirm;
	MenuConfirm         * defaults_confirm;
	MenuConfirm         * resolution_confirm;

	WidgetTooltip       * tip;
	TooltipData         tip_buf;

	int input_key;
	int mods_total;
	bool check_resolution;
	SDL_Rect scrollpane;
	SDL_Color scrollpane_color;
	int scrollpane_contents;
	int old_view_w;
	int old_view_h;
	int resolution_confirm_ticks;
};

#endif

