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

#include "WidgetListBox.h"
#include "SharedResources.h"

using namespace std;

WidgetListBox::WidgetListBox(int amount, int height, const std::string& _fileName)
	: fileName(_fileName) {

	listAmount = amount;
	listHeight = height;
	cursor = 0;
	hasScrollBar = false;
	non_empty_slots = 0;
	values = new std::string[listAmount];
	tooltips = new std::string[listAmount];
	vlabels = new WidgetLabel[listHeight];
	rows = new SDL_Rect[listHeight];
	tip = new WidgetTooltip();

	listboxs = NULL;
	click = NULL;
	pos.x = pos.y = pos.w = pos.h = 0;

	selected = new bool[listAmount];
	for (int i=0; i<listAmount; i++) {
		selected[i] = false;
	}
	
	loadArt();

	pos.w = listboxs->w;
	pos.h = (listboxs->h / 3); //height of one item

	scrollbar = new WidgetScrollBar(pos.x+pos.w, pos.y, pos.h*listHeight,mods->locate("images/menus/buttons/scrollbar_default.png"));
}

void WidgetListBox::loadArt() {

	// load ListBox images
	listboxs = IMG_Load(fileName.c_str());

	if(!listboxs) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
		exit(1); // or abort ??
	}
	
	// optimize
	SDL_Surface *cleanup = listboxs;
	listboxs = SDL_DisplayFormatAlpha(listboxs);
	SDL_FreeSurface(cleanup);
}

/**
 * Sets and releases the "pressed" visual state of the ListBox
 * If press and release, activate (return true)
 */
bool WidgetListBox::checkClick() {
	// check ScrollBar clicks
	if (hasScrollBar) {
		switch (scrollbar->checkClick()) {
			case 1:
				scrollUp();
				break;
			case 2:
				scrollDown();
				break;
			default:
				break;
		}
	}

	// main ListBox already in use, new click not allowed
	if (inpt->lock[MAIN1]) return false;

	// main click released, so the ListBox state goes back to unpressed
	if (pressed && !inpt->lock[MAIN1]) {
		pressed = false;
		
		for(int i=0; i<listHeight; i++) {
			if (i<listAmount) {
				if (isWithin(rows[i], inpt->mouse) && values[i+cursor] != "") {
					// activate upon release
					selected[i+cursor] = !selected[i+cursor];
					refresh();
					return true;
				}
			}
		}
	}

	pressed = false;

	// detect new click
	if (inpt->pressing[MAIN1]) {
		for (int i=0; i<listHeight;i++) {
			if (isWithin(rows[i], inpt->mouse)) {
			
				inpt->lock[MAIN1] = true;
				pressed = true;

			}
		}
	}
	return false;

}

/**
 * If mousing-over an item with a tooltip, return that tooltip data.
 *
 * @param mouse The x,y screen coordinates of the mouse cursor
 */
TooltipData WidgetListBox::checkTooltip(Point mouse) {
	TooltipData tip;

	for(int i=0; i<listHeight; i++) {
		if (i<listAmount) {
			if (isWithin(rows[i], mouse) && tooltips[i+cursor] != "") {
				tip.lines[tip.num_lines++] = tooltips[i+cursor];
				break;
			}
		}
	}

	return tip;
}

/**
 * Set the value and tooltip of the first available slot
 */
void WidgetListBox::append(std::string value, std::string tooltip) {
	for (int i=0;i<listAmount;i++) {
		if (values[i] == "") {
			values[i] = value;
			tooltips[i] = tooltip;
			refresh();
			return;
		}
	}
}

/**
 * Clear a slot at a specified index, shifting the other items accordingly
 */
void WidgetListBox::remove(int index) {
	for (int i=index;i<listAmount;i++) {
		if (i==listAmount-1) {
			selected[i] = false;
			values[i] = "";
			tooltips[i] = "";
		} else {
			selected[i] = selected[i+1];
			values[i] = values[i+1];
			tooltips[i] = tooltips[i+1];
		}
	}
	refresh();
}

/*
 * Move an item up on the list
 */
void WidgetListBox::shiftUp(int index) {
	if (index > 0) {
		bool tmp_selected = selected[index];
		std::string tmp_value = values[index];
		std::string tmp_tooltip = tooltips[index];

		selected[index] = selected[index-1];
		values[index] = values[index-1];
		tooltips[index] = tooltips[index-1];

		selected[index-1] = tmp_selected;
		values[index-1] = tmp_value;
		tooltips[index-1] = tmp_tooltip;

		scrollUp();
	}
}

/*
 * Move an item down on the list
 */
void WidgetListBox::shiftDown(int index) {
	if (index < listAmount-1) {
		bool tmp_selected = selected[index];
		std::string tmp_value = values[index];
		std::string tmp_tooltip = tooltips[index];

		selected[index] = selected[index+1];
		values[index] = values[index+1];
		tooltips[index] = tooltips[index+1];

		selected[index+1] = tmp_selected;
		values[index+1] = tmp_value;
		tooltips[index+1] = tmp_tooltip;

		scrollDown();
	}
}

/*
 * Get the item name at a specific index
 */
std::string WidgetListBox::getValue(int index) {
	return values[index];
}

/*
 * Get the item tooltip at a specific index
 */
std::string WidgetListBox::getTooltip(int index) {
	return tooltips[index];
}

/*
 * Shift the viewing area up
 */
void WidgetListBox::scrollUp() {
	if (cursor > 0)
		cursor -= 1;
	refresh();
}

/*
 * Shift the viewing area down
 */
void WidgetListBox::scrollDown() {
	if (cursor+listHeight < non_empty_slots)
		cursor += 1;
	refresh();
}

void WidgetListBox::render() {
	SDL_Rect src;
	src.x = 0;
	src.w = pos.w;
	src.h = pos.h;

	for(int i=0; i<listHeight; i++) {
		if(i==0)
			src.y = 0;
		else if(i==listHeight-1)
			src.y = pos.h*2;
		else
			src.y = pos.h;

		refresh();
		SDL_BlitSurface(listboxs, &src, screen, &rows[i]);
		if (i<listAmount) {
			vlabels[i].render();
		}
	}

	if (hasScrollBar)
		scrollbar->render();

	TooltipData tip_new = checkTooltip(inpt->mouse);
	if (tip_new.num_lines > 0) {
		if (tip_new.lines[0] != tip_buf.lines[0]) {
			tip->clear(tip_buf);
			tip_buf = tip_new;
		}
		tip->render(tip_buf, inpt->mouse, STYLE_FLOAT);
	}


}

/**
 * Create the text buffer
 * Also, toggle the scrollbar based on the size of the list
 */
void WidgetListBox::refresh() {
	non_empty_slots = 0;
	for (int i=0;i<listAmount;i++) {
		if (values[i] != "")
			non_empty_slots = i+1;
	}

	for(int i=0;i<listHeight;i++)
	{
		rows[i].x = pos.x;
		rows[i].y = ((pos.h-1)*i)+pos.y;
		rows[i].w = pos.w;
		rows[i].h = pos.h;

		int font_x = rows[i].x + (rows[i].w/2);
		int font_y = rows[i].y + (rows[i].h/2);

		if (i<listAmount) {
			if(selected[i+cursor]) {
				vlabels[i].set(font_x, font_y, JUSTIFY_CENTER, VALIGN_CENTER, values[i+cursor], FONT_WHITE);
			} else {
				vlabels[i].set(font_x, font_y, JUSTIFY_CENTER, VALIGN_CENTER, values[i+cursor], FONT_GRAY);
			}
		}
	}

	if (non_empty_slots > listHeight) {
		hasScrollBar = true;
	} else {
		hasScrollBar = false;
	}
	scrollbar->refresh(pos.x+pos.w,pos.y);
}

WidgetListBox::~WidgetListBox() {
	tip->clear(tip_buf);
	SDL_FreeSurface(listboxs);
	delete[] values;
	delete[] tooltips;
	delete[] vlabels;
	delete[] rows;
	delete[] selected;
	delete tip;
	delete scrollbar;
}

