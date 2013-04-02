Project (Flare)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "FLARE")
Set (VERSION "0.18")


# Default definitions
if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wunused -Wshadow -Woverloaded-virtual -Wunreachable-code")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno -fno-exceptions")
  if (NOT CMAKE_CXX_COMPILER MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
  endif (NOT CMAKE_CXX_COMPILER MATCHES "clang")
  set(CMAKE_LD_FLAGS "-fwhole-program")
endif (NOT MSVC)

if (CMAKE_CXX_COMPILER MATCHES "clang")
  # clang needs lstdc++ linked explicitly:
  set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -lstdc++")
elseif ()
  # assume gcc:
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-enforce-eh-specs")
endif ()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-pg")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "-pg")
endif()

set(BINDIR  "games"             CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game executable will be installed.")
set(DATADIR "share/games/flare" CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game data files will be installed.")

If(NOT IS_ABSOLUTE "${DATADIR}")
	add_definitions(-DDATA_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${DATADIR}")
Else(NOT IS_ABSOLUTE "${DATADIR}")
	add_definitions(-DDATA_INSTALL_DIR="${DATADIR}")
EndIf(NOT IS_ABSOLUTE "${DATADIR}")

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

Find_Package (SDL_ttf REQUIRED)
If (NOT SDLTTF_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL-ttf development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-ttf2.0-dev' package.")
Else (NOT SDLTTF_FOUND)
  Include_Directories (${SDLTTF_INCLUDE_DIR})
EndIf (NOT SDLTTF_FOUND)


# Sources

Set (FLARE_SOURCES
	./src/Entity.cpp
	./src/Animation.cpp
	./src/AnimationManager.cpp
	./src/AnimationSet.cpp
	./src/AStarNode.cpp
	./src/Avatar.cpp
	./src/BehaviorStandard.cpp
	./src/CampaignManager.cpp
	./src/CombatText.cpp
	./src/EffectManager.cpp
	./src/Enemy.cpp
	./src/EnemyBehavior.cpp
	./src/EnemyGroupManager.cpp
	./src/EnemyManager.cpp
	./src/FileParser.cpp
	./src/FontEngine.cpp
	./src/GameState.cpp
	./src/GameStateConfig.cpp
	./src/GameStateCutscene.cpp
	./src/GameStateTitle.cpp
	./src/GameStateLoad.cpp
	./src/GameStatePlay.cpp
	./src/GameStateNew.cpp
	./src/GameSwitcher.cpp
	./src/GetText.cpp
	./src/Hazard.cpp
	./src/HazardManager.cpp
	./src/ImageManager.cpp
	./src/InputState.cpp
	./src/ItemManager.cpp
	./src/ItemStorage.cpp
	./src/Loot.cpp
	./src/LootManager.cpp
	./src/MapCollision.cpp
	./src/MapRenderer.cpp
	./src/Menu.cpp
	./src/MenuActionBar.cpp
	./src/MenuActiveEffects.cpp
	./src/MenuCharacter.cpp
	./src/MenuConfirm.cpp
	./src/MenuEnemy.cpp
	./src/MenuExit.cpp
	./src/MenuHUDLog.cpp
	./src/MenuInventory.cpp
	./src/MenuItemStorage.cpp
	./src/MenuLog.cpp
	./src/MenuManager.cpp
	./src/MenuMiniMap.cpp
	./src/MenuNPCActions.cpp
	./src/MenuPowers.cpp
	./src/MenuStash.cpp
	./src/MenuStatBar.cpp
	./src/MenuTalker.cpp
	./src/MenuVendor.cpp
	./src/MessageEngine.cpp
	./src/ModManager.cpp
	./src/NPC.cpp
	./src/NPCManager.cpp
	./src/PowerManager.cpp
	./src/QuestLog.cpp
	./src/SaveLoad.cpp
	./src/SDL_gfxBlitFunc.c
	./src/Settings.cpp
	./src/SharedResources.cpp
	./src/StatBlock.cpp
	./src/SoundManager.cpp
	./src/TileSet.cpp
	./src/Utils.cpp
	./src/UtilsDebug.cpp
	./src/UtilsFileSystem.cpp
	./src/UtilsParsing.cpp
	./src/Widget.cpp
	./src/WidgetCheckBox.cpp
	./src/WidgetButton.cpp
	./src/WidgetInput.cpp
 	./src/WidgetLabel.cpp
 	./src/WidgetListBox.cpp
	./src/WidgetScrollBar.cpp
	./src/WidgetScrollBox.cpp
	./src/WidgetSlider.cpp
 	./src/WidgetTabControl.cpp
	./src/WidgetTooltip.cpp
	./src/main.cpp
)

# Add icon and file info to executable for Windows systems
IF (WIN32)
  SET(FLARE_SOURCES
    ${FLARE_SOURCES}
    ./src/Flare.rc
    )
ENDIF (WIN32)

Add_Executable (flare ${FLARE_SOURCES})

# libSDLMain comes with libSDL if needed on certain platforms
If (NOT SDLMAIN_LIBRARY)
  Set (SDLMAIN_LIBRARY "")
EndIf (NOT SDLMAIN_LIBRARY)

Target_Link_Libraries (flare ${CMAKE_LD_FLAGS} ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${SDLMAIN_LIBRARY})


# desktop file
If(NOT IS_ABSOLUTE "${BINDIR}")
	set(FLARE_EXECUTABLE_PATH ${CMAKE_INSTALL_PREFIX}/${BINDIR}/flare)
Else(NOT IS_ABSOLUTE "${BINDIR}")
	set(FLARE_EXECUTABLE_PATH ${BINDIR}/flare)
EndIf(NOT IS_ABSOLUTE "${BINDIR}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/distribution/flare.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/flare.desktop")


# installing to the proper places
install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/flare
  DESTINATION ${BINDIR})
install(DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/mods"
  DESTINATION ${DATADIR})
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/flare.desktop"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/distribution/flare_logo.svg"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps
  RENAME flare.svg)
