#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

#include "tgs.h"
#include "timing.h"
#include "debug.h"


#define BUFFER 1024*1024*10

#define DIRECT_NEIGHBORS 0
#define REVERSE_NEIGHBORS 1
#define DIRECT_NEIGHBORS_WEAK 2
#define DIRECT_NEIGHBORS_STRONG 3
#define REVERSE_NEIGHBORS_WEAK 4
#define REVERSE_NEIGHBORS_STRONG 5

int main(int argc, char ** argv) {

	char * fileNameIndex;
	int typeQuery;
	int timePoint;

	int timeIntervalStart;
	int timeIntervalEnd;

	ifstream f;
	unsigned int * gotreslist;

	if (argc < 4) {
		fprintf(stderr,
				"Usage: %s <graphfile> <typequery> (<time> | <initime> <endtime>)\n",
				argv[0]);
		fprintf(stderr, "Queryname and its code: \n"
				"DIRECT_NEIGHBORS 0  \n"
				"REVERSE_NEIGHBORS 1  \n"
				"DIRECT_NEIGHBORS_WEAK 2  \n"
				"DIRECT_NEIGHBORS_STRONG 3  \n"
				"REVERSE_NEIGHBORS_WEAK 4  \n"
				"REVERSE_NEIGHBORS_STRONG 5  \n");
		exit(1);
	}

	fileNameIndex = argv[1];

	typeQuery = atoi(argv[2]);

	switch (typeQuery) {
	case DIRECT_NEIGHBORS:
	case REVERSE_NEIGHBORS: {
		timePoint = atoi(argv[3]);
		break;
	}
	case DIRECT_NEIGHBORS_WEAK:
	case DIRECT_NEIGHBORS_STRONG:
	case REVERSE_NEIGHBORS_WEAK:
	case REVERSE_NEIGHBORS_STRONG: {
		timeIntervalStart = atoi(argv[3]);
		timeIntervalEnd = atoi(argv[4]);
		break;
	}
	}

	// Opening files
	f.open(fileNameIndex, ios::binary);

	if (!f.is_open()) {
		fprintf(stderr, "Error, data structure '%s' not found.\n",
				fileNameIndex);
		exit(1);
	}

	struct tgs index;
	tgs_load(&index, f);
	f.close();

	gotreslist = (uint*) malloc(sizeof(unsigned int) * BUFFER);

	unsigned vertices = index.nodes;
    
	// For testing, we profer to query a random permutation of vertices
	vector<unsigned> vrand(vertices,0);
	for(unsigned i=0; i < vertices; i++) {
		vrand[i] = i;
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (vrand.begin(), vrand.end(), std::default_random_engine(seed));
	
	unsigned v;
	for (auto it = vrand.begin(); it != vrand.end(); ++it) {
		if (v % 1000 == 0)
			fprintf(stderr, "Progress: %.2f%%\r",
					(float) v / vertices * 100.0);

		*gotreslist = 0;
		
		v = *it;
		startClockTime();

		switch (typeQuery) {

		case DIRECT_NEIGHBORS: {
			//index->direct_point(v, timePoint, gotreslist);
			get_neighbors_point(gotreslist, &index, v, timePoint);
			break;
		}
		case REVERSE_NEIGHBORS: {
			//index->reverse_point(v, timePoint, gotreslist);
			get_reverse_point(gotreslist, &index, v, timePoint);
			break;
		}
		case DIRECT_NEIGHBORS_WEAK: {
			//index->direct_weak(v, timeIntervalStart, timeIntervalEnd, gotreslist);
			get_neighbors_weak(gotreslist, &index, v, timeIntervalStart, timeIntervalEnd);
			break;
		}
		case REVERSE_NEIGHBORS_WEAK: {
			//index->reverse_weak(v, timeIntervalStart, timeIntervalEnd,
			//		gotreslist);
			 get_reverse_weak(gotreslist, &index, v, timeIntervalStart, timeIntervalEnd);
			break;
		}
		case DIRECT_NEIGHBORS_STRONG: {
			//index->direct_strong(v, timeIntervalStart, timeIntervalEnd,
			//		gotreslist);
			get_neighbors_strong(gotreslist, &index, v, timeIntervalStart, timeIntervalEnd);
			break;
		}
		case REVERSE_NEIGHBORS_STRONG: {
			//index->reverse_strong(v, timeIntervalStart, timeIntervalEnd,
			//		gotreslist);
			get_reverse_strong(gotreslist, &index, v, timeIntervalStart, timeIntervalEnd);
			break;
		}
		}
		
		// Time is expresed in micro secs
		switch (typeQuery) {
		case DIRECT_NEIGHBORS:
		case REVERSE_NEIGHBORS: {
			printf("%s\t%u\t%.3lf\t%u\t%u\t%u\n", fileNameIndex, v,
					1.0*endClockTime()/1000, *gotreslist, typeQuery, timePoint);
			break;
		}
		case DIRECT_NEIGHBORS_WEAK:
		case DIRECT_NEIGHBORS_STRONG:
		case REVERSE_NEIGHBORS_WEAK:
		case REVERSE_NEIGHBORS_STRONG: {
			printf("%s\t%u\t%.3lf\t%u\t%u\t%u\t%u\n", fileNameIndex, v,
					1.0*endClockTime()/1000, *gotreslist, typeQuery, timeIntervalStart,
					timeIntervalEnd);
			break;
		}
		}

	}

	return 0;
}
