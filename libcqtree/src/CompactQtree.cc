/*
 * CompactQtree.cc
 *
 *  Created on: Aug 20, 2014
 *      Author: diegocaro
 */

#include "CompactQtree.h"

namespace cqtree_static {

CompactQtree * CompactQtree::load(ifstream & fp) {
	uint r = loadValue<uint>(fp);
	size_t pos = fp.tellg();
	fp.seekg(pos - sizeof(uint));
	switch (r) {
	case MXQDPT_SAV:
		return new MXCompactQtree(fp);
		//case MXQFIX_SAV: return new MXCompactQtreeFixed(fp);
	case PRQBLK_SAV:
		return new PRBCompactQtree(fp);
	case PRQBLK2_SAV:
		return new PRB2CompactQtree(fp);
		//case PRQBLK2XOR_SAV: return new PRB2XORCompactQtree(fp);
		//case PRQWHT_SAV: return new PRWCompactQtree(fp);
	}
	return NULL;
}

}
