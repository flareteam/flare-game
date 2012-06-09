/*
Copyright © 2011-2012 Clint Bellanger
Copyright © 2012 Justin Jacobs

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
 * class WidgetListBox
 */

#ifndef WIDGET_ListBox_H
#define WIDGET_ListBox_H

#include "Widget.h"
#include "WidgetLabel.h"
#include "WidgetScrollBar.h"
#include "WidgetTooltip.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>


class WidgetListBox : public Widget {
private:

	std::string fileName; // the path to the ListBoxs background image

	SDL_Surface *listboxs;
	Mix_Chunk *click;
	
	int listAmount;
	int listHeight;
	int cursor;
	bool hasScrollBar;
	int non_empty_slots;
	TooltipData tip_buf;
	std::string *values;
	std::string *tooltips;
	WidgetLabel *vlabels;
	SDL_Rect *rows;
	WidgetTooltip *tip;
	WidgetScrollBar *scrollbar;
	
public:
	WidgetListBox(int amount, int height, const std::string& _fileName);
	~WidgetListBox();

	void loadArt();
	bool checkClick();
	TooltipData checkTooltip(Point mouse);
	void append(std::string value, std::string tooltip);
	void remove(int index);
	void shiftUp(int index);
	void shiftDown(int index);
	std::string getValue(int index);
	std::string getTooltip(int index);
	void scrollUp();
	void scrollDown();
	void render();
	void refresh();

	SDL_Rect pos;
	bool pressed;
	bool *selected;
};

#endif
