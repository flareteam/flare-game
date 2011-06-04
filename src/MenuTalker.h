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


class MenuTalker {
private:
	SDL_Surface *screen;
	FontEngine *font;
	CampaignManager *camp;

	void loadGraphics();
	SDL_Surface *background;

	int dialog_node;

public:
	MenuTalker(SDL_Surface *screen, FontEngine *font, CampaignManager *camp);
	~MenuTalker();

	NPC *npc;
	
	void chooseDialogNode();
	void logic(bool pressing_accept);
	void render();
	
	bool visible;
	int event_cursor;
	bool accept_lock;
	
};

#endif
