/**
 * Generic main.cpp for game engines
 *
 */
   
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

#include "Settings.h"
#include "InputState.h"
#include "GameSwitcher.h"
#include "MessageEngine.h"
#include "ModManager.h"

SDL_Surface *screen;
InputState *inps;
GameSwitcher *gswitch;

static void init() {

	setPaths();

	if (!loadSettings()) {
		fprintf(stderr, "Error: could not load config/settings.txt.");
		exit(1);
	}

	// SDL Inits
	if ( SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0 ) {		
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	Uint32 flags = 0;
    
	if (FULLSCREEN) flags = flags | SDL_FULLSCREEN;
	if (DOUBLEBUF) flags = flags | SDL_DOUBLEBUF;
	if (HWSURFACE)
		flags = flags | SDL_HWSURFACE;
	else
		flags = flags | SDL_SWSURFACE;

	// Create window
	screen = SDL_SetVideoMode (VIEW_W, VIEW_H, 0, flags);
	if (screen == NULL) {
		
        fprintf (stderr, "Error during SDL_SetVideoMode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	
	if (Mix_OpenAudio(22050, AUDIO_S16, 2, 1024)) {
		fprintf (stderr, "Error during Mix_OpenAudio: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	
    // initialize Joysticks
	if(SDL_NumJoysticks() == 1) {
		printf("1 joystick was found:\n");
	}
	else if(SDL_NumJoysticks() > 1) {
		printf("%d joysticks were found:\n", SDL_NumJoysticks());
	}
	else {
		printf("No joysticks were found\n");
	}
	for(int i = 0; i < SDL_NumJoysticks(); i++)
	{
		SDL_JoystickOpen(i);
		printf("  Joy %d) %s\n", i, SDL_JoystickName(i));
	}
	printf("Using joystick #%d\n", JOYSTICK_DEVICE);

    // setup ModManager and MessageEngine here instead of below so we can initialize window title
	mods = new ModManager();
    msg = new MessageEngine();

	const char* title = msg->get("Flare").c_str();
	SDL_WM_SetCaption(title, title);
	
	// Set sound effects volume from settings file
	Mix_Volume(-1, SOUND_VOLUME);

	/* Shared game units setup */
	inps = new InputState();
	gswitch = new GameSwitcher(screen, inps);
}

static void mainLoop () {

	bool done = false;
	int fps = FRAMES_PER_SEC;
	int delay = 1000/fps;
	int prevTicks = SDL_GetTicks();
	int nowTicks;
	
	while ( !done ) {
		
		// black out
		SDL_FillRect(screen, NULL, 0);

		SDL_PumpEvents();
		inps->handle();
		gswitch->logic();
		gswitch->render();
		
		// Engine done means the user escapes the main game menu.
		// Input done means the user closes the window.
		done = gswitch->done || inps->done;
		
		nowTicks = SDL_GetTicks();
		if (nowTicks - prevTicks < delay) SDL_Delay(delay - (nowTicks - prevTicks));
		prevTicks = SDL_GetTicks();
		
		SDL_Flip(screen);		
		
		
	}
}

static void cleanup() {
	delete gswitch;
	delete inps;
	delete msg;
	delete mods;
	SDL_FreeSurface(screen);
	Mix_CloseAudio();
	SDL_Quit();
}

int main(int argc, char *argv[])
{	
	srand((unsigned int)time(NULL));
	
	init();
	mainLoop();	
	cleanup();

	return 0;
}
