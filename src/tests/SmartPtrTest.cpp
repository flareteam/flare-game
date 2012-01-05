#include "SmartPtr.h"
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

