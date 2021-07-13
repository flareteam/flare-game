# Flare

Flare is a single-player 2D action RPG with fast-paced action and a dark fantasy style.

It is built on the FLARE engine (Free/Libre Action Roleplaying Engine). Its repo is at https://github.com/flareteam/flare-engine

Flare uses simple file formats (INI style config files) for most of the game data, allowing anyone to easily modify game contents. Open formats are preferred (png, ogg). Maps are edited using Tiled.

For best results, please use the latest version of the FLARE engine with the game data in this repository. Using an older version of the engine could result in some game elements not working correctly.

![Flare screenshot](distribution/screenshot1.jpg)

The flare-game content contains depictions of violence, blood, and mild gore.

## Copyright and License

Flare (the game) is Copyright ©2010-2013 Clint Bellanger. Contributors retain copyrights to their original contributions.

The Flare Engine is released under GPL version 3 or later.

All of Flare's art and data files are released under CC-BY-SA 3.0. Later versions are permitted.

The Liberation Sans fonts version 2 are released under the SIL Open Font License, Version 1.1.

The Marck Script font is licensed under the SIL Open Font License, Version 1.1.


## Links

* Homepage  http://flarerpg.org
* Repo      https://github.com/flareteam/flare-game
* Binaries  https://sourceforge.net/projects/flare-game/
* Forums    http://opengameart.org/forums/flare
* Email     clintbellanger@gmail.com

## The Simplest Setup

If you are building and running Flare Game from source, this is the simplest way to get started on all operating systems. Here you build and play Flare in a local folder instead of installing it to a standard OS folder (e.g. /usr/local or Program Files).

* Clone the flare-engine repo and build the executable.
* Clone the flare-game repo into your user directory.
* Copy the flare-engine "default" mod into the flare-game mods folder.
* Put the flare executable inside the flare-game folder
* The executable and the "mods" folder should be in the same directory
* Run Flare

## Building and Running (non-Linux)

See the Flare-Engine repo for instructions on how to build/install Flare:

https://github.com/flareteam/flare-engine

The mods folder should be in the same directory as the flare executable when building from source.

Flare Game is installed as a set of mods for Flare Engine. Place the contents of the "mods" folder inside Flare Engine's mods folder.

Then enable these mods in Flare's Configuration screen.

    fantasycore
    empyrean_campaign


### Building and Running on Linux

Clone the repos:

	git clone https://github.com/flareteam/flare-engine
	git clone https://github.com/flareteam/flare-game


Move to the flare-engine repo and build:
For instructions more in detail see the INSTALL file in the
flare-engine repo.

	cd flare-engine
	cmake . && make


From this point we have two options:

Option 1
Install the game system-wide (requires root access):

	sudo make install
	cd ../flare-game
	cmake . && sudo make install
	cd ~ #this step is important because we don't want to load mods from the repo folders
	flare #alternatively, launch Flare from the desktop application menu


Option 2
Run the game without installing:

	cd ../flare-game
	ln -s ../flare-engine/flare flare
	cd mods
	ln -s ../../flare-engine/mods/default default
	cd ..
	./flare



## Settings

Settings are stored in one of these places:

    $XDG_CONFIG_HOME/flare
    $HOME/.config/flare
    ./config

Here you can enable fullscreen, change the game resolution, enable mouse-move, and change keybindings. The settings files are created the first time you run Flare.

## Save Files

Save files are stored in one of these places:

    $XDG_DATA_HOME/flare
    $HOME/.local/share/flare
    ./saves

If permissions are correct, the game is automatically saved when you exit.

## Packaging and Distributing

If you are packaging Flare Game for release (e.g. on an operating system's software repo), we suggest creating two packages.

* flare-engine the package that contains the single engine reused by several games
* flare-game, a package that requires flare-engine that only contains this game data

When distributing flare-game you may elect to omit these folders which are not used at runtime.

* art_src contains the raw files (e.g. Blender files) used to generate Flare's art.
* tiled contains the Tiled-native map files used to export Flare's maps
