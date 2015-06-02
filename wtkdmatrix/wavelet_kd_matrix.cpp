#include <cstring>
#include "wavelet_kd_matrix.h"
#include "debug.h"

u_long in1(uint x, uint y) {
        //unsigned short x;   // Interleave bits of x and y, so that all of the
        //unsigned short y;   // bits of x are in the even positions and y in the odd;
        uint i;
        u_long z = 0; // z gets the resulting Morton Number.

        for (i = 0; i < 32; i++) // unroll for more speed...
        {
         //z |= (x & 1U << i) << i | (y & 1U << i) << (i + 1);
                z |= (x & 1UL << i) << (i + 1) | (y & 1UL << i) << i;
        }

        return z;
}

void de1(u_long z, uint *x, uint *y) {
        uint i;
        *x = 0;
        *y = 0;
        for (i = 0; i < 64; i+=2) {
//              printf("aa: %lu\n", z & (1UL << (i+1)) );
//              printf("bb: %lu\n", z & (1UL << (i)));
                *x |= (z & (1UL << (i+1))) >> (i/2+1);
                *y |= (z & (1UL << i)) >> i/2;
        }
}

WaveletKdMatrix::WaveletKdMatrix( usym * symbols, size_t n, BitSequenceBuilder * bmb, bool lessmemory) {
	max_v = max_value(symbols, n);
	height = 2 * bits(max_v);
	
	Z = new size_t[height];
	bitmap = new BitSequence*[height];
	
	this->n = n;

	uint *b;
	usym tmp;
	
	size_t j,k;
	for(uint i = 0; i < height; i++) {
		b = new uint [n/W + 1];
    fill_n(b, n/W+1, 0); //set bits to zero
    
		//printf("level: %u\n",i);
		for(k = 0; k < n; k++) {
			//			printf("%u\n", is_set(symbols[k],i));
			if(is_set(symbols[k], i)) {
				cds_utils::bitset(b, k);
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




WaveletKdMatrix::WaveletKdMatrix ( usym * symbols, size_t n, BitSequenceBuilder * bmb) {
	max_v = max_value(symbols, n);
	height = 2 * bits(max_v);

	Z = new size_t[height];
	bitmap = new BitSequence*[height];

	this->n = n;

	uint *b;
	
	size_t k;

	usym *zeros = new usym[n];
	usym *ones = new usym[n];
	uint czeros = 0;
	uint cones = 0;

	for(uint i = 0; i < height; i++) {
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

		memcpy(symbols, zeros, sizeof(usym) * czeros);
		memcpy(&symbols[czeros], ones, sizeof(usym) * cones);

		czeros = 0;
		cones = 0;
	}

	delete [] zeros;
	delete [] ones;
	delete [] symbols;
}






usym WaveletKdMatrix::access(size_t pos) {

	usym ret={0, 0};
	for (uint l = 0; l < height; l++) {
		size_t r = 0;
		//printf("pos: %u\n", pos);

		if ( bitmap[l]->access(pos, r) ) {
			//			printf("1");
			pos = Z[l] + r - 1;

			if (l%2 == 0) {
				ret.x = ret.x | (1 << (height/2-l/2-1));
			}
			else {
				ret.y = ret.y | (1 << (height/2-l/2-1));
			}

		}
		else {
			//			printf("0");
			pos = r - 1;
		}

	}
	//	printf("ret: %u\n", ret);
	return ret;
}


size_t WaveletKdMatrix::rank(usym symbol, size_t pos) {
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

size_t WaveletKdMatrix::select(usym symbol, size_t pos) {
	return _select(symbol, pos, 0, 0);
}


size_t WaveletKdMatrix::_select(usym symbol, size_t pos, uint l, size_t pre) {
	if (l == height) {
		//printf(" pre: %u ", pre);
		//printf(" pos: %u ", pos);

		if (pre + pos - 1 == n) return n*2;
		else return pre + pos-1;
	}
	
	if (is_set(symbol, l)) {
		
		pre = bitmap[l]->rank1(pre-1) + Z[l];
		pos = _select(symbol, pos, l+1, pre);

		if (pos < n)
			return bitmap[l]->select1(pos-Z[l]+1);
		else
			return n;
	} else {
		pre = pre - bitmap[l]->rank1(pre-1);
		pos = _select(symbol, pos, l+1, pre);

		if (pos < n)
			return  bitmap[l]->select0(pos+1);
		else
			return n;
	}
}

//TODO FINISH THIS METHOD
void WaveletKdMatrix::range(size_t start, size_t end, usym lowvoc, usym uppvoc, vector<pair<usym,size_t> > &res) {
	usym a = {0,0};
	usym b = {1u << height/2, 1u << height/2};

	//_range(start, end, lowvoc, uppvoc, 0, 0, 1u << height/2, 1u << height/2 , 0, res);
	_range(start, end, lowvoc, uppvoc, a, b , 0, res);
}
// TODO: add the "p" position
void WaveletKdMatrix::_range(size_t start, size_t end, usym lowvoc, usym uppvoc, usym vocmin, usym vocmax, uint level, vector<pair<usym,size_t> > &res) {
		
	printf("level: %u\n", level);
	if (start >= end) {
		printf("start >= end\n");
		return;
	}

	/// TODO chequear pertenencia del alfabeto



	if (height == level) {
		printf("leaf node\n");
		printf("[%u, %u] -> %lu\n", vocmin.x, vocmin.y, end -start);
		return;
	}

	/*

	if ( (lowvoc > vocmax) || (vocmin < lowvoc) || (vocmin >= uppvoc)) {

		//printf("node voc range [%u, %u)\n ", vocmin, vocmax);
		//printf(" should be in [%u, %u) \n", lowvoc, uppvoc);
//
//		printf("me fui aca\n");
		return;
	}

	if (vocmin +1== vocmax) {
	//	printf("leaf \n");
			
			
		if ( vocmin >= lowvoc && vocmin < uppvoc ) {
			res.push_back(vocmin);
			res.push_back(end-start);
		}
		return;
			
	}
	*/

	size_t s0,e0,s1,e1;
		
	s1 = bitmap[level]->rank1(start-1);
	e1 = bitmap[level]->rank1(end-1);
	
	//printf("s1: %u\ne1: %u\n", s1,e1);			

	s0 = start - s1;
	e0 = end - e1;
//	printf("s0: %u\ne0: %u\n", s0,e0);

	usym vocmid;
	uint mid;

	if (level%2 == 0) {
		mid = (vocmax.x-vocmin.x)/2 + vocmin.x;
		vocmid.x = mid;
		vocmid.y = vocmax.y;

		_range(s0, e0, lowvoc, uppvoc, vocmin, vocmid, level+1, res);

		vocmid.x = mid;
		vocmid.y = vocmin.y;

		_range(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, vocmid, vocmax, level+1, res);
	}
	else {
		mid = (vocmax.y-vocmin.y)/2 + vocmin.y;
		vocmid.x = vocmax.x;
		vocmid.y = mid;

		_range(s0, e0, lowvoc, uppvoc, vocmin, vocmid, level+1, res);

		vocmid.x = vocmin.x;
		vocmid.y = mid;

		_range(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, vocmid, vocmax, level+1, res);
	}

}



void WaveletKdMatrix::rankall_slow(size_t start, size_t end, vector<pair<usym,size_t> > &res) {
	usym a = {0,0};
	usym b = {1u << height/2, 1u << height/2};

	//_range(start, end, lowvoc, uppvoc, 0, 0, 1u << height/2, 1u << height/2 , 0, res);
	_rankall_slow(start, end, a, b , 0, res);
}

void WaveletKdMatrix::_rankall_slow(size_t start, size_t end, usym vocmin, usym vocmax, uint level, vector<pair<usym,size_t> > &res) {


//	printf("level: %u\n", level);
	if (start >= end) {
//		printf("start >= end\n");
		return;
	}

	if (height == level) {
//		printf("leaf node\n");
		if ((end-start)%2 == 1) {
			//printf("[%u, %u] -> %u\n", vocmin.x, vocmin.y, end -start);
			//pair<usym,uint> p(vocmin, end-start);
			//res.push_back(p);
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

	usym vocmid;
	uint mid;
	if (level%2 == 0) {
		mid = (vocmax.x-vocmin.x)/2 + vocmin.x;
		vocmid.x = mid;
		vocmid.y = vocmax.y;

		_rankall_slow(s0, e0, vocmin, vocmid, level+1, res);

		vocmid.x = mid;
		vocmid.y = vocmin.y;

		_rankall_slow(s1 + Z[level], e1 + Z[level], vocmid, vocmax, level+1, res);
	}
	else {
		mid = (vocmax.y-vocmin.y)/2 + vocmin.y;
		vocmid.x = vocmax.x;
		vocmid.y = mid;

		_rankall_slow(s0, e0,  vocmin, vocmid, level+1, res);

		vocmid.x = vocmin.x;
		vocmid.y = mid;

		_rankall_slow(s1 + Z[level], e1 + Z[level], vocmid, vocmax, level+1, res);
	}

}


void WaveletKdMatrix::rankall(size_t start, size_t end, vector<pair<usym,size_t> > &res) {
	usym a = {0,0};

	//_range(start, end, lowvoc, uppvoc, 0, 0, 1u << height/2, 1u << height/2 , 0, res);
	_rankall(start, end, a, 0, res);
}

void WaveletKdMatrix::_rankall(size_t start, size_t end, usym symbol, uint level, vector<pair<usym,size_t> > &res) {


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


//
//void WaveletKdMatrix::_range(uint start, uint end, usym lowvoc, usym uppvoc, uint Xvocmin, uint Yvocmin, uint Xvocmax, uint Yvocmax, uint level, vector<pair<usym,uint> > &res) {
//
//	uint isLevelX = (level%2 == 0);
//	uint vocmid;
//	printf("level: %u\n", level);
//	if (start >= end) {
//		printf("start >= end\n");
//		return;
//	}
//
//	if (height == level) {
//		printf("leaf node\n");
//		printf("[%u, %u] -> %u\n", Xvocmin, Yvocmin, end -start);
//		return;
//	}
//
//	/*
//
//	if ( (lowvoc > vocmax) || (vocmin < lowvoc) || (vocmin >= uppvoc)) {
//
//		//printf("node voc range [%u, %u)\n ", vocmin, vocmax);
//		//printf(" should be in [%u, %u) \n", lowvoc, uppvoc);
////
////		printf("me fui aca\n");
//		return;
//	}
//
//	if (vocmin +1== vocmax) {
//	//	printf("leaf \n");
//
//
//		if ( vocmin >= lowvoc && vocmin < uppvoc ) {
//			res.push_back(vocmin);
//			res.push_back(end-start);
//		}
//		return;
//
//	}
//	*/
//
//	uint s0,e0,s1,e1;
//
//	s1 = bitmap[level]->rank1(start-1);
//	e1 = bitmap[level]->rank1(end-1);
//
//	//printf("s1: %u\ne1: %u\n", s1,e1);
//
//	s0 = start - s1;
//	e0 = end - e1;
////	printf("s0: %u\ne0: %u\n", s0,e0);
//
//	if (isLevelX) {
//		vocmid = ceil((float)(Xvocmax-Xvocmin)/2) + Xvocmin;
//
//		_range(s0, e0, lowvoc, uppvoc, Xvocmin, Yvocmin, vocmid,  Yvocmax, level+1, res);
//		_range(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, vocmid, Yvocmin, Xvocmax, Yvocmax, level+1, res);
//	}
//	else {
//		vocmid = ceil((float)(Yvocmax-Yvocmin)/2) + Yvocmin;
//
//		_range(s0, e0, lowvoc, uppvoc, Xvocmin, Yvocmin, Xvocmax,  vocmid, level+1, res);
//		_range(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, Xvocmin, vocmid, Xvocmax, Yvocmax, level+1, res);
//	}
//
//}

//void WaveletKdMatrix::range(uint start, uint end, usym lowvoc, usym uppvoc, vector<pair<usym,uint> > &res) {
//	usym vocmin = {0,0};
//	usym vocmax = {1u << height/2, 1u << height/2};
//
//	_range(start, end, lowvoc, uppvoc, vocmin, vocmax, 0, res);
//}
//
//
//
//void WaveletKdMatrix::_range(uint start, uint end, usym lowvoc, usym uppvoc, usym vocmin, usym vocmax, uint level, vector<pair<usym,uint> > &res) {
//
//	uint vocmid;
//	usym vocmidL;
//	usym vocmidR;
//
//
//	if (start >= end) {
//		//printf("start >= end\n");
//		return;
//	}
//
//	if (level%2 == 0) {
//		vocmid = ceil((float)(vocmax.x-vocmin.x)/2) + vocmin.x;
//
//		vocmidL.x = vocmid;
//		vocmidL.y = vocmax.y;
//
//
//		vocmidR.x = vocmid;
//		vocmidR.y = vocmin.y;
//
//	}
//	else {
//		vocmid = ceil((float)(vocmax.y-vocmin.y)/2) + vocmin.y;
//
//		vocmidL.x = vocmax.x;
//		vocmidL.y = vocmid;
//
//
//		vocmidR.x = vocmin.x;
//		vocmidR.y = vocmid;
//
//	}
//
//
//	/*
//
//	if ( (lowvoc > vocmax) || (vocmin < lowvoc) || (vocmin >= uppvoc)) {
//
//		//printf("node voc range [%u, %u)\n ", vocmin, vocmax);
//		//printf(" should be in [%u, %u) \n", lowvoc, uppvoc);
////
////		printf("me fui aca\n");
//		return;
//	}
//
//	if (vocmin +1== vocmax) {
//	//	printf("leaf \n");
//
//
//		if ( vocmin >= lowvoc && vocmin < uppvoc ) {
//			res.push_back(vocmin);
//			res.push_back(end-start);
//		}
//		return;
//
//	}
//	*/
//
//	uint s0,e0,s1,e1;
//
//	s1 = bitmap[level]->rank1(start-1);
//	e1 = bitmap[level]->rank1(end-1);
//
//	//printf("s1: %u\ne1: %u\n", s1,e1);
//
//	s0 = start - s1;
//	e0 = end - e1;
////	printf("s0: %u\ne0: %u\n", s0,e0);
//
//
//	_range(s0, e0, lowvoc, uppvoc, vocmin, vocmidL, level+1, res);
//
//	_range(s1 + Z[level], e1 + Z[level], lowvoc, uppvoc, vocmidR, vocmax, level+1, res);
//}


void WaveletKdMatrix::axis(size_t start, size_t end, uint axis, uint node, vector< pair<uint,size_t> > &res) {
	usym n = {0, 0};
	if(axis == 0) {
		n.x = node;
	}
	else {
		n.y = node;
	}

	LOG("neighbors of {%u, %u}", n.x, n.y);
	_axis(start, end, axis, n, 0, 0, res);
}

void WaveletKdMatrix::_axis(size_t start, size_t end, uint axis, usym node, uint level, uint value, vector< pair<uint,size_t> > &res) {
	LOG("level: %u", level);
	if (start >= end) {
//		printf("start >= end\n");
		return;
	}

	if (level == height) {
		INFO("nodo hoja\n");
		res.push_back(make_pair(value,end-start));
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
			_axis(s1 + Z[level], e1 + Z[level], axis, node, level+1, value, res);
		}
		else {
		//	printf("0");
			//bajar nodo izq
			_axis(s0, e0, axis, node, level+1, value, res);
		}
	}
	else { // bajar por ambos!
		//printf("X");
		_axis(s0, e0, axis, node, level+1, value, res);

		value = value | (1 << (height/2-level/2-1));
		_axis(s1 + Z[level], e1 + Z[level], axis, node, level+1, value, res);
	}
	
	return;
}




uint WaveletKdMatrix::max_value(usym *symbols, size_t n) {
	uint max_v = 0;
	uint *p = (uint *) symbols;

	for(size_t i=0;i<2*n;i++)
		max_v = max(*p++, max_v);
	return max_v;
}

uint WaveletKdMatrix::bits(uint val) {
	uint ret = 0;
	while(val!=0) {
		ret++;
		val >>= 1;
	}
	return ret;
}

bool WaveletKdMatrix::is_set(usym val, uint ind) const
{
	assert(ind<height);

	uint l = height/2 - ind/2 - 1;

	if ( ind%2 == 0) {
		//if ind es impar, queremos obtener la coordenada x
		return (val.x >> l) & 1U;
	} else {
		//if ind es par, queremos la coordenada y
		return (val.y >> l) & 1U;
	}


}


void WaveletKdMatrix::save(ofstream &f) {
	cds_utils::saveValue<WaveletKdMatrix>(f, this, 1);

	saveValue<size_t>(f, Z, height);

	uint i;
	for(i = 0; i < height; i++)
		bitmap[i]->save(f);
}

WaveletKdMatrix * WaveletKdMatrix::load(ifstream & f) {

	WaveletKdMatrix * ret;
	ret = cds_utils::loadValue<WaveletKdMatrix>(f, 1);

	ret->Z = loadValue<size_t>(f, ret->height);
	ret->bitmap = new BitSequence*[ret->height];

	for(uint i = 0; i < ret->height; i++) {
		ret->bitmap[i] = BitSequence::load(f);
	}

    return ret;
}
