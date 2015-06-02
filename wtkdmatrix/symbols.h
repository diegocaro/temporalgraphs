/*
 * symbols.h
 *
 *  Created on: May 22, 2013
 *      Author: diegocaro
 */

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

struct symbols {
	uint x;
	uint y;
};

typedef struct symbols usym;

struct symbols_c {
	usym symbol;
	uint count;
};

typedef struct symbols_c usym_c;


bool operator==(const usym& lhs, const usym& rhs);

bool operator!=(const usym& lhs, const usym& rhs);


#endif /* SYMBOLS_H_ */
