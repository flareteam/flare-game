Project (Flare)
cmake_minimum_required (VERSION 2.6)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/..)

Set (PACKAGE "FLARE")
Set (VERSION "0.13")


# Detect missing dependencies

Find_Package (SDL REQUIRED)
If (NOT SDL_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl1.2-dev' package.")
Else (NOT SDL_FOUND)
  Include_Directories (${SDL_INCLUDE_DIR})
EndIf (NOT SDL_FOUND)

Find_Package (SDL_mixer REQUIRED)
If (NOT SDLMIXER_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL-mixer development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-mixer1.2-dev' package.")
Else (NOT SDLMIXER_FOUND)
  Include_Directories (${SDLMIXER_INCLUDE_DIR})
EndIf (NOT SDLMIXER_FOUND)

Find_Package (SDL_image REQUIRED)
If (NOT SDLIMAGE_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL-image development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-image1.2-dev' package.")
Else (NOT SDLIMAGE_FOUND)
  Include_Directories (${SDLIMAGE_INCLUDE_DIR})
EndIf (NOT SDLIMAGE_FOUND)


# Sources

Set (FLARE_SOURCES 
	../src/Entity.cpp
	../src/Avatar.cpp
	../src/CampaignManager.cpp
	../src/Enemy.cpp
	../src/EnemyManager.cpp
	../src/FileParser.cpp
	../src/FontEngine.cpp
	../src/GameSwitcher.cpp
	../src/Hazard.cpp
	../src/HazardManager.cpp
	../src/InputState.cpp
	../src/ItemDatabase.cpp
	../src/ItemStorage.cpp
	../src/LootManager.cpp
	../src/MapCollision.cpp
	../src/MapIso.cpp
	../src/Menu.cpp
	../src/MenuActionBar.cpp
	../src/MenuCharacter.cpp
	../src/MenuEnemy.cpp
	../src/MenuExit.cpp
	../src/MenuExperience.cpp
	../src/MenuHPMP.cpp
	../src/MenuHUDLog.cpp
	../src/MenuInventory.cpp
	../src/MenuItemStorage.cpp
	../src/MenuLog.cpp
	../src/MenuManager.cpp
	../src/MenuMiniMap.cpp
	../src/MenuPowers.cpp
	../src/MenuTalker.cpp
	../src/MenuTooltip.cpp
	../src/MenuVendor.cpp
	../src/NPC.cpp
	../src/NPCManager.cpp
	../src/PowerManager.cpp
	../src/QuestLog.cpp
	../src/SaveLoad.cpp
	../src/Settings.cpp
	../src/StatBlock.cpp
	../src/TileSet.cpp
	../src/Utils.cpp
	../src/UtilsParsing.cpp
	../src/WidgetButton.cpp
	../src/main.cpp
	../src/GameState.cpp
	../src/GameStateTitle.cpp
	../src/GameStateLoad.cpp
	../src/GameStateGameEngine.cpp
)

Add_Executable (flare ${FLARE_SOURCES})
Target_Link_Libraries (flare ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
