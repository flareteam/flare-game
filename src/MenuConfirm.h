#ifndef MENU_CONFIRM_H
#define MENU_CONFIRM_H

#include "Menu.h"
#include "SDL_image.h"
#include "WidgetButton.h"

class MenuConfirm : public Menu {
protected:
	void loadGraphics();

	WidgetButton *buttonConfirm;
	WidgetButton *buttonClose;

	bool confirmClicked;

	string boxMsg;
public:
	MenuConfirm(SDL_Surface*, InputState*, FontEngine*, string, string);
	~MenuConfirm();

	void logic();
	virtual void render();

	bool isConfirmRequested() { return confirmClicked; }
};

#endif
