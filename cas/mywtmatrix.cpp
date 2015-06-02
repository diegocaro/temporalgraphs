

#include <libcdsBasics.h>
#include "mywtmatrix.h"


template void MyWaveletMatrix::range_report<append_odd>(size_t start, size_t end, uint lowvoc, uint uppvoc, uint *res);
template void MyWaveletMatrix::range_report<append_symbol>(size_t start, size_t end, uint lowvoc, uint uppvoc, uint *res);
template void MyWaveletMatrix::range_report<append_dummy>(size_t start, size_t end, uint lowvoc, uint uppvoc, uint *res);


template<action F>
void MyWaveletMatrix::range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, uint *res) {
	*res = 0;
	_range_report<F>(start, end, lowvoc, uppvoc, 0, 1u << height, 0,  res);
}


template<action F>
void MyWaveletMatrix::_range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, uint vocmin, uint vocmax, uint level, uint *res) {

	uint vocmid;
	vocmid = (vocmax-vocmin)/2 + vocmin;
	//printf("vocmin: %u\n", vocmin);
//	printf("vocmid: %u\n", vocmid);
//	printf("vocmax: %u\n", vocmax);


	if (start >= end) {
//		printf("start >= end\n");
		return;
	}
	if ( (lowvoc > vocmax) || (vocmin >= uppvoc)) {

		/*printf("node voc range [%u, %u)\n ", vocmin, vocmax);
		printf(" should be in [%u, %u) \n", lowvoc, uppvoc);

		printf("me fui aca\n");*/
//		printf("out of range\n");
		return;
	}

	if (height == level) {
//		printf("leaf \n");

		if ( vocmin >= lowvoc && vocmin < uppvoc) {
			F(vocmin, end-start, res);
		}
		return;

	}

	uint s0,e0,s1,e1;

	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);

	//printf("s1: %u\ne1: %u\n", s1,e1);

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);


	_range_report<F>(s0, e0, lowvoc, uppvoc, vocmin, vocmid, level+1, res);
	_range_report<F>(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, vocmid, vocmax, level+1, res);
}

void MyWaveletMatrix::select_all(uint symbol, size_t *res) {
	*res = 0;
	_select_all(0, n, symbol, 0,  res);
}


void MyWaveletMatrix::_select_all(size_t start, size_t end, uint symbol, uint level, size_t *res) {
	size_t i;
	if (start >= end) {
		//printf("start >= end\n");
		return;
	}
	if (level == height) {
		//printf(" pre: %u ", pre);
		//printf(" pos: %u ", pos);

		//return start-end;
//		printf("start: %u end: %u\n", start, end);
		for(i = 0; i < end-start;i++) {
			*res += 1;
			res[*res] = start+i;
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

	if (is_set(symbol, level)) {
		_select_all(s1+Z[level], e1+Z[level], symbol, level+1, res);

		for(i = 1; i <= *res; i++) {
			//printf("res[%u] = %u\n",i ,res[i]);
			res[i] = bitmap[level]->select1(res[i]-Z[level]+1);
			//printf("res[%u] = %u\n",i ,res[i]);
		}
	} else {
		_select_all(s0, e0, symbol, level+1, res);

		for(i = 1; i <= *res; i++) {
//printf("res[%u] = %u\n",i ,res[i]);
			res[i] = bitmap[level]->select0(res[i]+1);
//printf("res[%u] = %u\n",i ,res[i]);
		}

	}
}
