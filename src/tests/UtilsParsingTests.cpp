/*
Copyright 2011 Clint Bellanger

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

#include "UtilsParsing.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(UtilsParsingSuite)

BOOST_AUTO_TEST_CASE(btoxTest) {
	BOOST_CHECK_EQUAL('0', btox(0, 0, 0, 0));
	BOOST_CHECK_EQUAL('1', btox(1, 0, 0, 0));
	BOOST_CHECK_EQUAL('2', btox(0, 1, 0, 0));
	BOOST_CHECK_EQUAL('3', btox(1, 1, 0, 0));
	BOOST_CHECK_EQUAL('4', btox(0, 0, 1, 0));
	BOOST_CHECK_EQUAL('5', btox(1, 0, 1, 0));
	BOOST_CHECK_EQUAL('6', btox(0, 1, 1, 0));
	BOOST_CHECK_EQUAL('7', btox(1, 1, 1, 0));
	BOOST_CHECK_EQUAL('8', btox(0, 0, 0, 1));
	BOOST_CHECK_EQUAL('9', btox(1, 0, 0, 1));
	BOOST_CHECK_EQUAL('a', btox(0, 1, 0, 1));
	BOOST_CHECK_EQUAL('b', btox(1, 1, 0, 1));
	BOOST_CHECK_EQUAL('c', btox(0, 0, 1, 1));
	BOOST_CHECK_EQUAL('d', btox(1, 0, 1, 1));
	BOOST_CHECK_EQUAL('e', btox(0, 1, 1, 1));
	BOOST_CHECK_EQUAL('f', btox(1, 1, 1, 1));
}

BOOST_AUTO_TEST_SUITE_END()

