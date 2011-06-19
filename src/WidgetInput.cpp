#include "WidgetInput.h"

WidgetInput::WidgetInput(SDL_Surface* _screen, FontEngine *_font, InputState *_inp, string _label)
	: screen(_screen), font(_font), inp(_inp), label(_label),
	  enabled(true), inFocus(false), pressed(false),
	  max_characters(20) {

	// load button images
	background = IMG_Load("./images/menus/input.png");

	if(!background) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);

	// position
	pos.w = background->w;
	pos.h = background->h/2;
	pos.x = VIEW_W_HALF - pos.w/2;
	pos.y = VIEW_H_HALF+176;
}


void WidgetInput::logic() {
	if (checkClick()) {
		inFocus = true;
	}

	// if clciking elsewhere unfocus the text box
	if (inp->pressing[MAIN1]) {
		if (!isWithin(pos, inp->mouse)) {
			inFocus = false;
		}
	}

	if (inFocus) {

		// handle text input
		text += inp->inkeys;
		if (text.length() > max_characters) {
			text = text.substr(0, max_characters);
		}
			
		// handle backspaces
		if (!inp->lock[DELETE] && inp->pressing[DELETE]) {
			inp->lock[DELETE] = true;
			text = text.substr(0, text.length()-1);
		}
	}
}

void WidgetInput::render() {
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = pos.w;
	src.h = pos.h;

	if (!inFocus)
		src.y = 0 * pos.h;
	else if (isWithin(pos, inp->mouse))
		src.y = 1 * pos.h;
	else
		src.y = 1 * pos.h;

	SDL_BlitSurface(background, &src, screen, &pos);

	// show the label above the text box
	font->render(label, pos.x + (pos.w/2), pos.y - 10, JUSTIFY_CENTER, screen, FONT_GRAY);

	// show dimmed text if box not in focus
	if (!inFocus) {
		font->render(text, pos.x + 20, pos.y + (pos.h/2) - 4, JUSTIFY_LEFT, screen, FONT_GRAY);
	}
	else {
		font->render(text + "|" , pos.x + 20, pos.y + (pos.h/2) - 4, JUSTIFY_LEFT, screen, FONT_WHITE);
	}
}

bool WidgetInput::checkClick() {

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

