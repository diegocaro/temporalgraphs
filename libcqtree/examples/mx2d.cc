
/*
 * pepe-mx2d.cpp
 *
 *  Created on: Jan 07, 2015
 *      Author: diegocaro
 *
 * Compile with (in os x): 
 *      c++ pepe-mx2d.cc MXCompactQtree.cc utils.cc -o pepe-mx2d -std=c++11 -stdlib=libc++ -O3 \
 *     	-I ../../cdswrapper/include/ -I ../../../sdsl-lite/include/ -I ../../../libcds/include/ \
 *		../../../libcds/libcds.a ../../../sdsl-lite/lib/libsdsl.a 
 *
 */



#include "utils.h"
#include "MXCompactQtree.h"

using namespace cqtree_static;
using namespace cqtree_utils;
using namespace cds_static;
using namespace std;

class k2tree : public MXCompactQtree {
public:
	k2tree(ifstream &f) {
		depth_ = cds_utils::loadValue<int>(f);

        for(int i = 0; i < depth_; i++) {
            T_.push_back(BitSequence::load(f));
        }
		
	}

	void save(ofstream &f) {
		cds_utils::saveValue(f,depth_);
		
        for(int i = 0; i < depth_; i++) {
            T_[i]->save(f);
        }
	}
	
	//private:
	//int depth_;
	//vector<BitSequence*> T_;
};


int main(int argc, char *argv[]) {
    vector<Point<uint> > vp;
    Point<uint> c(2);

	if (argc < 2){
		fprintf(stderr, "Usage: %s <outputfile>\n",argv[0]);
		return -1;
	}

    ofstream f;
    f.open(argv[1],ios::binary);
	if (!f.good()) {
		fprintf(stderr, "Error opening '%s'\n",argv[1]);
		return -1;
	}	
    
	
    while(EOF != scanf("%u %u", &c[0], &c[1])) {
        vp.push_back(c);
    }

    BitSequenceBuilderRG rg(20);
    MXCompactQtree *a;
	a = new MXCompactQtree(vp, &rg, 2,2,0,0);
    
    ((k2tree *)a )->save(f);
	
    f.close();


	return 0;
}
