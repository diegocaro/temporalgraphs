/*
 * test.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: diegocaro
 */

#include <vector>
#include <iostream>
#include "Point.h"
//#include "CompactQtree.h"

//using namespace cqtree_static;
using namespace cqtree_utils;

//using namespace cds_static;

using namespace std;

int main() {
	vector<Point<int> > vp;

	Point<int> p(3);
	p[0] = 1;
	p[1] = 2;
	p[2] = 3;

	vp.push_back(p);
	p[0] = 2;
	p[1] = 4;
	p[2] = 6;

	vp.push_back(p);

	p[0] = 3;
	p[1] = 5;
	p[2] = 7;

	vp.push_back(p);

	//PRCompactQtreeBlack<Point3d> a(vp, new BitSequenceBuilderRG(20));

	vector<Point<int> > vp2;
	Point<int> q(4);
	q[0] = 1;
	q[1] = 2;
	q[2] = 3;
	q[3] = 4;
	vp2.push_back(q);
	q[0] = 2;
	q[1] = 4;
	q[2] = 6;
	q[3] = 8;
	vp2.push_back(q);

	q[0] = 3;
	q[1] = 5;
	q[2] = 7;
	q[3] = 9;
	vp2.push_back(q);
	vp2.push_back(p);
	/*
	 PRCompactQtreeBlack<Point4d> b(vp2, new BitSequenceBuilderRG(20));

	 ofstream fileout;
	 fileout.open("/tmp/test4d", ios::binary);
	 b.save(fileout);
	 fileout.close();

	 ifstream filein;
	 filein.open("/tmp/test4d", ios::binary);
	 CompactQtree<Point4d> *c = PRCompactQtreeBlack<Point4d>::load(filein);
	 filein.close();
	 */

	return 0;
}
