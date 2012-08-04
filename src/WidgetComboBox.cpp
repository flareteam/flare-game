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
 * class WidgetComboBox
 */

#include "WidgetComboBox.h"
#include "SharedResources.h"
#include "SDL_gfxBlitFunc.h"

using namespace std;

WidgetComboBox::WidgetComboBox(int amount, const std::string& _fileName)
	: fileName(_fileName) {

	cmbAmount = amount;
	values = new std::string[cmbAmount];
	vlabels = new WidgetLabel[cmbAmount];
	rows = new SDL_Rect[cmbAmount];

	comboboxs = NULL;
	click = NULL;
	label = "";
	pos.x = pos.y = pos.w = pos.h = 0;
	enabled = true;
	pressed = false;
	selected = 0;
	hover = false;
	
	loadArt();

	pos.w = (comboboxs->w / 2);
	pos.h = (comboboxs->h / 4); //height of one ComboBox

	render_to_alpha = false;
}

void WidgetComboBox::loadArt() {

	// load ComboBox images
	comboboxs = IMG_Load(fileName.c_str());

	if(!comboboxs) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
		exit(1); // or abort ??
	}
	
	// optimize
	SDL_Surface *cleanup = comboboxs;
	comboboxs = SDL_DisplayFormatAlpha(comboboxs);
	SDL_FreeSurface(cleanup);
}

bool WidgetComboBox::checkClick() {
	if (checkClick(inpt->mouse.x,inpt->mouse.y))
		return true;
	else
		return false;
}

/**
 * Sets and releases the "pressed" visual state of the ComboBox
 * If press and release, activate (return true)
 */
bool WidgetComboBox::checkClick(int x, int y) {
	Point mouse = {x,y};
	mouse_point = mouse;

	// Change the hover state
	if (isWithin(pos, mouse)) {
		hover = true;
	} else {
		hover = false;
	}

	// disabled ComboBoxs can't be clicked;
	if (!enabled) return false;

	// main ComboBox already in use, new click not allowed
	if (inpt->lock[MAIN1]) return false;

	// main click released, so the ComboBox state goes back to unpressed
	if (pressed && !inpt->lock[MAIN1]) {
		pressed = false;
		
		for(int i=0;i<cmbAmount;i++)
		{
		if (isWithin(rows[i], mouse)) {
			// activate upon release
			selected = i;
			if (i<cmbAmount-1) {
				if (isWithin(rows[i+1], mouse))
					selected = i+1;
			}
			refresh();
			return true;
		}
		}
	}

	pressed = false;

	// detect new click
	if (inpt->pressing[MAIN1]) {
		if (isWithin(pos, mouse)) {
		
			inpt->lock[MAIN1] = true;
			pressed = true;

		}
	}
	return false;

}

void WidgetComboBox::set(int index, std::string value) {
	values[index] = value;
}

std::string WidgetComboBox::get(int index) {
	return values[index];
}

void WidgetComboBox::render(SDL_Surface *target) {
	if (target == NULL) {
		target = screen;
	}

	SDL_Rect src;
	src.x = 0;
	src.w = pos.w;
	src.h = pos.h;

	// the "ComboBox" surface contains ComboBox variations.
	// choose which variation to display.
	if (!enabled)
		src.y = COMBOBOX_GFX_DISABLED * pos.h;
	else if (pressed)
		src.y = COMBOBOX_GFX_PRESSED * pos.h;
	else if (hover)
		src.y = COMBOBOX_GFX_HOVER * pos.h;
	else
		src.y = COMBOBOX_GFX_NORMAL * pos.h;

	if (render_to_alpha)
		SDL_gfxBlitRGBA(comboboxs, &src, target, &pos);
	else
		SDL_BlitSurface(comboboxs, &src, target, &pos);

	wlabel.render(target);

	if(pressed)
	{
		src.x = pos.w;
		for(int i=0;i<cmbAmount;i++)
		{
			rows[i].x = pos.x;
			rows[i].y = ((pos.h-1)*i)+pos.y-3+pos.h;
			rows[i].w = pos.w;
			rows[i].h = pos.h;

			if(i==cmbAmount-1)
				src.y = pos.h;
			else
				src.y = 0;

			refresh();
			if (render_to_alpha)
				SDL_gfxBlitRGBA(comboboxs, &src, target, &rows[i]);
			else
				SDL_BlitSurface(comboboxs, &src, target, &rows[i]);
			vlabels[i].render(target);
		}
	}
}

/**
 * Create the text buffer
 */
void WidgetComboBox::refresh() {

	std::string temp;

	// Draw the label for the selected item
	if (values[selected].length() > 19) {
		label = values[selected].substr(0,16);
		label.append("...");
	} else {
		label = values[selected];
	}
	if (label != "") {
		SDL_Color font_color = font->getColor("widget_normal");
		if (!enabled) font_color = font->getColor("widget_disabled");

		int font_x = pos.x + 8;
		int font_y = pos.y + (pos.h/2);

		wlabel.set(font_x, font_y, JUSTIFY_LEFT, VALIGN_CENTER, label, font_color);
	}

	// Draw the labels for each option if the menu is open
	if(pressed) {
		for(int i=0;i<cmbAmount;i++) {
			SDL_Color font_color;
			if(isWithin(rows[i], mouse_point)) {
				font_color = font->getColor("widget_normal");
				if(i<cmbAmount-1) {
					if(isWithin(rows[i+1], mouse_point))
						font_color = font->getColor("widget_disabled");
				}
			} else {
				font_color = font->getColor("widget_disabled");
			}
			if (values[i].length() > 23) {
				temp = values[i].substr(0,20);
				temp.append("...");
			} else {
				temp = values[i];
			}

			int font_x = rows[i].x + 8;
			int font_y = rows[i].y + (rows[i].h/2);

			vlabels[i].set(font_x, font_y, JUSTIFY_LEFT, VALIGN_CENTER, temp, font_color);
		}
	}
}

WidgetComboBox::~WidgetComboBox() {
	SDL_FreeSurface(comboboxs);
	delete[] values;
	delete[] vlabels;
	delete[] rows;
}

