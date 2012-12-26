/*
Copyright © 2012 Piotr Rak

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

#pragma once
#ifndef UTILS_MATH_H
#define UTILS_MATH_H 1

#include <cassert>
#include <cstdlib>
#include <algorithm> // for std::min()/std::max()

/**
 * Clamps value to floor.
 *
 * Postcondition: value >= floor
 */
template <typename Ty1_, typename Ty2_>
inline void clampFloor(Ty1_& value, Ty2_ floor) {
	value = std::max<Ty1_>(value, floor);
}

/**
 * Clamps value to ceiling.
 *
 * Postcondition: value <= ceiling
 */
template <typename Ty1_, typename Ty2_>
inline void clampCeil(Ty1_& value, Ty2_ ceiling) {
	value = std::min<Ty1_>(value, ceiling);
}

/**
 * Clamps value.
 *
 * Postcondition: floor <= value <= ceiling
 */
template <typename Ty1_, typename Ty2_>
inline void clamp(Ty1_& value, Ty2_ floor, Ty2_ ceiling) {
	clampFloor(value, floor);
	clampCeil(value, ceiling);
}

/**
 * Returns sign of value.
 */
template <typename Ty_>
inline Ty_ signum(Ty_ value)
{
	return (Ty_(0) < value) - (value < Ty_(0));
}

/**
 * Returns random number between minVal and maxVal.
 */
inline int randBetween(int minVal, int maxVal)
{
	if (minVal == maxVal) return minVal;
	int d = maxVal - minVal;
	return minVal + (rand() % (d + signum(d)));
}

/**
 * Returns true with random percent chance.
 */
inline bool percentChance(int percent)
{
	return rand() % 100 < percent;
}

#endif // UTILS_MATH_H
