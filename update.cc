/*
 * This code was originally made by Guillermo de Bernardo <guillermo.debernardo@udc.es>
 *
 * It's evaluate the performance of the datastructure.
 *
 * Has been modified by Diego Caro <diegocaro@gmail.com>
 */
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "debug.h"

#include "TemporalGraph.h"

int main(int argc, char ** argv) {

	char *fileName;
	char *fileOut;

	ifstream f;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s <inputfile> <outputfile> 'flags'\n",
				argv[0]);
		exit(1);
	}

	fileName = argv[1];
	fileOut = argv[2];

	f.open(fileName, ios::binary);

	if (!f.is_open()) {
		fprintf(stderr, "Error, data structure '%s' not found.\n", fileName);
		exit(1);
	}

	TemporalGraph *index;
	index = TemporalGraph::load(f);
	f.close();

	struct opts opts;
	printf("flags: %s\n", argv[3]);
	const char *def_params = "T:RG5,B:RG5,C:RG5";
	readflags(&opts, def_params);
	readflags(&opts, argv[3]);

	opts.bs = getBSBuilder(opts.params["T"]);
	opts.bb = getBSBuilder(opts.params["B"]);
	opts.bc = getBSBuilder(opts.params["C"]);

	if (opts.bs == NULL || opts.bb == NULL || opts.bc == NULL) {
		fprintf(stderr, "Error, BitmapBuilder was not present.");
		return 1;
	}

	index->updateBitmaps(opts.bs, opts.bb, opts.bc);

	ofstream of;
	of.open(fileOut, ios::binary);

	index->save(of);
	of.close();

	exit(0);
}
