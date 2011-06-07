#ifndef MENU_EXIT_h
#define MENU_EXIT_H

/**
 * class MenuExit
 *
 * @author kitano
 * @license GPL
 */

#include "Menu.h"
#include "SDL_image.h"
#include "WidgetButton.h"

class MenuExit : public Menu {
protected:
	void loadGraphics();

	WidgetButton *buttonExit;

	bool exitClicked;

public:
	MenuExit(SDL_Surface*, InputState*, FontEngine*);
	~MenuExit();

	void logic();
	virtual void render();

	bool isExitRequested() { return exitClicked; }
};

#endif
