/*
 * utils.h
 *
 *  Created on: Apr 16, 2014
 *      Author: diegocaro
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "point.h"

#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

// to avoid the unused parameter warning... ugly, but it maintains
// the requirements of the super class
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

namespace cqtree_utils {

// return a point with the maximum range of all dimensions
template<typename T>
Point<T> max(const std::vector<Point<T> > &v) {
	assert(v.size() > 0);
	Point<T> m(v[0]);

	for (typename std::vector<Point<T> >::const_iterator it = v.begin();
			it != v.end(); ++it) {
		for (int i = 0; i < m.num_dims(); i++) {
			if (it->comp(i) > m.comp(i)) {
				m.set_comp(i, it->comp(i));
			}
		}
	}
	return m;
}

// return the maximum value of all components
template<typename T>
T max(const Point<T> &p) {
	T m = 0;
	for (int i = 0; i < p.num_dims(); ++i) {
		if (p[i] > m) {
			m = p[i];
		}
	}
	return m;
}

unsigned mypow(unsigned x, int y);
unsigned mylog(unsigned b, unsigned x);

template<typename T> void saveVector(ofstream & out, const vector<T> &val) {
	assert(out.good());
	size_t s = val.size();
	out.write((char*) &s, sizeof(s));
	out.write((char*) val.data(), val.size() * sizeof(T));
}

template<typename T> void loadVector(ifstream & in, vector<T> &val) {
	assert(in.good());
	size_t s;

	in.read((char*) &s, sizeof(s));

	T * ret = new T[s];
	in.read((char*) ret, s * sizeof(T));

	val.insert(val.begin(), ret, ret + s);
	delete ret;
}

template<typename T> void loadValue(ifstream & in, T &ret) {
	assert(in.good());
	in.read((char*) &ret, sizeof(T));
}

}

#endif /* UTILS_H_ */
