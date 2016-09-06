/*
 * MyWaveletKdMatrix.h
 *
 *  Created on: Mar 26, 2014
 *      Author: diegocaro
 */

#ifndef MYWAVELETKDMATRIX_H_
#define MYWAVELETKDMATRIX_H_
#include <wavelet_kd_matrix.h>

inline void append_odd(uint symbol, uint freq, uint *res) {
  if (freq % 2 == 1) { *res+=1; res[*res] = symbol; };
}

inline void append_symbol(uint symbol, uint freq, uint *res) {
  *res += 1; res[*res] = symbol;
}

typedef void (*action)(uint, uint, uint*);


inline void append_change(usym symbol, size_t start, size_t end, size_t &res) {
    res += 1;
    //printf("%u %u\n",symbol.x, symbol.y);
}

inline void append_actived(usym symbol, size_t start, size_t end, size_t &res) {
    if (start %2 == 0 || (end-start)>1) res += 1;
}

inline void append_deactived(usym symbol, size_t start, size_t end, size_t &res) {
    if (start %2 == 1  || (end-start)>1) res += 1;
}

typedef void (*filter)(usym, size_t, size_t, size_t &);

class MyWaveletKdMatrix : public WaveletKdMatrix {
public:
	template<action F>
	void axis(size_t start, size_t end, uint axis, uint node, uint *res);


	void rankall(size_t start, size_t end, size_t &res);

	template<filter F>
	void range(size_t start, size_t end, size_t &res);


protected:
	template<action F>
	void _axis(size_t start, size_t end, uint axis, usym node, uint level, uint value, uint *res);


	void _rankall(size_t start, size_t end, usym symbol, uint level, size_t &res);

	template<filter F>
	  void _range(size_t start, size_t end, usym symbol, uint level, size_t p,size_t &res);
};

#endif /* MYWAVELETKDMATRIX_H_ */
