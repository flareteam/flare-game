/*
Copyright © 2011-2012 kitano

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

#include "GameState.h"
#include "SharedResources.h"

GameState::GameState() {

	requestedGameState = NULL;

	exitRequested = false;
	hasMusic = false;
}

GameState* GameState::getRequestedGameState() {
	return requestedGameState;
}

void GameState::logic() {
}

void GameState::render() {
}

GameState::~GameState() {
}
