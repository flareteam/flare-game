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

#ifndef WIDGET_TOOLTIP_H
#define WIDGET_TOOLTIP_H

#include "SharedResources.h"
#include "Utils.h"

#include <SDL.h>
#include <SDL_image.h>


const int STYLE_FLOAT = 0;
const int STYLE_TOPLABEL = 1;

const int TOOLTIP_MAX_LINES = 25;

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
	std::string lines[TOOLTIP_MAX_LINES];
	SDL_Color colors[TOOLTIP_MAX_LINES];
	int num_lines;
	SDL_Surface *tip_buffer;

	// Constructor
	TooltipData() {
		num_lines = 0;
		tip_buffer = NULL;
		for (int i=0; i<TOOLTIP_MAX_LINES; i++) {
			lines[i] = "";
			colors[i] = font->getColor("widget_normal");
		}
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

		num_lines = tdSource.num_lines;
		for (int i=0; i<tdSource.num_lines; i++) {
			lines[i] = tdSource.lines[i];
			colors[i] = tdSource.colors[i];
		}
	}

	// Assignment Operator
	TooltipData& operator= (const TooltipData &tdSource) {

		// if the buffer already exists, deallocate it
		SDL_FreeSurface(tip_buffer);

		// DO NOT copy the buffered text render
		// Allow the new copy to create its own buffer
		// Otherwise the same buffer will be deleted twice, causing a mem error
		tip_buffer = NULL;

		num_lines = tdSource.num_lines;
		for (int i=0; i<tdSource.num_lines; i++) {
			lines[i] = tdSource.lines[i];
			colors[i] = tdSource.colors[i];
		}

		return *this;
	}

	// clear this existing tooltipdata
	void clear() {
		num_lines = 0;
		for (int i=0; i<TOOLTIP_MAX_LINES; i++) {
			lines[i] = "";
			colors[i] = font->getColor("widget_normal");
		}
		SDL_FreeSurface(tip_buffer);
		tip_buffer = NULL;

	}

	// add text with support for new lines
	void addText(std::string text, SDL_Color color) {
		for (int i=0; i<TOOLTIP_MAX_LINES; i++) {
			if (lines[i] == "") {
				int cur = i;
				colors[cur] = color;
				for (unsigned j=0; j<text.length(); j++) {
					if (text[j] == '\n') {
						// insert a space so intentionally blank lines are counted
						lines[cur] += ' ';
						if (num_lines<TOOLTIP_MAX_LINES-1) {
							num_lines++;
							colors[++cur] = color;
						}
					} else if (cur<TOOLTIP_MAX_LINES) {
						lines[cur] += text[j];
					}
				}
				num_lines++;
				break;
			}
		}
	}

	void addText(std::string text) {
		addText(text,font->getColor("widget_normal"));
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
