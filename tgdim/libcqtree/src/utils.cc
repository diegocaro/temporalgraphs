#include "utils.h"

unsigned cqtree_utils::mypow(unsigned x, int y) {
	assert(y < 32);
	unsigned r = 1;
	for (int i = 0; i < y; i++) {
		r *= x;
	}
	return r;
}

unsigned cqtree_utils::mylog(unsigned b, unsigned x) {
	return ceil(log(x) / log(b));
}
