/**
 * class MenuTalker
 *
 * @author morris989 and Clint Bellanger
 * @license GPL
 */

#ifndef MENU_TALKER_H
#define MENU_TALKER_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Utils.h"
#include "FontEngine.h"
#include "NPC.h"
#include "CampaignManager.h"
#include <string>
#include <sstream>
#include "WidgetButton.h"


class MenuTalker {
private:
	SDL_Surface *screen;
	InputState *inp;
	FontEngine *font;
	CampaignManager *camp;

	void loadGraphics();
	SDL_Surface *background;
	SDL_Surface *portrait;
	string hero_name;

	int dialog_node;

public:
	MenuTalker(SDL_Surface *screen, InputState *inp, FontEngine *font, CampaignManager *camp);
	~MenuTalker();

	NPC *npc;
	
	void chooseDialogNode();
	void logic();
	void render();
	void setHero(string name, string portrait_filename);
	
	bool visible;
	int event_cursor;
	bool accept_lock;

	WidgetButton *advanceButton;
	WidgetButton *closeButton;
	
};

#endif
