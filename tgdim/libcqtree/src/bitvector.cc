/*
 * bitvector.cc
 *
 *  Created on: 01-06-2015
 *      Author: diegocaro
 */

#include "bitvector.h"

namespace cqtree_utils {

bitvector::bitvector() {
	length_ = 0;
}

// build vector of a given size
bitvector::bitvector(size_t size) {
	reserve(size);
	length_ = size;
}

bitvector::~bitvector() {

}

// reserve spaces and increase capacity
void bitvector::reserve(size_t s) {
//        printf("BitVector: increasing size\n");
	data_.reserve(s / W_ + 1);
	data_.insert(data_.end(), s / W_ + 1 - data_.size(), 0);
}

/** sets bit p in data */
void bitvector::bitset(size_t p) {
	if (p >= length_) {
		//if current data_ size need to be updated

		if (p / W_ + 1 < data_.size()) {
			printf("reserving more data...\n");
			reserve(2 * p);
		}
		length_ = p + 1;
	}

	data_[p / W_] |= (1 << (p % W_));
}

const unsigned int * bitvector::data() const {
	return data_.data();
}

size_t bitvector::length() const {
	return length_;
}

}


