/*
Copyright © 2011-2012 Clint Bellanger
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
 * class WidgetButton
 */

#include "WidgetButton.h"
#include "SharedResources.h"
#include "SDL_gfxBlitFunc.h"

using namespace std;

WidgetButton::WidgetButton(const std::string& _fileName)
	: Widget()
	, fileName(_fileName)
	, buttons(NULL)
	, click(NULL)
	, wlabel()
	, color_normal(font->getColor("widget_normal"))
	, color_disabled(font->getColor("widget_disabled"))
	, tip_buf()
	, tip_new()
	, tip(new WidgetTooltip())
	, label("")
	, tooltip("")
	, enabled(true)
	, pressed(false)
	, hover(false)
{
	pos.x = pos.y = pos.w = pos.h = 0;
	loadArt();
	pos.w = buttons->w;
	pos.h = (buttons->h / 4); //height of one button
}

void WidgetButton::loadArt() {

	// load button images
	buttons = loadGraphicSurface(fileName);

	if (!buttons) {
		SDL_Quit();
		exit(1); // or abort ??
	}
}

bool WidgetButton::checkClick() {
	return checkClick(inpt->mouse.x,inpt->mouse.y);
}

/**
 * Sets and releases the "pressed" visual state of the button
 * If press and release, activate (return true)
 */
bool WidgetButton::checkClick(int x, int y) {
	Point mouse(x,y);

	// Change the hover state
	hover = isWithin(pos, mouse);

	// Check the tooltip
	tip_new = checkTooltip(mouse);

	// disabled buttons can't be clicked;
	if (!enabled) return false;

	// main button already in use, new click not allowed
	if (inpt->lock[MAIN1]) return false;

	// main click released, so the button state goes back to unpressed
	if (pressed && !inpt->lock[MAIN1]) {
		pressed = false;

		if (isWithin(pos, mouse)) {

			// activate upon release
			return true;
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

void WidgetButton::render(SDL_Surface *target) {
	if (target == NULL) {
		target = screen;
	}
	SDL_Rect src;
	src.x = 0;
	src.w = pos.w;
	src.h = pos.h;

	// the "button" surface contains button variations.
	// choose which variation to display.
	if (!enabled)
		src.y = BUTTON_GFX_DISABLED * pos.h;
	else if (pressed)
		src.y = BUTTON_GFX_PRESSED * pos.h;
	else if (hover)
		src.y = BUTTON_GFX_HOVER * pos.h;
	else
		src.y = BUTTON_GFX_NORMAL * pos.h;

	// create a temporary rect so we don't modify pos
	SDL_Rect offset = pos;

	if (render_to_alpha)
		SDL_gfxBlitRGBA(buttons, &src, target, &offset);
	else
		SDL_BlitSurface(buttons, &src, target, &offset);

	wlabel.render(target);

	// render the tooltip
	// TODO move this to menu rendering
	if (!tip_new.isEmpty()) {
		if (!tip_new.compare(&tip_buf)) {
			tip_buf.clear();
			tip_buf = tip_new;
		}
		tip->render(tip_buf, inpt->mouse, STYLE_FLOAT, target);
	}
}

/**
 * Create the text buffer
 */
void WidgetButton::refresh() {
	if (label != "") {

		int font_x = pos.x + (pos.w/2);
		int font_y = pos.y + (pos.h/2);

		if (enabled)
			wlabel.set(font_x, font_y, JUSTIFY_CENTER, VALIGN_CENTER, label, color_normal);
		else
			wlabel.set(font_x, font_y, JUSTIFY_CENTER, VALIGN_CENTER, label, color_disabled);
	}
}

/**
 * If mousing-over an item with a tooltip, return that tooltip data.
 *
 * @param mouse The x,y screen coordinates of the mouse cursor
 */
TooltipData WidgetButton::checkTooltip(Point mouse) {
	TooltipData _tip;

	if (isWithin(pos, mouse) && tooltip != "") {
		_tip.addText(tooltip);
	}

	return _tip;
}

WidgetButton::~WidgetButton() {
	SDL_FreeSurface(buttons);
	tip_buf.clear();
	delete tip;
}

