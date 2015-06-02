/*
 * This code was originally made by Guillermo de Bernardo <guillermo.debernardo@udc.es>
 *
 * Evaluate the performance of the datastructure.
 *
 * Has been modified by Diego Caro <diegocaro@gmail.com>
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "timing.h"
#include "tgs.h"
#include "debug.h"

#include "arraysort.h"

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

#define DIRECT_NEIGHBORS 0
#define REVERSE_NEIGHBORS 1
#define DIRECT_NEIGHBORS_WEAK 2
#define DIRECT_NEIGHBORS_STRONG 3
#define REVERSE_NEIGHBORS_WEAK 4
#define REVERSE_NEIGHBORS_STRONG 5

typedef struct squery {
        unsigned int row;
        unsigned int column;
        unsigned int xvalue; //x axis value (number of contacts or number of nodes)
} TimeQuery;

int typeQuery=0;
int getQuery(FILE *queryFile, TimeQuery *query) {

                int res = EOF;
                
				res = fscanf(queryFile, "%d", &query->xvalue);
                if (res == EOF) return res;
				
                switch(typeQuery) {
                        case EDGE: case EDGE_NEXT: case EDGE_WEAK: case EDGE_STRONG: {
                                res = fscanf(queryFile, "%d %d\n", &query->row, &query->column);
                                break;
                        }
                        case DIRECT_NEIGHBORS: case REVERSE_NEIGHBORS: 
                        case DIRECT_NEIGHBORS_WEAK: case DIRECT_NEIGHBORS_STRONG:
                        case REVERSE_NEIGHBORS_WEAK: case REVERSE_NEIGHBORS_STRONG: {
                                res = fscanf(queryFile, "%d\n", &query->row);
                                break;
                        }
                        case SNAPSHOT: {
							// None to do here
                                break;
                        }
                }
			

        return res;
}

int main(int argc, char ** argv) {

        char * fileNameIndex;
		char * fileNameQuery;
        struct tgs index;

		
		int timePoint;
		
		int timeIntervalStart;
		int timeIntervalEnd;

        ifstream f;
        unsigned int * gotreslist;
        uint gotres = 0;

        if (argc < 5) {
                fprintf(stderr, "Usage: %s <graphfile> <queryfile> <typequery> (<time> | <initime> <endtime>)\n", argv[0]);
				fprintf(stderr, "Queryname and its code: \n"
"EDGE 6 \n"
"EDGE_WEAK 7  \n"
"EDGE_STRONG 8  \n"
"EDGE_NEXT 10  \n"

"DIRECT_NEIGHBORS 0  \n"
"REVERSE_NEIGHBORS 1  \n"
"DIRECT_NEIGHBORS_WEAK 2  \n"
"DIRECT_NEIGHBORS_STRONG 3  \n"
"REVERSE_NEIGHBORS_WEAK 4  \n"
"REVERSE_NEIGHBORS_STRONG 5  \n"
"SNAPSHOT 9 \n");
                exit(1);
        }

        fileNameIndex = argv[1];
		fileNameQuery = argv[2];
		
		typeQuery = atoi(argv[3]);
		
		switch(typeQuery) {
            case EDGE: case EDGE_NEXT: case DIRECT_NEIGHBORS: case REVERSE_NEIGHBORS: case SNAPSHOT: {
				timePoint = atoi(argv[4]);
                break;
            }
            case EDGE_WEAK: case EDGE_STRONG:
            case DIRECT_NEIGHBORS_WEAK: case DIRECT_NEIGHBORS_STRONG:
            case REVERSE_NEIGHBORS_WEAK: case REVERSE_NEIGHBORS_STRONG: {
				timeIntervalStart = atoi(argv[4]);
				timeIntervalEnd = atoi(argv[5]);
                break;
            }
		}
		
		
		
        f.open(fileNameIndex, ios::binary);
        tgs_load(&index, f);
        f.close();

        gotreslist = (uint*)malloc(sizeof(unsigned int)*BUFFER);




		FILE * queryFile = fopen(fileNameQuery, "r");
		
		TimeQuery query;
        while(EOF != getQuery(queryFile, &query) ) {

		        startClockTime();



           switch(typeQuery) {
               case EDGE: {
                       //gotres = index->edge_point(query.row, query.column, timePoint);
                       //gotres = findEdge(tree, query.row, query.column, timePoint);
		       gotres = get_edge_point(&index, query.row, query.column, timePoint);
			   *gotreslist = gotres;
                       break;
               }
               case EDGE_NEXT: {
                       //gotres = findEdgeInterval(tree, query.row, query.column, timeIntervalStart, timeIntervalEnd, 1);
                       //gotres = index->edge_next(query.row, query.column, timePoint);
		       gotres = get_edge_next(&index, query.row, query.column, timePoint);
			   *gotreslist = gotres;
                       break;
               }
               case EDGE_WEAK: {
                       //gotres = findEdgeInterval(tree, query.row, query.column, timeIntervalStart, timeIntervalEnd, 0);
                       //gotres = index->edge_weak(query.row, query.column, timeIntervalStart, timeIntervalEnd);
		       gotres = get_edge_weak(&index, query.row, query.column,timeIntervalStart, timeIntervalEnd);
			   *gotreslist = gotres;
                       break;
               }
               case EDGE_STRONG: {
                       //gotres = findEdgeInterval(tree, query.row, query.column, timeIntervalStart, timeIntervalEnd, 1);
                       //gotres = index->edge_strong(query.row, query.column, timeIntervalStart, timeIntervalEnd);
		       gotres = get_edge_strong(&index, query.row, query.column,timeIntervalStart, timeIntervalEnd);
			   *gotreslist = gotres;
                       break;
               }
                case DIRECT_NEIGHBORS: {
                        get_neighbors_point(gotreslist, &index, query.row, timePoint);
			//index->direct_point(query.row, timePoint, gotreslist);

                        break;
                }
                case REVERSE_NEIGHBORS: {
			get_reverse_point(gotreslist, &index, query.row, timePoint);
			//index->reverse_point(query.row, timePoint, gotreslist);
                        break;
                }
                case DIRECT_NEIGHBORS_WEAK: {
                        get_neighbors_weak(gotreslist, &index, query.row, timeIntervalStart, timeIntervalEnd);
			//index->direct_weak(query.row, timeIntervalStart, timeIntervalEnd, gotreslist);
                        break;
                }
                case REVERSE_NEIGHBORS_WEAK: {
                        get_reverse_weak(gotreslist, &index, query.row, timeIntervalStart, timeIntervalEnd);
			//index->reverse_weak(query.row, timeIntervalStart, timeIntervalEnd, gotreslist);
                        break;
                }
                case DIRECT_NEIGHBORS_STRONG: {
                        get_neighbors_strong(gotreslist, &index, query.row, timeIntervalStart, timeIntervalEnd);
			//index->direct_strong(query.row, timeIntervalStart, timeIntervalEnd, gotreslist);
                        break;
                }
                case REVERSE_NEIGHBORS_STRONG: {
			get_reverse_strong(gotreslist, &index, query.row, timeIntervalStart, timeIntervalEnd);
			//index->reverse_strong(query.row, timeIntervalStart, timeIntervalEnd, gotreslist);
                        break;
                }
		
		case SNAPSHOT: {
			//gotres = index->snapshot(timePoint);
			gotres = get_snapshot(&index, timePoint);
			*gotreslist = gotres;
//                      gotres = findRange(tree, 0, tree->nNodesReal, 0, tree->nNodesReal, time)[0][0];
			break;
		}
                }

 				switch(typeQuery) {
		            case EDGE: case EDGE_NEXT: case DIRECT_NEIGHBORS: case REVERSE_NEIGHBORS: case SNAPSHOT: {
						printf("%s\t%u\t%ld\t%u\t%u\t%u\n", fileNameIndex, query.xvalue, endClockTime(), *gotreslist, typeQuery, timePoint);
		                break;
		            }
		            case EDGE_WEAK: case EDGE_STRONG:
		            case DIRECT_NEIGHBORS_WEAK: case DIRECT_NEIGHBORS_STRONG:
		            case REVERSE_NEIGHBORS_WEAK: case REVERSE_NEIGHBORS_STRONG: {
						printf("%s\t%u\t%ld\t%u\t%u\t%u\t%u\n", fileNameIndex, query.xvalue, endClockTime(), *gotreslist, typeQuery, timeIntervalStart, timeIntervalEnd);
						break;
		            }
				}
		
				
        }
        

        return 0;
}
