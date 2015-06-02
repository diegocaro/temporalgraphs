
/*
 * pepe-mx2d.cpp
 *
 *  Created on: Jan 07, 2015
 *      Author: diegocaro
 *
 * Compile with (in os x): 
 *      c++ prb2d.cc PRBCompactQtree.cc utils.cc -o prb2d -std=c++11 -stdlib=libc++ -O3 \
 *     	-I ../../cdswrapper/include/ -I ../../../sdsl-lite/include/ -I ../../../libcds/include/ \
 *		-I ../../../compresslists/ ../../../libcds/libcds.a ../../../sdsl-lite/lib/libsdsl.a 
 *
 */



#include "utils.h"
#include "PRBCompactQtree.h"

using namespace cqtree_static;
using namespace cqtree_utils;
using namespace cds_static;
using namespace std;


int main(int argc, char *argv[]) {
    vector<Point<uint> > vp;
    Point<uint> c(2);

	if (argc < 2){
		fprintf(stderr, "Usage: %s <outputfile> [no|hc|fc] [RRR|RG]\n",argv[0]);
		return -1;
	}

	char *ncconf="no"; //levels using compression nodes

	
	char *bitmap = "RG";
	if (argc == 4) {
		bitmap = argv[3];
	}
	
	BitSequenceBuilder *rg;
	if (strcmp(bitmap,"RRR") == 0) {
		fprintf(stderr,"Using RRR bitmaps\n");
		rg = new BitSequenceBuilderRRR(32); //default samplerate=32
	}
	else if (strcmp(bitmap,"RG") == 0) {
		fprintf(stderr, "Using RG bitmaps\n");
		rg = new BitSequenceBuilderRG(20); //by default
	}
	else {
		fprintf(stderr,"Bitmap should be RG or RRR\n");
		return -1;
	}



	


    ofstream f;
    f.open(argv[1],ios::binary);
	if (!f.good()) {
		fprintf(stderr, "Error opening '%s'\n",argv[1]);
		return -1;
	}	
    
	int nodes,edges;
	scanf("%u %u",&nodes,&edges);
	
	int li = 0;
	if (argc <= 4) {
		ncconf = argv[2];
		if (strcmp(ncconf,"no") == 0) {
			fprintf(stderr,"without node compression\n");
		}
		else if (strcmp(ncconf,"hc") == 0) {
			li = ceil(log2(nodes));
			fprintf(stderr,"%d levels with node compression (half)\n");	
		}
		else if (strcmp(ncconf,"fc") == 0) {
			li = ceil(log2(nodes))*2;
			fprintf(stderr,"%d levels with node compression (full)\n",li);
		}
		else {
			fprintf(stderr,"Options for node compression: nc|hc|fc\n");
			return -1;
		}
	}
	
	
	
	
	
	int edges_read = 0;
    while(EOF != scanf("%u %u", &c[0], &c[1])) {
		if (edges_read % 100000 == 1) fprintf(stderr, "Reading %.1f%%\r",edges_read*100.0/edges); 

	assert(c[0] < nodes && c[1] < nodes);
        vp.push_back(c);
		edges_read ++;
    }

	assert(edges_read == edges);

//    BitSequenceBuilderRG rg(20);
//    BitSequenceBuilderRRR rg(32); //default samplerate=32
	PRBCompactQtree *a;
	// points, T bitmap, B bitmap, k1, k2, levels for k1, levels for k-interleaved
	a = new PRBCompactQtree(vp, rg, rg, 2,2,0,li);


        //First check points...
        vector<Point<uint> > vpall;
        a->all(vpall);

        if (vp.size() != vpall.size()) {
          fprintf(stderr, "Error: data from data structure doesnt match the input \n");
          fprintf(stderr, "Expected size: %lu\nActual size: %lu\n",vp.size(), vpall.size());
          abort();
        }
        for(size_t i=0; i < vp.size(); i++) {
          if (i%1000==0)fprintf(stderr, "Checking data: %.2f%% \r", (float)i/vp.size()*100);
          if (vp[i] != vpall[i]) {
              fprintf(stderr,"Construction failed\n");
              abort();
          }
        }


    
    a->save(f);
	
    f.close();


	return 0;
}
