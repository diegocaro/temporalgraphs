#ifndef WT_TEST_
#define WT_TEST_

#include <sys/types.h>
#include <map>
#include <vector>
#include <utility>
using namespace std;

template <typename T>
class WTest {
	
public:
	WTest (T* sequence, size_t _n) {seq=sequence; n=_n; sigma=0; for(size_t j=0; j<n;j++) if(seq[j]>sigma) sigma=seq[j];}
	T access(size_t pos) {return seq[pos];}

	size_t rank(T symbol, size_t pos) {
		size_t c=0;
		for(size_t i=0; i <= pos; i++) {
			if (seq[i]==symbol) c++;
		}
		return c;
	}
	
	size_t select(T symbol, size_t pos) {
		size_t c=0;
		for(size_t i=0; i < n; i++) {
			if (seq[i]==symbol) c++;
			if (c == pos) return i;
		}
		return n;
	}
	
	void select_all(T symbol, vector<size_t> &res) {
		for(size_t i=0; i < n; i++) {
			if( seq[i] == symbol) {
				res.push_back(i);
			}
		}
	}
	
	size_t next_value_pos(T symbol, size_t start, size_t end) {
		for(size_t j=symbol; j<sigma; j++)
		for(size_t i=start; i < end; i++) {
			if( seq[i] == j) {
				return i;
			}
		}
		return n;
	}
	
	
	void range_report(size_t start, size_t end, T lowvoc, T uppvoc, vector< pair<uint,size_t> > &res) {
		map<T, size_t> t;
		for(size_t i=start; i < end; i++) {
			if( seq[i] >= lowvoc && seq[i] < uppvoc) {
				t[seq[i]] += 1;
				
			}
		}
		
		typename map<T,size_t>::iterator it;
		for(it = t.begin(); it!=t.end(); ++it) {
			//printf("%u -> %u\n",it->first, it->second);
			res.push_back(make_pair(it->first,it->second));
		}
	}
    
    
private:
	T* seq;
	size_t n;
	uint sigma;
};

#endif
