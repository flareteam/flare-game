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

#ifndef GAMESTATECONFIG_H
#define GAMESTATECONFIG_H

#include <vector>
#include "GameState.h"
#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetComboBox.h"
#include "WidgetSlider.h"
#include "WidgetTabControl.h"
#include "WidgetInput.h"

class GameStateConfig : public GameState {
public:
	GameStateConfig    ();
	~GameStateConfig   ();

	void    logic   ();
	void    render  ();

private:
	int optiontab[78];
	SDL_Rect** video_modes;

	std::string * language_ISO;
	std::string * language_full;

	int getVideoModes(void);
	bool getLanguagesList(void);
	int getLanguagesNumber(void);
	void update();

	std::vector<Widget*>      child_widget;
	WidgetTabControl    * tabControl;
	WidgetButton        * ok_button;
	WidgetButton        * defaults_button;
	WidgetButton        * cancel_button;

	WidgetLabel         * settings_lb[39];
	WidgetSlider        * settings_sl[2];
	WidgetCheckBox      * settings_cb[6];
	WidgetInput         * settings_key[25];
	WidgetComboBox      * settings_cmb[3];
};

#endif

