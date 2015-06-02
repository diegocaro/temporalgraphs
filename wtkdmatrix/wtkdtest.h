#ifndef WT_TEST_
#define WT_TEST_

#include <sys/types.h>
#include <map>
#include <vector>

using namespace std;

template <typename T>
class WTKDTest {
	
public:
	WTKDTest (T* sequence, size_t _n) {seq=sequence; n=_n;}
	T access(size_t pos) {return seq[pos];}

	size_t rank(T symbol, size_t pos) {
		uint c=0;
		for(uint i=0; i <= pos; i++) {
			if (seq[i]==symbol) c++;
		}
		return c;
	}
	
	size_t select(T symbol, size_t pos) {
		uint c=0;
		for(uint i=0; i < n; i++) {
			if (seq[i]==symbol) c++;
			if (c == pos) return i;
		}
		return n;
	}
	
	void range(size_t start, size_t end, T lowvoc, T uppvoc, vector<T> &res) {
		map<T, size_t> t;
		for(size_t i=start; i < end; i++) {
			if( seq[i] >= lowvoc && seq[i] < uppvoc) {
				t[seq[i]] += 1;
				
			}
		}
		
		typename map<T,size_t>::iterator it;
		for(it = t.begin(); it!=t.end(); ++it) {
			//printf("%u -> %u\n",it->first, it->second);
			res.push_back(it->first);
			res.push_back(it->second);
		}
	}
    
    
private:
	T* seq;
	size_t n;
    
};

#endif
