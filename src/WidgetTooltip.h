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
 * class WidgetTooltip
 */


#pragma once
#ifndef WIDGET_TOOLTIP_H
#define WIDGET_TOOLTIP_H

#include "SharedResources.h"
#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>

#include <vector>

const int STYLE_FLOAT = 0;
const int STYLE_TOPLABEL = 1;

extern int TOOLTIP_CONTEXT;
const int TOOLTIP_NONE = 0;
const int TOOLTIP_MAP = 1;
const int TOOLTIP_MENU = 2;

/**
 * TooltipData contains the text and line colors for one tool tip.
 * Useful for keeping the data separate from the widget itself, so the data
 * can be passed around easily.
 *
 * Contains a image buffer to keep a render of the tooltip, rather than needing
 * to render it each frame. This buffer is not copied during copy/assign to
 * avoid multiple deconstructors on the same dynamically allocated memory. Thus
 * the new copy will recreate its own buffer next time it is displayed.
 */
class TooltipData {
public:
	std::vector<std::string> lines;
	std::vector<SDL_Color> colors;
	SDL_Surface *tip_buffer;
	SDL_Color default_color;

	// Constructor
	TooltipData() {
		tip_buffer = NULL;
		default_color = font->getColor("widget_normal");
	}

	// Deconstructor
	~TooltipData() {
		SDL_FreeSurface(tip_buffer);
	}

	// Copy Constructor
	TooltipData(const TooltipData &tdSource) {

		// DO NOT copy the buffered text render
		// Allow the new copy to create its own buffer
		// Otherwise the same buffer will be deleted twice, causing a mem error
		tip_buffer = NULL;

		lines.clear();
		colors.clear();

		for (unsigned int i=0; i<tdSource.lines.size(); i++) {
			lines.push_back(tdSource.lines[i]);
			colors.push_back(tdSource.colors[i]);
		}
	}

	// Assignment Operator
	TooltipData& operator= (const TooltipData &tdSource) {

		clear();

		for (unsigned int i=0; i<tdSource.lines.size(); i++) {
			lines.push_back(tdSource.lines[i]);
			colors.push_back(tdSource.colors[i]);
		}

		return *this;
	}

	// clear this existing tooltipdata
	void clear() {
		lines.clear();
		colors.clear();
		SDL_FreeSurface(tip_buffer);
		tip_buffer = NULL;
	}

	// add text with support for new lines
	void addText(const std::string &text, SDL_Color color) {
		lines.push_back("");
		colors.push_back(color);
		for (unsigned int i=0; i<lines.size(); i++) {
			if (lines[i] == "") {
				colors[i] = color;
				for (unsigned int j=0; j<text.length(); j++) {
					if (text[j] == '\n') {
						// insert a space so intentionally blank lines are counted
						if (lines.back() == "") lines.back() += ' ';
						lines.push_back("");
						colors.push_back(color);
					} else {
						lines.back() += text[j];
					}
				}
				break;
			}
		}
	}

	void addText(const std::string &text) {
		addText(text,default_color);
	}

	// check if there's exisiting tooltip data
	bool isEmpty() {
		if (!lines.empty()) return false;
		return true;
	}

	// compare the first line
	bool compareFirstLine(const std::string &text) {
		if (lines.empty()) return false;
		if (lines[0] != text) return false;
		return true;
	}

	// compare all lines
	bool compare(const TooltipData *tip) {
		if (lines.size() != tip->lines.size()) return false;
		for (unsigned int i=0; i<lines.size(); i++) {
			if (lines[i] != tip->lines[i]) return false;
		}
		return true;
	}

};

class WidgetTooltip {
private:
	int offset; // distance between cursor and tooltip
	int width; // max width of tooltips (wrap text)
	int margin; // outer margin between tooltip text and the edge of the tooltip background
public:
	WidgetTooltip();
	Point calcPosition(int style, Point pos, Point size);
	void render(TooltipData &tip, Point pos, int style, SDL_Surface *target = NULL);
	void createBuffer(TooltipData &tip);
};

#endif
