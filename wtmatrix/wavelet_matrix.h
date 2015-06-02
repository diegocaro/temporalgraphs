#ifndef _WVTREE_MATRIX_H
#define _WVTREE_MATRIX_H


#include <sys/types.h>
#include <libcdsBasics.h>
#include <BitSequence.h>
#include <BitSequenceBuilder.h>
#include <cassert>

#include <vector>
#include <utility>
using namespace cds_static;


// Max alphabet size: 2^32-1
// Max sequence size: 2^64-1

class WaveletMatrix {
public:
	
	WaveletMatrix(uint * symbols, size_t n, BitSequenceBuilder * bmb);
	WaveletMatrix() {};

	size_t rank(uint symbol, size_t pos); //return frequency
	size_t select(uint symbol, size_t pos); //return position

	uint access(size_t pos); //return symbol

	size_t next_value_pos(uint symbol, size_t start, size_t end); //return position
	uint prev_value(uint symbol, size_t start, size_t end); //return symbol



	void range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, vector< pair<uint,size_t> > &res);

	void select_all(uint symbol, vector<size_t> &res);

	static WaveletMatrix* load(ifstream &f);
	void save(ofstream &f);
	size_t getSize();
protected:
	size_t _select(uint symbol, size_t pos, uint level, size_t pre);


	void _range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, uint vocmin, uint vocmax, uint level,  vector< pair<uint,size_t> > &res);


	void _select_all(size_t start, size_t end, uint symbol, uint level, vector<size_t> &res);

	
	size_t _next_value_pos(uint symbol, size_t start, size_t end, uint vocmin, uint vocmax, uint level, uint &success);
	uint _prev_value(uint symbol, size_t start, size_t end, uint vocmin, uint vocmax, uint level, uint &success);
        
        
	size_t *Z;
	
	/** Height of the Wavelet Tree. */
	uint height, max_v;
	
	size_t n;
	
	BitSequence **bitmap;
	
	
/** Obtains the maximum value from the string
 * symbols of length n */
uint max_value(uint *symbols, size_t n);

/** How many bits are needed to represent val */
uint bits(uint val);

/** Returns true if val has its ind-th bit set
 * to one. */
bool is_set(uint val, uint ind) const;

};


#endif
