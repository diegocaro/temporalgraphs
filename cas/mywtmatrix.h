/*
 * mywtmatrix.h
 *
 *  Created on: Mar 25, 2014
 *      Author: diegocaro
 */

#ifndef MYWTMATRIX_H_
#define MYWTMATRIX_H_
#include <wavelet_matrix.h>

inline void append_odd(uint symbol, uint freq, uint *res) {
  if (freq % 2 == 1 && freq > 0) { *res+=1; res[*res] = symbol; };
}

inline void append_symbol(uint symbol, uint freq, uint *res) {
  *res += 1; res[*res] = symbol;
}


inline void append_dummy(uint symbol, uint freq, uint *res) {
  *res += 1;
}

typedef void (*action)(uint, uint, uint*);

class MyWaveletMatrix : public WaveletMatrix {
public:
	void select_all(uint symbol, size_t *res);


	template<action F>
	void range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, uint *res);

protected:
	void _select_all(size_t start, size_t end, uint symbol, uint level, size_t *res);

	template<action F>
	void _range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, uint vocmin, uint vocmax, uint level,  uint *res);
};


#endif /* MYWTMATRIX_H_ */
