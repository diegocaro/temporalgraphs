/*
 * CompactQtree.h
 *
 *  Created on: Apr 15, 2014
 *      Author: diegocaro
 */

#ifndef COMPACTQTREE_H_
#define COMPACTQTREE_H_

#include "utils.h"
#include "count_ops.h"


#include <vector>

// Libcds
#include <libcdsBasics.h>
#include <BitSequenceBuilder.h>

using namespace cds_static;
using namespace cqtree_utils;

//CODES to save structures
#define MXQDPT_SAV 2
#define MXQFIX_SAV 3
#define PRQBLK_SAV 4
#define PRQWHT_SAV 5
#define PRQBLK2_SAV 6
#define PRQBLK2XOR_SAV 7

namespace cqtree_static {

class CompactQtree {
public:
	CompactQtree() {};
	virtual ~CompactQtree() {};

	//implement on inherited classes
	virtual void save(ofstream & fp) const=0;

	static CompactQtree * load(ifstream & fp);

	virtual void all(vector<Point<uint> > &vpall) =0;

	virtual size_t range(Point<uint> &from, Point<uint> &to,vector<Point<uint> > &vpall, bool pushval=true)=0;
  
  virtual void stats_space() const=0;
};


} /* namespace cqtree_static */

#include "MXCompactQtree.h"
#include "PRBCompactQtree.h"
#include "PRB2CompactQtree.h"
#endif /* COMPACTQTREE_H_ */
