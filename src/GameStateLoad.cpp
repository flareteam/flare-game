/**
 * GameStateLoad
 * 
 * @author Clint Bellanger
 * @license GPL
 */
 
#include "GameStateLoad.h"
#include "GameStateTitle.h"
#include "GameStateGameEngine.h"

GameStateLoad::GameStateLoad(SDL_Surface *_screen, InputState *_inp, FontEngine *_font) : GameState(_screen, _inp, _font) {
	items = new ItemDatabase(screen, font);
	
	button_exit = new WidgetButton(screen, font, inp, "./images/menus/buttons/button_default.png");
	button_exit->label = "Exit to Title";
	button_exit->pos.x = VIEW_W_HALF - button_exit->pos.w/2;
	button_exit->pos.y = VIEW_H - button_exit->pos.h;	
	
	button_action = new WidgetButton(screen, font, inp, "./images/menus/buttons/button_default.png");
	button_action->label = "Choose a Slot";
	button_action->enabled = false;
	button_action->pos.x = (VIEW_W - 640)/2 + 480 - button_action->pos.w/2;
	button_action->pos.y = (VIEW_H - 480)/2 + 384;
	
	load_game = false;
	
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		sprites[i] = NULL;
	}
	
	loadGraphics();
	readGameSlots();
	
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		slot_pos[i].x = (VIEW_W - 640)/2 + 32;
		slot_pos[i].y = (VIEW_H - 480)/2 + (i * 96) + 32;
		slot_pos[i].w = 288;
		slot_pos[i].h = 96;
	}
	
	selected_slot = -1;
	
	// label positions within each slot
	name_pos.x = 16;
	name_pos.y = 16;

	level_pos.x = 24;
	level_pos.y = 40;

	phys_pos.x = 24;
	phys_pos.y = 56;
	ment_pos.x = 24;
	ment_pos.y = 72;

	off_pos.x = 100;
	off_pos.y = 56;
	def_pos.x = 100;
	def_pos.y = 72;

	sprites_pos.x = 178;
	sprites_pos.y = -24;
	
	// temp
	current_frame = 0;
	frame_ticker = 0;
}

void GameStateLoad::loadGraphics() {
	background = NULL;
	selection = NULL;
	
	background = IMG_Load("images/menus/game_slots.png");
	selection = IMG_Load("images/menus/game_slot_select.png");
	if(!background || !selection) {
		fprintf(stderr, "Couldn't load image: %s\n", IMG_GetError());
		SDL_Quit();
	}
	
	SDL_SetColorKey( selection, SDL_SRCCOLORKEY, SDL_MapRGB(selection->format, 255, 0, 255) ); 
	
	// optimize
	SDL_Surface *cleanup = background;
	background = SDL_DisplayFormatAlpha(background);
	SDL_FreeSurface(cleanup);
	
	cleanup = selection;
	selection = SDL_DisplayFormatAlpha(selection);
	SDL_FreeSurface(cleanup);
	
}

void GameStateLoad::readGameSlots() {
	for (int i=0; i<GAME_SLOT_MAX; i++) {
		readGameSlot(i);
	}
}

void GameStateLoad::readGameSlot(int slot) {

	stringstream filename;
	FileParser infile;	
	
	// abort if not a valid slot number
	if (slot < 0 || slot >= GAME_SLOT_MAX) return;

	// save slots are named save#.txt
	filename << "saves/save" << (slot+1) << ".txt";

	if (!infile.open(filename.str())) return;
	
	while (infile.next()) {
	
		// load (key=value) pairs
		if (infile.key == "name")
			stats[slot].name = infile.val;
		else if (infile.key == "xp")
			stats[slot].xp = atoi(infile.val.c_str());
		else if (infile.key == "build") {
			infile.val = infile.val + ",";
			stats[slot].physical = eatFirstInt(infile.val, ',');
			stats[slot].mental = eatFirstInt(infile.val, ',');
			stats[slot].offense = eatFirstInt(infile.val, ',');
			stats[slot].defense = eatFirstInt(infile.val, ',');		
		}
		else if (infile.key == "equipped") {
			equipped[slot][0] = eatFirstInt(infile.val, ',');
			equipped[slot][1] = eatFirstInt(infile.val, ',');
			equipped[slot][2] = eatFirstInt(infile.val, ',');			
		}
		else if (infile.key == "base") {
			stats[slot].base = infile.val;
		}
		else if (infile.key == "look") {
			stats[slot].look = infile.val;
		}
	}
	infile.close();
	
	stats[slot].recalc();
	loadPreview(slot);

}

void GameStateLoad::loadPreview(int slot) {

	string img_main;
	string img_body;
	string img_off;

	SDL_Surface *gfx_body = NULL;
	SDL_Surface *gfx_main = NULL;
	SDL_Surface *gfx_off = NULL;
	SDL_Surface *gfx_head = NULL;
	SDL_Rect src;
	SDL_Rect dest;
	
	if (equipped[slot][0] != 0)	img_main = items->items[equipped[slot][0]].gfx;
	if (equipped[slot][1] != 0)	img_body = items->items[equipped[slot][1]].gfx;
	else img_body = "clothes";
	if (equipped[slot][2] != 0)	img_off = items->items[equipped[slot][2]].gfx;
	
	if (sprites[slot]) SDL_FreeSurface(sprites[slot]);	
	sprites[slot] = IMG_Load("images/avatar/preview_background.png");
	SDL_SetColorKey(sprites[slot], SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 

	// optimize
	SDL_Surface *cleanup = sprites[slot];
	sprites[slot] = SDL_DisplayFormatAlpha(sprites[slot]);
	SDL_FreeSurface(cleanup);
	
	// composite the hero graphic
	
	if (img_body != "") gfx_body = IMG_Load(("images/avatar/" + stats[slot].base + "/" + img_body + ".png").c_str());
	if (img_main != "") gfx_main = IMG_Load(("images/avatar/" + stats[slot].base + "/" + img_main + ".png").c_str());
	if (img_off != "") gfx_off = IMG_Load(("images/avatar/" + stats[slot].base + "/" + img_off + ".png").c_str());
	gfx_head = IMG_Load(("images/avatar/" + stats[slot].base + "/" + stats[slot].look + ".png").c_str());

	if (gfx_body) SDL_SetColorKey(gfx_body, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	if (gfx_main) SDL_SetColorKey(gfx_main, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	if (gfx_off) SDL_SetColorKey(gfx_off, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	if (gfx_head) SDL_SetColorKey(gfx_head, SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 255, 0, 255)); 
	
	src.w = dest.w = 512; // for this menu we only need the stance animation
	src.h = dest.h = 128; // for this menu we only need one direction
	src.x = dest.x = 0;
	src.y = 768; // for this meny we only need facing down
	dest.y = 0;
	
	if (gfx_body) SDL_BlitSurface(gfx_body, &src, sprites[slot], &dest);
	if (gfx_main) SDL_BlitSurface(gfx_main, &src, sprites[slot], &dest);
	if (gfx_head) SDL_BlitSurface(gfx_head, &src, sprites[slot], &dest);	
	if (gfx_off) SDL_BlitSurface(gfx_off, &src, sprites[slot], &dest);

	if (gfx_body) SDL_FreeSurface(gfx_body);
	if (gfx_main) SDL_FreeSurface(gfx_main);
	if (gfx_head) SDL_FreeSurface(gfx_head);
	if (gfx_off) SDL_FreeSurface(gfx_off);

}


void GameStateLoad::logic() {

	frame_ticker++;
	if (frame_ticker == 64) frame_ticker = 0;
	if (frame_ticker < 32)
		current_frame = frame_ticker / 8;
	else
		current_frame = (63 - frame_ticker) / 8;

	if (button_exit->checkClick()) {
		requestedGameState = new GameStateTitle(screen, inp, font);
	}
	
	if (button_action->checkClick()) {
		GameStateGameEngine* eng = new GameStateGameEngine(screen, inp, font);
		eng->resetGame();
		eng->game_slot = selected_slot + 1;
		eng->loadGame();

		requestedGameState = eng;
	}
	
	// check clicking game slot
	if (inp->pressing[MAIN1] && !inp->lock[MAIN1]) {
		for (int i=0; i<GAME_SLOT_MAX; i++) {
			if (isWithin(slot_pos[i], inp->mouse)) {
				selected_slot = i;
				inp->lock[MAIN1] = true;
				
				button_action->enabled = true;
				if (stats[selected_slot].name == "") {
					button_action->label = "New Game";
				}
				else {
					button_action->label = "Load Game";
				}
			}
		}
	}
}

void GameStateLoad::render() {

	SDL_Rect src;
	SDL_Rect dest;

	// display buttons
	button_exit->render();
	button_action->render();
	
	// display background
	src.w = 288;
	src.h = 384;
	src.x = src.y = 0;
	dest.x = slot_pos[0].x;
	dest.y = slot_pos[0].y;
	SDL_BlitSurface(background, &src, screen, &dest);
	
	// display selection
	if (selected_slot >= 0) {
		src.w = 288;
		src.h = 96;
		src.x = src.y = 0;
		SDL_BlitSurface(selection, &src, screen, &slot_pos[selected_slot]);	
	}
	
	Point label;
	stringstream ss;
	
	// display text
	for (int slot=0; slot<GAME_SLOT_MAX; slot++) {
		if (stats[slot].name != "") {
		
			// name
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;		
			font->render(stats[slot].name, label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);

			// level
			ss.str("");
			label.x = slot_pos[slot].x + level_pos.x;
			label.y = slot_pos[slot].y + level_pos.y;		
			ss << "Level " << stats[slot].level;
			font->render(ss.str(), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);
			
			// physical
			ss.str("");
			label.x = slot_pos[slot].x + phys_pos.x;
			label.y = slot_pos[slot].y + phys_pos.y;		
			ss << "Physical " << stats[slot].physical;
			font->render(ss.str(), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);

			// mental
			ss.str("");
			label.x = slot_pos[slot].x + ment_pos.x;
			label.y = slot_pos[slot].y + ment_pos.y;		
			ss << "Mental " << stats[slot].mental;
			font->render(ss.str(), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);

			// offense
			ss.str("");
			label.x = slot_pos[slot].x + off_pos.x;
			label.y = slot_pos[slot].y + off_pos.y;		
			ss << "Offense " << stats[slot].offense;
			font->render(ss.str(), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);

			// defense
			ss.str("");
			label.x = slot_pos[slot].x + def_pos.x;
			label.y = slot_pos[slot].y + def_pos.y;		
			ss << "Defense " << stats[slot].defense;
			font->render(ss.str(), label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);
			
			// render character preview
			dest.x = slot_pos[slot].x + sprites_pos.x;
			dest.y = slot_pos[slot].y + sprites_pos.y;
			src.x = current_frame * 128;
			src.y = 0;
			src.w = src.h = 128;
			
			SDL_BlitSurface(sprites[slot], &src, screen, &dest);
			
		}
		else {
			label.x = slot_pos[slot].x + name_pos.x;
			label.y = slot_pos[slot].y + name_pos.y;		
			font->render("Empty Slot", label.x, label.y, JUSTIFY_LEFT, screen, FONT_WHITE);
		}
	}
}

GameStateLoad::~GameStateLoad() {
	SDL_FreeSurface(background);
	SDL_FreeSurface(selection);
	delete button_exit;
	delete button_action;
	delete items;
}
