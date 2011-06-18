/**
 * class WidgetButton
 *
 * @author Clint Bellanger
 * @license GPL
 */

#include "WidgetButton.h"

WidgetButton::WidgetButton(SDL_Surface *_screen, FontEngine *_font, InputState *_inp, const char* _fileName)
	: screen(_screen), font(_font), inp(_inp), fileName(_fileName) {

	buttons = NULL;
	click = NULL;
	label = "";
	pos.x = pos.y = pos.w = pos.y = 0;
	enabled = true;
	
	loadArt();

	pos.w = buttons->w;
	pos.h = (buttons->h / 4); //height of one button

}

void WidgetButton::loadArt() {

	// load button images
	buttons = IMG_Load(fileName);

	if(!buttons) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = buttons;
	buttons = SDL_DisplayFormatAlpha(buttons);
	SDL_FreeSurface(cleanup);
	
}

/**
 * Sets and releases the "pressed" visual state of the button
 * If press and release, activate (return true)
 */
bool WidgetButton::checkClick() {

	// disabled buttons can't be clicked;
	if (!enabled) return false;

	// main button already in use, new click not allowed
	if (inp->lock[MAIN1]) return false;

	// main click released, so the button state goes back to unpressed
	if (pressed && !inp->lock[MAIN1]) {
		pressed = false;
		
		if (isWithin(pos, inp->mouse)) {
		
			// activate upon release
			return true;
		}
	}

	pressed = false;
	
	// detect new click
	if (inp->pressing[MAIN1]) {
		if (isWithin(pos, inp->mouse)) {
		
			inp->lock[MAIN1] = true;
			pressed = true;

		}
	}
	return false;
	
}

void WidgetButton::render() {
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
	else if (isWithin(pos, inp->mouse))
		src.y = BUTTON_GFX_HOVER * pos.h;
	else
		src.y = BUTTON_GFX_NORMAL * pos.h;
	
	SDL_BlitSurface(buttons, &src, screen, &pos);
	
	// render text
	int font_color = FONT_WHITE;
	if (!enabled) font_color = FONT_GRAY;
	
	// center font on button
	int font_x = pos.x + (pos.w/2);
	int font_y = (pos.y + (pos.h/2)) - (font->getHeight() / 2);

	font->render(label, font_x, font_y, JUSTIFY_CENTER, screen, font_color);
}
	
WidgetButton::~WidgetButton() {
	SDL_FreeSurface(buttons);
}

