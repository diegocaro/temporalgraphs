/*
 * This code was originally made by Guillermo de Bernardo <guillermo.debernardo@udc.es>
 *
 * It's evaluate the performance of the datastructure.
 *
 * Has been modified by Diego Caro <diegocaro@gmail.com>
 */
#include <iostream>
#include <cstdio>
#include <time.h>
#include <stdlib.h>

#include "timing.h"
#include "debug.h"
#include "arraysort.h"

#include <point.h>
#include <CompactQtree.h>

#include "TemporalGraph.h"
#include <utils.h>
using namespace std;
using namespace cqtree_static;
using namespace cqtree_utils;
#define BUFFER 1024*1024*10

/*
 FIXME: Non soporta un numero de nodos maior que MAX_INT.

 As consultas son nodos aleatorios (non  unha permutacin,
 pode haber repetidos)
 */

#define EDGE 6
#define EDGE_WEAK 7
#define EDGE_STRONG 8

#define SNAPSHOT 9

#define EDGE_NEXT 10
#define CONTACTS 11

#define DIRECT_NEIGHBORS 0
#define REVERSE_NEIGHBORS 1
#define DIRECT_NEIGHBORS_WEAK 2
#define DIRECT_NEIGHBORS_STRONG 3
#define REVERSE_NEIGHBORS_WEAK 4
#define REVERSE_NEIGHBORS_STRONG 5

#define ISIZE 5

int main(int argc, char ** argv) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <graphfile> <querytype> <timepoint>\n",
				argv[0]);
		fprintf(stderr, "typequery=0 for direct neighbors\n");
		fprintf(stderr, "typequery=1 for reverse neighbors\n");
		exit(1);
	}

	char *fileName;
	ifstream f;
	fileName = argv[1];
	f.open(fileName, ios::binary);

	if (!f.is_open()) {
		fprintf(stderr, "Error, data structure '%s' not found.\n", fileName);
		exit(1);
	}

	int typequery = atoi(argv[2]);
	unsigned querytime = atoi(argv[3]);

	char namequery[255];
	if (typequery == DIRECT_NEIGHBORS) {
		sprintf(namequery, "dirnei-%u", querytime);
	} else if (typequery == REVERSE_NEIGHBORS) {
		sprintf(namequery, "revnei-%u", querytime);
	} else {
		fprintf(stderr, "Invalid query\n");
		exit(1);
	}

	TemporalGraph *index;
	index = TemporalGraph::load(f);
	f.close();

	unsigned int * gotreslist;
	gotreslist = (uint*) malloc(sizeof(unsigned int) * BUFFER);

	unsigned totalres = 0;
	unsigned gotres = 0;

	unsigned vertices = index->getNodes();

#ifndef TIMESAMPLE
	startClockTime();
#endif

	if (typequery == DIRECT_NEIGHBORS) {
		for (unsigned i = 0; i < vertices; i++) {
			*gotreslist = 0;
			if ((i & 0x8000) == 0x8000)
				fprintf(stderr, "Progress: %.2f\r",
						(float) i / vertices * 100.0);
#ifdef TIMESAMPLE
			startClockTime();
#endif

			index->direct_point(i, querytime, gotreslist);

#ifdef TIMESAMPLE
			printf("%s\t%s\t%.3lf\t%u\n", fileName, namequery, 1.0*endClockTime()/1000, *gotreslist);
#endif				 
			totalres += *gotreslist;
		}
	}

	else if (typequery == REVERSE_NEIGHBORS) {
		for (unsigned i = 0; i < vertices; i++) {
			*gotreslist = 0;
			if ((i & 0x8000) == 0x8000)
				fprintf(stderr, "Progress: %.2f\r",
						(float) i / vertices * 100.0);
#ifdef TIMESAMPLE
			startClockTime();
#endif

			index->reverse_point(i, querytime, gotreslist);

#ifdef TIMESAMPLE
			printf("%s\t%s\t%.3lf\t%u\n", fileName, namequery, 1.0*endClockTime()/1000, *gotreslist);
#endif				 
			totalres += *gotreslist;
		}
	}

#ifndef TIMESAMPLE

	unsigned long microsecs = endClockTime() / 1000; //to microsecs

	// datasets.structura namequery microsecs totaloutput timeperquery timeperoutput
	printf("%s\t%s\t%ld\t%d\t%d\t%lf\t%lf\n", fileName, namequery, microsecs,
			vertices, totalres, (double) microsecs / vertices,
			(double) microsecs / totalres);

	//destroyK2Tree(tree);
#endif

	exit(0);
}
