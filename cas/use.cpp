/*
 * use.c
 *
 *  Created on: Mar 20, 2013
 *      Author: diegocaro
 */

#include <stdio.h>
#include "arraysort.h"
#include "tgs.h"

int main(int argc, char *argv[]) {
	struct tgs index;
	ifstream f;
	uint *buffer;
	uint i, j;
	

	buffer = (uint *)malloc(sizeof(uint)*BUFFER);


	printf("Reading file: '%s'\n", argv[1]);

	f.open(argv[1], ios::binary);
	tgs_load(&index, f);
	f.close();
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

	for ( i = 0; i < 5; i++) {
		printf("start(%u): %lu\n", i, start(index.map, i));
		for( j = 0; j <= index.maxtime; j++) {
			get_neighbors_point(buffer, &index, i, j);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}
 
	for ( i = 0; i < 5; i++) {
		printf("direct weak %u\n", i);
		for( j = 0; j <= index.maxtime; j++) {
			printf("time interval [0, %u)\n", j);
			get_neighbors_weak(buffer, &index, i, 0, j);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}

	for ( i = 0; i < 5; i++) {
		printf("direct strong %u\n", i);
		for( j = 0; j <= index.maxtime; j++) {
			printf("time interval [0, %u)\n", j);
			get_neighbors_strong(buffer, &index, i, 0, j);
			printf("t=%u -> ", j);print_arraysort(buffer);
		}
	}

 
	for ( i = 0; i < 5; i++) {
		printf("Reverses of node %u\n", i);
		for( j = 0; j <= 5; j++) {
			get_reverse_point(buffer, &index, i, j);
			printf("t=%u <- ", j);print_arraysort(buffer);
		}
	}
	/*
	for ( i = 0; i < 5; i++) {
		printf("Reverses of node %u (slow)\n", i);
		for( j = 0; j <= 5; j++) {
			get_reverse_point_slow(buffer, &index, i, j);
			printf("t=%u <- ", j);print_arraysort(buffer);
		}
	}*/

	for ( i = 0; i < 5; i++) {
			printf("Reverses weak %u\n", i);
			for( j = 0; j <= index.maxtime; j++) {
				printf("time interval [0, %u)\n", j);
				get_reverse_weak(buffer, &index, i, 0, j);
				printf("t=%u <- ", j);print_arraysort(buffer);
			}
		}


	for ( i = 0; i < 5; i++) {
			printf("Reverses strong %u\n", i);
			for( j = 0; j <= index.maxtime; j++) {
				printf("time interval [0, %u)\n", j);
				get_reverse_strong(buffer, &index, i, 0, j);
				printf("t=%u <- ", j);print_arraysort(buffer);
			}
		}

	free(buffer);
	return 0;
}

