#include <cstdio>

#include <point.h>
#include <CompactQtree.h>

#include "TemporalGraph.h"

int main(int argc, char ** argv) {
	char * fileName;
	ifstream f;

	if (argc < 2) {
		printf("Usage: %s <graphfile>\n", argv[0]);
		exit(1);
	}

	fileName = argv[1];

	f.open(fileName, ios::binary);

	if (!f.is_open()) {
		fprintf(stderr, "Error, data structure '%s' not found.\n", fileName);
		exit(1);
	}

	TemporalGraph *index;

	index = TemporalGraph::load(f);

	f.close();
	// level c_1 c_2 c_3 c_4
	index->print_leaves();

	return 0;
}
