/**
 * class MenuTooltip
 *
 * @author Clint Bellanger
 * @license GPL
 */

#ifndef MENU_TOOLTIP_H
#define MENU_TOOLTIP_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "FontEngine.h"
#include "Utils.h"
#include "Settings.h"

const int STYLE_FLOAT = 0;
const int STYLE_TOPLABEL = 1;

const int TOOLTIP_MAX_LINES = 16;

struct TooltipData {
	string lines[TOOLTIP_MAX_LINES];
	int colors[TOOLTIP_MAX_LINES];
	int num_lines;
	
	TooltipData() {
		num_lines = 0;
		for (int i=0; i<TOOLTIP_MAX_LINES; i++) {
			lines[i] = "";
			colors[i] = FONT_WHITE;
		}
	}
	
};

class MenuTooltip {
private:
	FontEngine *font;
	SDL_Surface *screen;
	int offset;
	int width;
	int margin;
	int margin_bottom;
public:
	MenuTooltip(FontEngine *_font, SDL_Surface *_screen);
	void calcPosition(int style, Point pos, Point size, Sint16 &bgx, Sint16 &bgy, int &curx, int &cury);
	void render(TooltipData tip, Point pos, int style);
};

#endif
