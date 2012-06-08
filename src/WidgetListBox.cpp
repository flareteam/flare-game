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
	values = new std::string[listAmount];
	tooltips = new std::string[listAmount];
	vlabels = new WidgetLabel[listHeight];
	rows = new SDL_Rect[listHeight];

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

TooltipData WidgetListBox::checkTooltip(Point mouse) {
	TooltipData tip;

	for(int i=0; i<listHeight; i++) {
		if (i<listAmount) {
			if (isWithin(rows[i], mouse) && tooltips[i+cursor] != "") {
				tip.lines[0] = tooltips[i+cursor];
			}
		}
	}

	return tip;
}

void WidgetListBox::append(std::string value, std::string tooltip) {
	for (int i=0;i<listAmount;i++) {
		if (values[i] == "") {
			values[i] = value;
			tooltips[i] = tooltip;
			return;
		}
	}
}

void WidgetListBox::remove(int index) {
	selected[index] = false;
	values[index] = "";
	tooltips[index] = "";
}

std::string WidgetListBox::getValue(int index) {
	return values[index];
}

std::string WidgetListBox::getTooltip(int index) {
	return tooltips[index];
}

void WidgetListBox::scrollUp() {
	if (cursor > 0)
		cursor -= 1;
}

void WidgetListBox::scrollDown() {
	if (cursor+listHeight < listAmount-listHeight+1)
		cursor += 1;
}

void WidgetListBox::render() {
	SDL_Rect src;
	src.x = 0;
	src.w = pos.w;
	src.h = pos.h;

//	for(int i=cursor;i<cursor+listHeight;i++) {
//		if(i==cursor)
//			src.y = 0;
//		else if(i==cursor+listHeight-1)
//			src.y = pos.h*2;
//		else
//			src.y = pos.h;
//	}

	for(int i=0; i<listHeight; i++) {
		if(i==0)
			src.y = 0;
		else if(i==listHeight-1)
			src.y = pos.h*2;
		else
			src.y = pos.h;

		refresh();
		SDL_BlitSurface(listboxs, &src, screen, &rows[i]);
		vlabels[i].render();
	}
}

/**
 * Create the text buffer
 */
void WidgetListBox::refresh() {
	
	for(int i=0;i<listHeight;i++)
	{
		rows[i].x = pos.x;
		rows[i].y = ((pos.h-1)*i)+pos.y;
		rows[i].w = pos.w;
		rows[i].h = pos.h;

		int font_x = rows[i].x + (rows[i].w/2);
		int font_y = rows[i].y + (rows[i].h/2);

		if(selected[i+cursor]) {
			vlabels[i].set(font_x, font_y, JUSTIFY_CENTER, VALIGN_CENTER, values[i+cursor], FONT_WHITE);
		} else {
			vlabels[i].set(font_x, font_y, JUSTIFY_CENTER, VALIGN_CENTER, values[i+cursor], FONT_GRAY);
		}
	}
}

WidgetListBox::~WidgetListBox() {
	SDL_FreeSurface(listboxs);
	delete[] values;
	delete[] tooltips;
	delete[] vlabels;
	delete[] rows;
	delete[] selected;
}

