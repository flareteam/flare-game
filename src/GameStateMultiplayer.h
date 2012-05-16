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

#ifndef GAMESTATEMULTIPLAYER_H
#define GAMESTATEMULTIPLAYER_H

#include <vector>
#include "GameState.h"
#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetComboBox.h"
#include "WidgetSlider.h"
#include "WidgetTabControl.h"
#include "WidgetInput.h"

class GameStateMultiPlayer : public GameState {
public:
	GameStateMultiPlayer    ();
	~GameStateMultiPlayer   ();

	void    logic   ();
	void    render  ();

private:

	void update();

	WidgetButton        * host_button;
	WidgetButton        * join_button;
	WidgetButton        * cancel_button;
	WidgetInput         * ip_setting;
	WidgetInput         * port_setting;
};

#endif

