/*
 * MyWaveletKdMatrix.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: diegocaro
 */

#include "mywtkdmatrix.h"
#include "debug.h"

#include <queue>

struct wt_info {
	size_t start;
	size_t end;
	uint level;
	uint value;
};


template void MyWaveletKdMatrix::axis<append_odd>(size_t start, size_t end, uint axis, uint node, uint *res);
template void MyWaveletKdMatrix::_axis<append_odd>(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res);

template void MyWaveletKdMatrix::axis<append_symbol>(size_t start, size_t end, uint axis, uint node, uint *res);
template void MyWaveletKdMatrix::_axis<append_symbol>(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res);


template void MyWaveletKdMatrix::range<append_change>(size_t start, size_t end, size_t &res);
template void MyWaveletKdMatrix::range<append_actived>(size_t start, size_t end, size_t &res);
template void MyWaveletKdMatrix::range<append_deactived>(size_t start, size_t end, size_t &res);

template<action F>
void MyWaveletKdMatrix::axis(size_t start, size_t end, uint axis, uint node, uint *res) {
	usym n = {0, 0};
	if(axis == 0) {
		n.x = node;
	}
	else {
		n.y = node;
	}
	*res = 0;
	LOG("neighbors of {%u, %u}", n.x, n.y);

	//A queue based algorithm is used
	_axis<F>(start, end, axis, n, 0, 0, res);

	// Iterative code using a Queue
	/*
	queue<struct wt_info> q;
	wt_info z,t;
	z.start = start;
	z.end = end;
	z.level = 0;
	z.value = 0;
	q.push(z);
	uint value;

	while (!q.empty()) {
		z = q.front();
		q.pop();


		size_t s0,e0,s1,e1;

		s1 = bitmap[z.level]->rank1(z.start-1);
		e1 = bitmap[z.level]->rank1(z.end-1);

		//printf("s1: %u\ne1: %u\n", s1,e1);

		s0 = z.start - s1;
		e0 = z.end - e1;
	//	printf("s0: %u\ne0: %u\n", s0,e0);

		if ( z.level % 2 == axis) {
			// visitando la parte del nodo que me interesa :)
			if (is_set(n,z.level)) {
			//	printf("1");
				//bajar nodo derecho
				t.start = s1+Z[z.level];
				t.end = e1+Z[z.level];
			}
			else {
			//	printf("0");
				//bajar nodo izq

				t.start = s0;
				t.end = e0;
			}

			if (t.start >= t.end) continue;

			t.level = z.level +1;
			t.value = z.value;

			if (t.level == height) {
				F(t.value, t.end-t.start,res);
				continue;
			}

			q.push(t);

		}
		else { // bajar por ambos!
			//printf("X");

			t.start = s0;
			t.end = e0;

			if (t.start >= t.end) goto second_rec;

			t.level = z.level +1;
			t.value = z.value;

			if (t.level == height) {
				F(t.value, t.end-t.start,res);
				goto second_rec;
			}

			q.push(t);

	second_rec:
			t.start = s1+Z[z.level];
			t.end = e1+Z[z.level];

			if (t.start >= t.end) continue;

			t.level = z.level +1;
			t.value = z.value | (1 << (height/2-z.level/2-1));

			if (t.level == height) {
				F(t.value, t.end-t.start,res);
				continue;
			}

			q.push(t);
		}

	}
	*/

}

template<action F>
void MyWaveletKdMatrix::_axis(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res) {
	LOG("level: %u", level);
	if (start >= end) {
//		printf("start >= end\n");
		return;
	}

	if (level == height) {
		INFO("nodo hoja\n");

		F(value, end-start, res);

		return;
	}

	size_t s0,e0,s1,e1;

	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);

	//printf("s1: %u\ne1: %u\n", s1,e1);

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);

	if ( level % 2 == axis) {

		// visitando la parte del nodo que me interesa :)
		if (is_set(node,level)) {
		//	printf("1");
			//bajar nodo derecho
			_axis<F>(s1 + Z[level], e1 + Z[level], axis, node, level+1, value, res);
		}
		else {
		//	printf("0");
			//bajar nodo izq
			_axis<F>(s0, e0, axis, node, level+1, value, res);
		}
	}
	else { // bajar por ambos!
		//printf("X");
		_axis<F>(s0, e0, axis, node, level+1, value, res);

		value = value | (1 << (height/2-level/2-1));
		_axis<F>(s1 + Z[level], e1 + Z[level], axis, node, level+1, value, res);
	}

	return;
}




void MyWaveletKdMatrix::rankall(size_t start, size_t end, size_t &res) {
	usym a = {0,0};

	//_range(start, end, lowvoc, uppvoc, 0, 0, 1u << height/2, 1u << height/2 , 0, res);
	_rankall(start, end, a, 0, res);
}

void MyWaveletKdMatrix::_rankall(size_t start, size_t end, usym symbol, uint level, size_t &res) {


//	printf("level: %u\n", level);
	if (start >= end) {
//		printf("start >= end\n");
		return;
	}

	if (height == level) {
//		printf("leaf node\n");
		if ((end-start)%2 == 1) {
			//printf("[%u, %u] -> %u\n", symbol.x, symbol.y, end -start);
			//pair<usym,uint> p(symbol, end-start);
			//res.push_back(p);
			res += 1;
		}
		return;
	}

	size_t s0,e0,s1,e1;

	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);

	//printf("s1: %u\ne1: %u\n", s1,e1);

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);

	if (level%2 == 0) {
		// value for x :)

		_rankall(s0, e0, symbol, level+1, res);

		symbol.x = symbol.x | (1 << (height/2-level/2-1));
		_rankall(s1 + Z[level], e1 + Z[level], symbol, level+1, res);
	}
	else {
		_rankall(s0, e0,  symbol, level+1, res);

		symbol.y = symbol.y | (1 << (height/2-level/2-1));
		_rankall(s1 + Z[level], e1 + Z[level], symbol, level+1, res);
	}

}

template<filter F>
void MyWaveletKdMatrix::range(size_t start, size_t end, size_t &res) {
    usym a = {0,0};

    //_range(start, end, lowvoc, uppvoc, 0, 0, 1u << height/2, 1u << height/2 , 0, res);
    _range<F>(start, end, a, 0, 0, res);
}

template<filter F>
void MyWaveletKdMatrix::_range(size_t start, size_t end, usym symbol, uint level, size_t p, size_t &res) {
  // the "p" value will give us the position of the current node in the bitmap

//  printf("level: %u\n", level);
    if (start >= end) {
//      printf("start >= end\n");
        return;
    }

    if (height == level) {
      //printf("start-p: %lu\n",start-p);
      //printf("end-start: %lu\n",end-start);
        F(symbol, start-p, end-p, res);
        return;
    }

    size_t s0,e0,s1,e1;
    
    s1 = bitmap[level]->rank1(start-1);
    e1 = bitmap[level]->rank1(end-1);

    size_t p0,p1;

    p1 = bitmap[level]->rank1(p-1);
    p0 = p - p1;

    //printf("s1: %u\ne1: %u\n", s1,e1);

    s0 = start - s1;
    e0 = end - e1;

    //printf("s0: %u\ne0: %u\n", s0,e0);
    //printf("p0: %lu\np1: %lu\n",p0,p1);

    if (level%2 == 0) {
        // value for x :)

      _range<F>(s0, e0, symbol, level+1, p0, res);

      symbol.x = symbol.x | (1 << (height/2-level/2-1));
      _range<F>(s1 + Z[level], e1 + Z[level], symbol, level+1, p1 + Z[level], res);
    }
    else {
      _range<F>(s0, e0,  symbol, level+1, p0, res);

      symbol.y = symbol.y | (1 << (height/2-level/2-1));
      _range<F>(s1 + Z[level], e1 + Z[level], symbol, level+1, p1 + Z[level], res);
    }

}

