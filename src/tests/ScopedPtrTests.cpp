#include "ScopedPtr.h"
#include <boost/test/unit_test.hpp>

namespace {
	struct TrackedObject {
		static int in_existence;

		TrackedObject()  {
			++in_existence;
		}

		TrackedObject(TrackedObject const& o) {
			++in_existence;
		}

		~TrackedObject() {
			--in_existence;
		}
	};

	int TrackedObject::in_existence = 0;
}

BOOST_AUTO_TEST_SUITE(ScopedPtrSuite)

BOOST_AUTO_TEST_CASE(NormalUse) {
	// Make sure we're starting with a clean slate.
	TrackedObject::in_existence = 0;
	{
		ScopedPtr<TrackedObject> ptr(new TrackedObject());
		BOOST_CHECK_EQUAL(1, TrackedObject::in_existence);
	}
	BOOST_CHECK_EQUAL(0, TrackedObject::in_existence);
}

BOOST_AUTO_TEST_CASE(Get) {
	TrackedObject* p = new TrackedObject();
	ScopedPtr<TrackedObject> sp(p);
	BOOST_CHECK_EQUAL(p, sp.get());
}

BOOST_AUTO_TEST_CASE(Reset) {
	TrackedObject::in_existence = 0;
	ScopedPtr<TrackedObject> ptr(new TrackedObject());
	BOOST_CHECK_EQUAL(1, TrackedObject::in_existence);
	ptr.reset();
	BOOST_CHECK_EQUAL(0, TrackedObject::in_existence);
}

BOOST_AUTO_TEST_CASE(Release) {
	TrackedObject::in_existence = 0;
	ScopedPtr<TrackedObject> ptr(new TrackedObject());
	BOOST_CHECK_EQUAL(1, TrackedObject::in_existence);
	TrackedObject* p = ptr.release();
	BOOST_CHECK_EQUAL(1, TrackedObject::in_existence);
	delete p;
	BOOST_CHECK_EQUAL(0, TrackedObject::in_existence);
}

BOOST_AUTO_TEST_SUITE_END()

