Project (Flare-Game LANGUAGES NONE)
cmake_minimum_required (VERSION 3.5)

Set (PACKAGE "FLARE-GAME")
Set (VERSION "1.14")

set(BINDIR  "games"             CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game executable will be installed.")
set(DATADIR "share/games/flare" CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game data files will be installed.")

If(NOT IS_ABSOLUTE "${DATADIR}")
	add_definitions(-DDATA_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${DATADIR}")
Else(NOT IS_ABSOLUTE "${DATADIR}")
	add_definitions(-DDATA_INSTALL_DIR="${DATADIR}")
EndIf(NOT IS_ABSOLUTE "${DATADIR}")

# installing to the proper places
install(DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/mods/fantasycore"
  DESTINATION ${DATADIR}/mods)
install(DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/mods/empyrean_campaign"
  DESTINATION ${DATADIR}/mods)
install(DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/mods/centered_statbars"
  DESTINATION ${DATADIR}/mods)

# installing appdata
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/distribution/org.flarerpg.Flare.appdata.xml"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/metainfo)
