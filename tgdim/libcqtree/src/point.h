/*
 * point.h
 *
 *  Created on: Apr 15, 2014
 *      Author: diegocaro
 */

#ifndef POINT_H_
#define POINT_H_

#include <cstdio>
#include <cassert>
#include <algorithm> //for fill_n, copy and equal
//#include <cstring> //for memset and memcpy

#define PNT_INT_SAV 2
#define PNT_LONG_SAV 3

namespace cqtree_utils {

const int Wint = 32; //size of a integer word

class __Point {
};

// preconditions:
// 1. tdata should be int or long
// 2. dimensions must be an integer > 0
template<typename tdata>
class Point: public __Point {
public:

	tdata weight;

	Point() {
		//data_point_ = NULL;
		num_dims_ = 0;
	}

	Point(int dimensions) :
			num_dims_(dimensions) {
		assert(dimensions > 0);

//        data_point_ = new tdata[num_dims_];
		//memset(p, 0, sizeof(tdata)*num_dims_);
		std::fill_n(&data_point_[0], num_dims_, 0);
	}

	Point(const Point<tdata> &obj) :
			weight(obj.weight), num_dims_(obj.num_dims_) {
//        data_point_ = new tdata[num_dims_];

		//memcpy(data_point_, obj.data_point_, sizeof(tdata)*num_dims_);
		std::copy(obj.data_point_, obj.data_point_ + num_dims_, data_point_);

	}

//    ~Point() {
////		if (data_point_ != NULL) {
////        	delete[] data_point_;
////		}
////
////        data_point_ = NULL;
//        num_dims_ = 0;
//    }

	// Automatically generated due new array for data_point_[]
//    Point& operator=(const Point& that) {
//        tdata *local_data_point_ = new tdata[that.num_dims_];
//
//        std::copy(that.data_point_, that.data_point_ + that.num_dims_,
//                  local_data_point_);
//
//		delete[] data_point_;
//
//        data_point_ = local_data_point_;
//        num_dims_ = that.num_dims_;
//
//        return *this;
//    }

	// FIXME This works only for less than 32 dimensions, because bitpos should be less than 32
	inline int getBitDim(int bitpos, int dim) const {
		assert(dim < num_dims_);
		assert(bitpos >= 0);
		assert(bitpos < Wint);  //size is in bytes
		return ((data_point_[dim] >> bitpos) & 1); //FIXME here, read comment above
	}

	// aca la idea es obtener los dim bits correspondientes al morton code del punto
	// se asume que el morton code es de la forma p1_31 p2_31 ... pd_31 p1_30 ... pd_32 .... p1_0 .... pd_0
	// FIXME This works only for less than 32 dimensions, because num_dims_ should be less than 32
	inline int getMorton(int pos) const {
		assert(pos >= 0);
		assert(pos < Wint);

		int value = 0;
		for (int i = 0; i < num_dims_; i++) {
			value |= (getBitDim(pos, i) << (num_dims_ - i - 1)); //FIXME here, read comment above.
		}
		return value;
	}

	inline tdata comp(int i) const {
		assert(i >= 0);
		assert(i < num_dims_);
		return data_point_[i];
	}

	inline void set_comp(int i, tdata value) {
		assert(i >= 0);
		assert(i < num_dims_);
		data_point_[i] = value;
	}

	inline tdata& operator[](int i) {
		assert(i >= 0);
		assert(i < num_dims_);
		return data_point_[i];
	}

	inline tdata const& operator[](int i) const {
		assert(i >= 0);
		assert(i < num_dims_);
		return data_point_[i];
	}

	inline bool operator==(const Point<tdata> &x) const {
		assert(num_dims_ == x.num_dims_);

		return std::equal(data_point_, data_point_ + num_dims_, x.data_point_);
		//if (0 == Point::cmp(x,*this)) return true;
		//return false;

		//if (0 == memcmp(data_point_, x.data_point_, sizeof(tdata)*num_dims_)) return true;
		//return false;
	}

	inline bool operator<(const Point<tdata> &x) const {
		assert(num_dims_ == x.num_dims_);

		for (int i = 0; i < num_dims_; i++) {
			if (!(data_point_[i] < x.data_point_[i]))
				return false;
		}

		return true;
	}

	inline bool operator<=(const Point<tdata> &x) const {
		assert(num_dims_ == x.num_dims_);

		for (int i = 0; i < num_dims_; i++) {
			if (!(data_point_[i] <= x.data_point_[i]))
				return false;
		}

		return true;
	}

	//Imitating std::rel_ops
	inline bool operator!=(const Point<tdata> &y) const {
		return !(*this == y);
	}
//    inline bool operator>(const Point<tdata> &y) const {
//            return y < *this;
//        }
//    inline bool operator<=(const Point<tdata> &y) const {
//            return !(y<*this);
//        }
//    inline bool operator>=(const Point<tdata> &y) const {
//            return !(*this < y);
//        }

	const Point<tdata>& operator+=(const unsigned int scalar) {
		for (int i = 0; i < num_dims_; i++) {
			data_point_[i] += scalar;
		}
		return *this;
	}

	const Point<tdata>& operator+(const unsigned int scalar) const {
		return Point<tdata>(*this) += scalar;
	}

	int num_dims() const {
		return num_dims_;
	}

	static bool myisless(const tdata x, const tdata y) {
		if (x > y)
			return false;
		else
			return x < (x ^ y);
	}
	// only valid when k=2 (buuuuu!)
	static bool cmpmorton(const Point<tdata> &x, const Point<tdata> &y) {
		assert(x.num_dims_ == y.num_dims_);
		int i = 0;
		for (int j = 1; j < x.num_dims_; j++) {
			if (myisless(x[i] ^ y[i], x[j] ^ y[j])) {
				i = j;
			}
		}
		return x[i] < y[i];
	}

	/*
	 static int cmp(const Point<tdata> &x, const Point<tdata> &y) {
	 assert(x.num_dims_ == y.num_dims_);
	 return std::lexicographical_compare(x.data_point_,x.data_point_+x.num_dims, y.data_point_, y.data_point_+x.num_dims);
	 }*/

	void print() const {
		for (int i = 0; i < num_dims_; i++) {
			printf("%u ", data_point_[i]);
		}
		printf("\n");
	}
private:
	//tdata *data_point_;
	tdata data_point_[4];
	int num_dims_;

};

} /* namespace cqtree_utils */

#endif /* POINT_H_ */
