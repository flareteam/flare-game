/**
 * MenuEnemy
 *
 * Handles the display of the Enemy info of the screen
 *
 * @author Pavel Kirpichyov (Cheshire)
 * @license GPL
 */

#ifndef MENU_ENEMY_H
#define MENU_ENEMY_H

#include "SDL.h"
#include "SDL_image.h"
#include "StatBlock.h"
#include "Utils.h"
#include "FontEngine.h"
#include <string>
#include <sstream>
#include "Enemy.h"

const int MENU_ENEMY_TIMEOUT = FRAMES_PER_SEC * 10;

class MenuEnemy {
private:
	SDL_Surface *screen;
	FontEngine *font;
	SDL_Surface *background;
	SDL_Surface *bar_hp;
public:
	MenuEnemy(SDL_Surface *_screen, FontEngine *_font);
	~MenuEnemy();
	Enemy *enemy;
	void loadGraphics();
	void handleNewMap();
	void logic();
	void render();
	int timeout;
};

#endif
