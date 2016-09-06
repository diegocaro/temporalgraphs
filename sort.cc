//
// Maximo numero de vertexes: 2^32-1
// Maximo numero de timepoints 2^32-1
//

#include <cstdio>
#include <vector>
#include <algorithm>
#include <point.h>

#include <cassert>

#include "debug.h"

using namespace std;
using namespace cqtree_utils;

typedef unsigned uint;

int main(int argc, char *argv[]) {
	uint nodes, edges, lifetime, contacts;

	FILE *infile = stdin;

	// Reading input
	INFO("Reading input...");
	fscanf(infile, "%u %u %u %u", &nodes, &edges, &lifetime, &contacts);
	LOG("nodes: %u", nodes);
	LOG("edges: %u", edges);
	LOG("maxtime: %u", maxtime);
	LOG("contacts: %u", contacts);

	size_t readcontacts = 0;
	vector<Point<uint> > vp;
	vp.reserve(contacts);

	Point<uint> c(4);
	while (EOF != fscanf(infile, "%u %u %u %u", &c[0], &c[1], &c[2], &c[3])) {
		readcontacts++;
		if (readcontacts % 10000 == 0)
			fprintf(stderr, "\rReading data: %.2f%%",
					(float) readcontacts / contacts * 100);

		assert(c[0] < nodes);
		assert(c[1] < nodes);
		assert(c[2] < lifetime);
		assert(c[3] < lifetime);

		vp.push_back(c);
	}
	fclose(infile);
	assert(readcontacts == contacts);

	fprintf(stderr, "\nChecking if sorted\n");
	bool is_sorted = true;
	for (size_t i = 1; i < vp.size(); i++) {
		if (Point<uint>::cmpmorton(vp[i - 1], vp[i]) == false) {
			is_sorted = false;
			break;
		}
	}

	if (!is_sorted) {
		fprintf(stderr, "Sorting\n");
		sort(vp.begin(), vp.end(), Point<uint>::cmpmorton);
	}

	fprintf(stderr, "Deleting duplicated entries\n");

	fprintf(stderr, "Old size: %lu\n", vp.size());
	typename vector<Point<uint>>::iterator last = unique(vp.begin(), vp.end());
	vp.erase(last, vp.end());
	fprintf(stderr, "Actual size: %lu\n", vp.size());

	contacts = vp.size();

	fprintf(stderr, "Printing data\n");

	printf("%u %u %u %u\n", nodes, edges, lifetime, contacts);
	for (vector<Point<uint> >::iterator it = vp.begin(); it != vp.end(); ++it) {
		printf("%u %u %u %u\n", (*it)[0], (*it)[1], (*it)[2], (*it)[3]);
	}

	fprintf(stderr, "\n");
	return 0;

}
