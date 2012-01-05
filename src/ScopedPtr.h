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

#include <cassert>
#include <cstddef>

/**
 * class ScopedPtr
 *
 * Wraps a raw pointer, deleting it when it goes out of scope
 *
 * This class should _NOT_ be used to wrap pointers created with new[], as
 * those may not be deleted with delete -- request ScopedArray if you want
 * that.
 *
 * Comparing for equality is not currently supported, as it would 
 */
 
template<typename T>
class ScopedPtr {
private:
	T* ptr_;
	ScopedPtr(ScopedPtr const&);
	ScopedPtr& operator=(ScopedPtr const&);

public:
	ScopedPtr() : ptr_(NULL) {}

	explicit ScopedPtr(T* ptr) : ptr_(ptr) {}

	~ScopedPtr() {
		delete ptr_;
	}

	//! Return the owned pointer, but keep ownership of it.
	T* get() {
		assert(ptr_ && "Requested null pointer.");
		return ptr_;
	}

	//! Delete the old pointer and own the new one.
	void reset(T* ptr = NULL) {
		delete ptr_;
		ptr_ = ptr;
	}

	//! Return the owned pointer and surrender ownership of it.
	T* release() {
		assert(ptr_ && "Requested release of null pointer.");
		T* retval = ptr_;
		ptr_ = NULL;
		return retval;
	}

	//! More explicit check for NULL than is_bool.
	bool is_null() const {
		return ptr_ == NULL;
	}


	// operator overloads -- do the same thing as on a raw pointer, but check
	// for NULL.
	T& operator*() {
		assert(ptr_ && "Dereferencing null pointer.");
		return *ptr_;
	}
	T const& operator*() const {
		assert(ptr_ && "Dereferencing null pointer.");
		return *ptr_;
	}
	T* operator->() {
		assert(ptr_ && "Dereferencing null pointer.");
		return ptr_;
	}
	T const* operator->() const {
		assert(ptr_ && "Dereferencing null pointer.");
		return ptr_;
	}

	operator bool() const {
		return ptr_ == NULL;
	}

	bool operator!() const {
		return !ptr_;
	}
};

// operators for comparing ScopedPtrs with ScopedPtrs are not currently
// provided (as two ScopedPtrs being equal would be baaaad), and operators <,
// >, <=, and >= are not provided because ScopedPtrs should never point to
// arrays.

template<typename T1, typename T2>
bool operator==(ScopedPtr<T1> lhs, T2* rhs) {
	return lhs.get() == rhs;
}

template<typename T1, typename T2>
bool operator!=(ScopedPtr<T1> lhs, T2* rhs) {
	return lhs.get() != rhs;
}

template<typename T1, typename T2>
bool operator==(T1* lhs, ScopedPtr<T2> rhs) {
	return rhs.get() == lhs;
}

template<typename T1, typename T2>
bool operator!=(T1* lhs, ScopedPtr<T2> rhs) {
	return rhs.get() != lhs;
}

