#include <string.h>
#include "wavelet_matrix.h"


/*
WaveletMatrix::WaveletMatrix ( uint * symbols, size_t n, BitSequenceBuilder * bmb) {
	max_v=max_value(symbols,n);
	height=bits(max_v);
	
	Z = new uint[height];
	bitmap = new BitSequence*[height];
	
	this->n = n;
	
	uint *b;
	uint tmp;
	
	uint i,j,k;
	for(i = 0; i < height; i++) {
		b = new uint [n/W + 1];
		//printf("level: %u\n",i);
		for(k = 0; k < n; k++) {
			//			printf("%u\n", is_set(symbols[k],i));
			if(is_set(symbols[k], i)) {
				bitset(b, k);
			}
			//printf("symbols[%u] = %u -> %u\n", k, symbols[k], is_set(symbols[k], i));
		}

		bitmap[i] = bmb->build(b ,n);
		
		Z[i] = bitmap[i]->rank0(n-1);
		
		
		//printf("Z[%u]: %u\n", i, Z[i]);	
		//here swap values by bit value :)
		for(k = 0; k < n; k++) {
		//	printf("%u", is_set(symbols[k], i));
		//	printf("k: %u\n",k);

			for (j = 0; j < n-1; j++) {
				if (is_set(symbols[j], i) > is_set(symbols[j+1], i)) {
		//			printf("j: %u\n", j);
		//			printf("swap(s[%u]=%u, s[%u]=%u)\n",j,symbols[j],j+1,symbols[j+1]);
					tmp = symbols[j];
					symbols[j] = symbols[j+1];
					symbols[j+1] = tmp;
				}
			}
		}
	}
	
	delete [] symbols;
}
*/


WaveletMatrix::WaveletMatrix ( uint * symbols, size_t n, BitSequenceBuilder * bmb) {
	max_v=max_value(symbols,n);
	height=bits(max_v);
	
	Z = new size_t[height];
	bitmap = new BitSequence*[height];
	
	this->n = n;
	
	uint *b;
	
	uint i,k;
	
	uint *zeros = new uint[n];
	uint *ones = new uint[n];
	uint czeros = 0;
	uint cones = 0;
	
	for(i = 0; i < height; i++) {
		fprintf(stderr, "creating level: %u\n",i);

		b = new uint [n/W + 1];
    fill_n(b, n/W+1, 0); //set bits to zero
    
		for(k = 0; k < n; k++) {
			//			printf("%u\n", is_set(symbols[k],i));
			if(is_set(symbols[k], i)) {
				cds_utils::bitset(b, k);
				ones[cones++] = symbols[k];
			}
			else {
				zeros[czeros++] = symbols[k];
			}
			//printf("symbols[%u] = %u -> %u\n", k, symbols[k], is_set(symbols[k], i));
		}

		bitmap[i] = bmb->build(b ,n);

		//Z[i] = bitmap[i]->rank0(n-1);
		Z[i] = czeros;

		memcpy(symbols, zeros, sizeof(uint) * czeros);
		memcpy(&symbols[czeros], ones, sizeof(uint) * cones);

		czeros = 0;
		cones = 0;
	}

	
	delete [] zeros;
	delete [] ones;
	delete [] symbols;

}


size_t WaveletMatrix::rank(uint symbol, size_t pos) {
	size_t start = 0;
	
	
	for (uint l = 0; l < height; l++) {
		if (is_set(symbol, l)) {
			//			if (start > 0)//little optimization, can be removed
			//				start = bitmap[l]->rank1(start-1);
			//			start += Z[l];
			
			start = bitmap[l]->rank1(start-1) + Z[l];
			pos = bitmap[l]->rank1(pos) + Z[l] - 1;
			
		} else {
			//			if (start > 0) //little optimization, can be removed
			//				start = start - bitmap[l]->rank1(start-1);
			
			start = start - bitmap[l]->rank1(start-1);
			pos = pos - bitmap[l]->rank1(pos);
		}
		
		//printf("start: %u\n", start);
		//printf("pos: %u\n", pos);
		
		if ( pos - start +1 == 0) return 0;
		//printf("diff: %u\n", pos - start);
	}
	//printf("\n");
	
	return pos - start +1;
}

size_t WaveletMatrix::select(uint symbol, size_t pos) {
	return _select(symbol, pos, 0, 0);
}


size_t WaveletMatrix::_select(uint symbol, size_t pos, uint l, size_t pre) {
	if (l == height) {
		//printf(" pre: %u ", pre);
		//printf(" pos: %u ", pos);
		return pre + pos-1;
	}
	
	if (is_set(symbol, l)) {
		//			if (start > 0)//little optimization, can be removed
		//				start = bitmap[l]->rank1(start-1);
		//			start += Z[l];
		
		pre = bitmap[l]->rank1(pre-1) + Z[l];
		pos = _select(symbol, pos, l+1, pre);
		
		return bitmap[l]->select1(pos-Z[l]+1);
	} else {
		//			if (start > 0) //little optimization, can be removed
		//				start = start - bitmap[l]->rank1(start-1);
		pre = pre - bitmap[l]->rank1(pre-1);
		pos = _select(symbol, pos, l+1, pre);
		
		return  bitmap[l]->select0(pos+1);
	}
}


void WaveletMatrix::select_all(uint symbol, vector<size_t> &res) {
	_select_all(0, n, symbol, 0,  res);
}


void WaveletMatrix::_select_all(size_t start, size_t end, uint symbol, uint level, vector<size_t> &res) {
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
			res.push_back(start+i);
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
		
		for(i = 0; i < res.size(); i++) {
			//printf("res[%u] = %u\n",i ,res[i]);
			res[i] = bitmap[level]->select1(res[i]-Z[level]+1);
			//printf("res[%u] = %u\n",i ,res[i]);
		}
	} else {
		_select_all(s0, e0, symbol, level+1, res);
		
		for(i = 0; i < res.size(); i++) {
//printf("res[%u] = %u\n",i ,res[i]);
			res[i] = bitmap[level]->select0(res[i]+1);
//printf("res[%u] = %u\n",i ,res[i]);
		}
		
	}
}



size_t WaveletMatrix::next_value_pos(uint symbol, size_t start, size_t end) {
	uint success = 1;
	size_t pos;
	//printf("checking symbol: %u\n", symbol);
	pos = _next_value_pos(symbol, start, end, 0,  1u << height, 0, success);
	//printf("pos: %u (of %u)\n", pos, wt->wt->size);
	if (success)
		return pos;
	else
		return n;
	
}

size_t WaveletMatrix::_next_value_pos(uint symbol, size_t start, size_t end, uint vocmin, uint vocmax, uint level, uint &success) {
		
	uint vocmid;
	vocmid = (vocmax-vocmin)/2 + vocmin;
	/*printf("range: [%u, %u]\n",start,end);
	printf("symbol: %u\n", symbol);
	printf("vocmin: %u\n", vocmin);
	printf("vocmid: %u\n", vocmid);
	printf("vocmax: %u\n", vocmax);*/
		
		
	if (start >= end) {
		//printf("start >= end\n");
		success = 0;
		return 0;
	}
		
	if (height == level) {
		success = 1;
		//printf("symbol: %u start: %u end: %u\n",symbol,start,end);
		return start;
			
	}
		
	size_t s0,e0,s1,e1;
		
	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);
	
	//printf("s1: %u\ne1: %u\n", s1,e1);			

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);
	size_t r,c;
	if (symbol >= vocmid) {
		//printf("right side\n");
		r = _next_value_pos(symbol, s1 + Z[level], e1 + Z[level], vocmid, vocmax, level+1, success);
		c = bitmap[level]->select1(r-Z[level]+1);
		return c;
		
	}
	else {
		//printf("left side\n");
		r = _next_value_pos(symbol, s0, e0, vocmin, vocmid, level+1, success);
		if (success!=0) {
			c = bitmap[level]->select0(r+1);
			return c;
		}
		else {
			//printf("left failed, going right side\n");
			r = _next_value_pos(vocmid, s1 + Z[level], e1 + Z[level], vocmid, vocmax, level+1, success);
			c = bitmap[level]->select1(r-Z[level]+1);
			return c;
		}
		
	}

	return n;
}


uint WaveletMatrix::prev_value(uint symbol, size_t start, size_t end) {
	uint success = 1;
	size_t pos;

	pos = _prev_value(symbol, start, end, 0,  1u << height, 0, success);
	//printf("pos: %u (of %u)\n", pos, wt->wt->size);
	if (success)
		return pos;
	else
		return n;
	
}

uint WaveletMatrix::_prev_value(uint symbol, size_t start, size_t end, uint vocmin, uint vocmax, uint level, uint &success) {
		
	uint vocmid;
	vocmid = (vocmax-vocmin)/2 + vocmin;
	//printf("vocmin: %u\n", vocmin);
//	printf("vocmid: %u\n", vocmid);
//	printf("vocmax: %u\n", vocmax);
		
		
	if (start >= end) {
		//printf("start >= end\n");
		success = 0;
		return 0;
	}
		
	if (height == level) {
		success = 1;
		//printf("symbol: %u start: %u end: %u\n",symbol,start,end);
		return vocmin;
			
	}
		
	uint s0,e0,s1,e1;
		
	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);
	
	//printf("s1: %u\ne1: %u\n", s1,e1);			

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);
	size_t r;
	if (symbol >= vocmid) {
		//printf("right side\n");
		r = _prev_value(symbol, s1 + Z[level], e1 + Z[level], vocmid, vocmax, level+1, success);
		if (success!=0) {
			return r;
		}
		else {
			r = _prev_value(vocmid-1, s0, e0, vocmin, vocmid, level+1, success);
			return r;
		}
		
	}
	else {
		r = _prev_value(vocmid-1, s0, e0, vocmin, vocmid, level+1, success);
		return r;
	}

	return n;
}



uint WaveletMatrix::access(size_t pos) {
	
	size_t ret=0;
	for (uint l = 0; l < height; l++) {
		size_t r = 0;		
		//		printf("pos: %u\n", pos);
		if ( bitmap[l]->access(pos, r) ) {
			//			printf("1");
			pos = Z[l] + r - 1;
			ret = ret | (1 << (height-l-1));
		}
		else {
			//			printf("0");
			pos = r - 1;
		}
		
	}
	//	printf("ret: %u\n", ret);
	return ret;
}

void WaveletMatrix::range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, vector< pair<uint, size_t> > &res) {
	_range_report(start, end, lowvoc, uppvoc, 0, 1u << height, 0,  res);
}



void WaveletMatrix::_range_report(size_t start, size_t end, uint lowvoc, uint uppvoc, uint vocmin, uint vocmax, uint level, vector< pair<uint, size_t> > &res) {
		
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
			
			
		if ( vocmin >= lowvoc && vocmin < uppvoc ) {
			res.push_back(make_pair(vocmin, end-start));
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
			

	_range_report(s0, e0, lowvoc, uppvoc, vocmin, vocmid, level+1, res);
	_range_report(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, vocmid, vocmax, level+1, res);
}







uint WaveletMatrix::max_value(uint *symbols, size_t n) {
	uint max_v = 0;
	for(size_t i=0;i<n;i++)
		max_v = max(symbols[i],max_v);
	return max_v;
}

uint WaveletMatrix::bits(uint val) {
	uint ret = 0;
	while(val!=0) {
		ret++;
		val >>= 1;
	}
	return ret;
}

bool WaveletMatrix::is_set(uint val, uint ind) const
{
	assert(ind<height);
	//return (val & (1<<(height-ind-1)))!=0;
	return (val >> (height-ind-1)) & 1U;
}

void WaveletMatrix::save(ofstream &f) {
	cds_utils::saveValue<WaveletMatrix>(f, this, 1);
	saveValue<size_t>(f, Z, height);
	for(uint i = 0; i < height; i++)
		bitmap[i]->save(f);
}

size_t WaveletMatrix::getSize() {
	size_t size=0;
	
	size += sizeof(uint)*height; // Z values
	
	for(uint i = 0; i < height; i++)
		size += bitmap[i]->getSize();
	
	return size;
}

WaveletMatrix * WaveletMatrix::load(ifstream & f) {

	WaveletMatrix * ret;
	ret = cds_utils::loadValue<WaveletMatrix>(f, 1);

	ret->Z = loadValue<size_t>(f, ret->height);
	ret->bitmap = new BitSequence*[ret->height];

	for(uint i = 0; i < ret->height; i++) {
		ret->bitmap[i] = BitSequence::load(f);
	}

    return ret;
}

