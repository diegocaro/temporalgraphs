#ifndef _WVTREE_KD_MATRIX_H
#define _WVTREE_KD_MATRIX_H


#include <sys/types.h>
#include <libcdsBasics.h>
#include <BitSequence.h>
#include <BitSequenceBuilder.h>
#include <cassert>

#include "symbols.h"

using namespace cds_static;
class WaveletKdMatrix {
public:
	
	WaveletKdMatrix(usym * symbols, size_t n, BitSequenceBuilder * bmb);
	WaveletKdMatrix( usym * symbols, size_t n, BitSequenceBuilder * bmb, bool lessmemory);
	WaveletKdMatrix() {};
	size_t rank(usym symbol, size_t pos);
	size_t select(usym symbol, size_t pos);

	usym access(size_t pos);
	void range(size_t start, size_t end, usym lowvoc, usym uppvoc, vector<pair<usym,size_t> > &res);

	void rankall_slow(size_t start, size_t end, vector<pair<usym,size_t> > &res);

	void rankall(size_t start, size_t end, vector<pair<usym,size_t> > &res);

	void axis(size_t start, size_t end, uint axis, uint node, vector< pair<uint,size_t> > &res);

	static WaveletKdMatrix* load(ifstream &f);
	void save(ofstream &f);

  size_t getSize() {
    size_t levels=0;
    for(uint i=0; i < height; i++) levels += bitmap[i]->getSize() + sizeof(BitSequence *);
    return levels + sizeof(WaveletKdMatrix);
  }

	//protected:


	size_t _select(usym symbol, size_t pos, uint level, size_t pre);
	void _range(size_t start, size_t end, usym lowvoc, usym uppvoc, usym vocmin, usym vocmax, uint level, vector<pair<usym,size_t> > &res);

	void _rankall_slow(size_t start, size_t end, usym vocmin, usym vocmax, uint level, vector<pair<usym,size_t> > &res);

	void _rankall(size_t start, size_t end, usym symbol, uint level, vector<pair<usym,size_t> > &res);

	//void _range(uint start, uint end, usym lowvoc, usym uppvoc, uint Xvocmin, uint Xvocmax, uint Yvocmin, uint Yvocmax, uint level, vector<pair<usym,uint> > &res);

	void _axis(size_t start, size_t end, uint axis, usym node, uint level, uint value, vector< pair<uint,size_t> > &res);

	size_t *Z;
	
	/** Height of the Wavelet Tree. */
	uint height, max_v;
	
	size_t n;
	
	BitSequence **bitmap;
	
	
/** Obtains the maximum value from the string
 * symbols of length n */
uint max_value(usym *symbols, size_t n);

/** How many bits are needed to represent val */
uint bits(uint val);

/** Returns true if val has its ind-th bit set
 * to one. */
bool is_set(usym val, uint ind) const;

};


#endif
