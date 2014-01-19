Project (Flare-Game)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "FLARE-GAME")
Set (VERSION "0.19")

set(BINDIR  "games"             CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game executable will be installed.")
set(DATADIR "share/games/flare" CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game data files will be installed.")

If(NOT IS_ABSOLUTE "${DATADIR}")
	add_definitions(-DDATA_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${DATADIR}")
Else(NOT IS_ABSOLUTE "${DATADIR}")
	add_definitions(-DDATA_INSTALL_DIR="${DATADIR}")
EndIf(NOT IS_ABSOLUTE "${DATADIR}")


# desktop file
If(NOT IS_ABSOLUTE "${BINDIR}")
	set(FLARE_EXECUTABLE_PATH ${CMAKE_INSTALL_PREFIX}/${BINDIR}/flare)
Else(NOT IS_ABSOLUTE "${BINDIR}")
	set(FLARE_EXECUTABLE_PATH ${BINDIR}/flare)
EndIf(NOT IS_ABSOLUTE "${BINDIR}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/distribution/flare.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/flare.desktop")


# installing to the proper places
install(DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/mods"
  DESTINATION ${DATADIR}/flare-game)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/flare.desktop"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/distribution/flare_logo.svg"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps
  RENAME flare.svg)
