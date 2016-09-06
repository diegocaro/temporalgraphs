/*
 * bitvector.h
 *
 *  Created on: Aug 28, 2014
 *      Author: diegocaro
 */

#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <vector>

namespace cqtree_utils {
const unsigned int W_ = 32;
class bitvector {
public:
	bitvector();

	// build vector of a given size
	bitvector(size_t size);

	~bitvector();

	// reserve spaces and increase capacity
	void reserve(size_t s);

	/** sets bit p in data */
	void bitset(size_t p);

	const unsigned int *data() const;

	size_t length() const;

private:
	size_t length_;
	std::vector<unsigned int> data_;

};

} /* namespace cqtree_utils */

#endif /* BITVECTOR_H_ */
