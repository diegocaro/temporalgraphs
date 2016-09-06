/*
 * use.c
 *
 *  Created on: Mar 20, 2013
 *      Author: diegocaro
 */

#include <cstdio>
#include "tgl.h"
#include "arraysort.h"

int main(int argc, char *argv[]) {
	TemporalGraphLog *index;
	ifstream f;
	uint *buffer;
	uint i, j;
	

	buffer = (uint *)malloc(sizeof(uint)*BUFFER);


	printf("Reading file: '%s'\n", argv[1]);

	f.open(argv[1], ios::binary);
	index = TemporalGraphLog::load(f);
	f.close();
  
  printf("Size of the structure: %lu\n", index->getSize());
  
/*
	uint a,b;
	printf("Write your query: ");
	while(scanf("%u %u", &a, &b)) {
		get_neighbors_point(buffer, &index, a, b);
		printf("t=%u -> ", 0);
		print_arraysort(buffer);
	}
	return 0;
*/
	vector<uint> bb;

	for ( i = 0; i < 5; i++) {
		//printf("start(%u): %u\n", i, start(index.map, i));
		printf("Direct of node %u\n", i);
		for( j = 0; j <= index->getMaxtime(); j++) {
			index->direct_point(i, j, buffer);
			printf("t=%u -> ", j);print_arraysort(buffer);
			bb.clear();
		}
		
	}


	for ( i = 0; i < 5; i++) {
		printf("direct weak %u\n", i);
		for( j = 0; j <= index->getMaxtime(); j++) {
			printf("time interval [0, %u)\n", j);
			index->direct_weak(i, 0, j, buffer);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}

	for ( i = 0; i < 5; i++) {
		printf("direct strong %u\n", i);
		for( j = 0; j <= index->getMaxtime(); j++) {
			printf("time interval [0, %u)\n", j);
			index->direct_strong(i, 0, j, buffer);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}
	
 
	for ( i = 0; i < 5; i++) {
		printf("Reverses of node %u\n", i);
		for( j = 0; j <= index->getMaxtime(); j++) {
			index->reverse_point(i, j, buffer);
			printf("t=%u <- ", j);print_arraysort(buffer);
			bb.clear();
		}
		
	}
	
	for ( i = 0; i < 5; i++) {
			printf("Reverses weak %u\n", i);
			for( j = 0; j <= index->getMaxtime(); j++) {
				printf("time interval [0, %u)\n", j);
				index->reverse_weak(i, 0, j, buffer);
				printf("t=%u <- ", j);print_arraysort(buffer);
			}
		}


	for ( i = 0; i < 5; i++) {
			printf("Reverses strong %u\n", i);
			for( j = 0; j <= index->getMaxtime(); j++) {
				printf("time interval [0, %u)\n", j);
				index->reverse_strong(i, 0, j, buffer);
				printf("t=%u <- ", j);print_arraysort(buffer);
			}
		}
	
	//free(buffer);
	return 0;
}

