/*
 * This code was originally made by Guillermo de Bernardo <guillermo.debernardo@udc.es>
 *
 * It's evaluate the performance of the datastructure.
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

#define CHANGE_POINT 11
#define CHANGE_INTERVAL 12
#define ACTIVED_POINT 13
#define ACTIVED_INTERVAL 14
#define DEACTIVED_POINT 15
#define DEACTIVED_INTERVAL 16

#define ISIZE 5

int compareRes(unsigned int * l1, unsigned int * l2) {
        uint i;
        if (l1[0]!=l2[0]) return 1;
        for (i = 1; i <= l1[0]; i++) {
                if (l1[i] != l2[i])  {
                        printf("got value %u at index %d, expected %u\n", l1[i], i, l2[i]);
                        return 1;
                }
        }
        return 0;
}

typedef struct squery {
        int type;
        unsigned int row;
        unsigned int column;
        unsigned int time; //point
        unsigned int initime; //interval
        unsigned int endtime; //interval
        unsigned int expectednres;
        unsigned int * expectedres;
} TimeQuery;

#define MAX_QUERIES 10000
TimeQuery * readQueries(char * filename, int * nqueries) {
        TimeQuery * ret = (TimeQuery *) malloc(MAX_QUERIES*sizeof(TimeQuery));
        FILE * queryFile = fopen(filename, "r");
        int curn = 0;
        while(1) {
                TimeQuery *query = &ret[curn];
                int res = EOF;
                res = fscanf(queryFile, "%d", &query->type);
                if (res == EOF) break;
                switch(query->type) {
                        case EDGE: case EDGE_NEXT: {
                                res = fscanf(queryFile, "%d %d %d\n", &query->row, &query->column, &query->time);
                                break;
                        }
                        case EDGE_WEAK: case EDGE_STRONG: {
                                res = fscanf(queryFile, "%d %d %d %d\n", &query->row, &query->column, &query->initime, &query->endtime);
                                break;
                        }
                        case DIRECT_NEIGHBORS: case REVERSE_NEIGHBORS: {
                                res = fscanf(queryFile, "%d %d\n", &query->row, &query->time);
                                break;
                        }
                        case DIRECT_NEIGHBORS_WEAK: case DIRECT_NEIGHBORS_STRONG:
                        case REVERSE_NEIGHBORS_WEAK: case REVERSE_NEIGHBORS_STRONG: {
                                res = fscanf(queryFile, "%d %d %d\n", &query->row, &query->initime, &query->endtime);
                                break;
                        }
                        case SNAPSHOT: case CHANGE_POINT: case ACTIVED_POINT: case DEACTIVED_POINT: {
                            res = fscanf(queryFile, "%d\n", &query->time);
                            break;
                    }

                    case CHANGE_INTERVAL: case ACTIVED_INTERVAL: case DEACTIVED_INTERVAL: {
                            res = fscanf(queryFile, "%d %d\n", &query->initime, &query->endtime);
                            break;
                    }
                }

                if(query->type  == EDGE || query->type  == EDGE_NEXT ||query->type == EDGE_STRONG || query->type == EDGE_WEAK || query->type == SNAPSHOT ||
                        query->type == CHANGE_POINT || query->type == CHANGE_INTERVAL || query->type == ACTIVED_POINT || query->type == ACTIVED_INTERVAL ||
                        query->type == DEACTIVED_POINT || query->type == DEACTIVED_INTERVAL)
                        res = fscanf(queryFile, "%d\n", &query->expectednres);
                else {
                        res = fscanf(queryFile, "%d", &query->expectednres);
                        uint i;
                        query->expectedres = (unsigned int *) malloc((1+query->expectednres)*sizeof(unsigned int));
                        query->expectedres[0] = query->expectednres;
                        for (i = 1; i <= query->expectednres; i++) {
                                fscanf(queryFile, "%u ", &query->expectedres[i]);
                        }
                }
                curn++;
        }
        *nqueries = curn;
        return ret;
}

void printQuery(TimeQuery q) {
        printf("%d %d %d %d %d %d %d\n", q.type, q.row, q.column, q.time, q.initime, q.endtime, q.expectednres);
}

#define CHECK_RESULTS 1
int savegotFile = 0;

int main(int argc, char ** argv) {

        char * fileName;
        struct tgs index;
        int totalres = 0;
        char * gotqueryFile = NULL;
        FILE * gotFile = NULL;
        ifstream f;
        unsigned int * gotreslist;
        uint gotres = 0;

        if (argc < 3) {
                printf("Usage: %s <graphfile> <queryfile> [<gotqueryfile>]\n", argv[0]);
                exit(1);
        }
        if (argc == 4) {
          savegotFile = 1;
          gotqueryFile = argv[3];
        }

        fileName = argv[1];

        f.open(fileName, ios::binary);
        tgs_load(&index, f);
        f.close();

        gotreslist = (uint*)malloc(sizeof(unsigned int)*BUFFER);

        int nqueries = 0;
        TimeQuery * queries = readQueries(argv[2], &nqueries);

        int i;

#ifndef EXPERIMENTS
	printf("We are checking the results... Experiments mode off.\n");
#endif

	startClockTime();
        for (i = 0; i < nqueries; i++) {
		fprintf(stderr, "Processing %d/%d\r", i, nqueries);
                TimeQuery query = queries[i];

		//cleaning vector of results

                switch(query.type) {
               case EDGE: {
                       //gotres = index->edge_point(query.row, query.column, query.time);
                       //gotres = findEdge(tree, query.row, query.column, query.time);
		       gotres = get_edge_point(&index, query.row, query.column, query.time);
		       *gotreslist = gotres;
                       break;
               }
               case EDGE_NEXT: {
                       //gotres = findEdgeInterval(tree, query.row, query.column, query.initime, query.endtime, 1);
                       //gotres = index->edge_next(query.row, query.column, query.time);
		       gotres = get_edge_next(&index, query.row, query.column, query.time);
		       *gotreslist = gotres;
                       break;
               }
               case EDGE_WEAK: {
                       //gotres = findEdgeInterval(tree, query.row, query.column, query.initime, query.endtime, 0);
                       //gotres = index->edge_weak(query.row, query.column, query.initime, query.endtime);
		       gotres = get_edge_weak(&index, query.row, query.column,query.initime, query.endtime);
		       *gotreslist = gotres;
                       break;
               }
               case EDGE_STRONG: {
                       //gotres = findEdgeInterval(tree, query.row, query.column, query.initime, query.endtime, 1);
                       //gotres = index->edge_strong(query.row, query.column, query.initime, query.endtime);
		       gotres = get_edge_strong(&index, query.row, query.column,query.initime, query.endtime);
		       *gotreslist = gotres;
                       break;
               }
                case DIRECT_NEIGHBORS: {
                        get_neighbors_point(gotreslist, &index, query.row, query.time);
			//index->direct_point(query.row, query.time, gotreslist);

                        break;
                }
                case REVERSE_NEIGHBORS: {
			get_reverse_point(gotreslist, &index, query.row, query.time);
			//index->reverse_point(query.row, query.time, gotreslist);
                        break;
                }
                case DIRECT_NEIGHBORS_WEAK: {
                        get_neighbors_weak(gotreslist, &index, query.row, query.initime, query.endtime);
			//index->direct_weak(query.row, query.initime, query.endtime, gotreslist);
                        break;
                }
                case REVERSE_NEIGHBORS_WEAK: {
                        get_reverse_weak(gotreslist, &index, query.row, query.initime, query.endtime);
			//index->reverse_weak(query.row, query.initime, query.endtime, gotreslist);
                        break;
                }
                case DIRECT_NEIGHBORS_STRONG: {
                        get_neighbors_strong(gotreslist, &index, query.row, query.initime, query.endtime);
			//index->direct_strong(query.row, query.initime, query.endtime, gotreslist);
                        break;
                }
                case REVERSE_NEIGHBORS_STRONG: {
			get_reverse_strong(gotreslist, &index, query.row, query.initime, query.endtime);
			//index->reverse_strong(query.row, query.initime, query.endtime, gotreslist);
                        break;
                }
		
		case SNAPSHOT: {
			//gotres = index->snapshot(query.time);
			gotres = get_snapshot(&index, query.time);
			*gotreslist = gotres;
//                      gotres = findRange(tree, 0, tree->nNodesReal, 0, tree->nNodesReal, time)[0][0];
			break;
		}



        case CHANGE_POINT: {
            gotres = get_change_point(&index, query.time);
            *gotreslist = gotres;
            break;
        }
        case CHANGE_INTERVAL: {
            gotres = get_change_interval(&index, query.initime, query.endtime);
            *gotreslist = gotres;
            break;
        }
        case ACTIVED_POINT: {
            gotres = get_actived_point(&index, query.time);
            *gotreslist = gotres;
            break;
        }
        case ACTIVED_INTERVAL: {
            gotres = get_actived_interval(&index, query.initime, query.endtime);
            *gotreslist = gotres;
            break;
        }
        case DEACTIVED_POINT: {
            gotres = get_deactived_point(&index, query.time);
            *gotreslist = gotres;
            break;
        }
        case DEACTIVED_INTERVAL: {
            gotres = get_deactived_interval(&index, query.initime, query.endtime);
            *gotreslist = gotres;
            break;
        }


                }

#ifndef EXPERIMENTS
//                //Comentar para medir tiempos:
                if (CHECK_RESULTS) {
                  if (savegotFile) {
                    gotFile = fopen(gotqueryFile, "a");
                    fprintf(gotFile, "%d ", query.type);
                    switch(query.type) {
                    case EDGE: case EDGE_NEXT: {
                      fprintf(gotFile, "%d %d %d\n", query.row, query.column, query.time);
                      break;
                    }
                    case EDGE_WEAK: case EDGE_STRONG: {
                      fprintf(gotFile, "%d %d %d %d\n", query.row, query.column, query.initime, query.endtime);
                      break;
                    }
                    case DIRECT_NEIGHBORS: case REVERSE_NEIGHBORS: {
                      fprintf(gotFile, "%d %d\n", query.row, query.time);
                      break;
                    }
                    case DIRECT_NEIGHBORS_WEAK: case DIRECT_NEIGHBORS_STRONG:
                    case REVERSE_NEIGHBORS_WEAK: case REVERSE_NEIGHBORS_STRONG: {
                      fprintf(gotFile, "%d %d %d\n", query.row, query.initime, query.endtime);
                      break;
                    }
		    case SNAPSHOT:case CHANGE_POINT: case ACTIVED_POINT: case DEACTIVED_POINT:
		      fprintf(gotFile, "%d\n", query.time);
		    break;
                    }

                    if (query.type == EDGE || query.type == EDGE_NEXT || query.type == EDGE_WEAK || query.type == EDGE_STRONG || query.type == SNAPSHOT ||
                            query.type == CHANGE_POINT || query.type == CHANGE_INTERVAL || query.type == ACTIVED_POINT || query.type == ACTIVED_INTERVAL ||
                            query.type == DEACTIVED_POINT || query.type == DEACTIVED_INTERVAL) {
                      fprintf(gotFile,"%d\n", gotres);
                    } else {
                      uint j;
                      fprintf(gotFile, "%d", gotreslist[0]);
                      for (j = 1; j <= gotreslist[0]; j++) {
                        fprintf(gotFile, " %d", gotreslist[j]);
                      }
                      fprintf(gotFile, "\n");
                    }

                    fclose(gotFile);
                  }


                  int failcompare = 0;
                  if (query.type == EDGE || query.type == EDGE_NEXT || query.type == EDGE_WEAK || query.type == EDGE_STRONG || query.type == SNAPSHOT||
                          query.type == CHANGE_POINT || query.type == CHANGE_INTERVAL || query.type == ACTIVED_POINT || query.type == ACTIVED_INTERVAL ||
                          query.type == DEACTIVED_POINT || query.type == DEACTIVED_INTERVAL) {
                    failcompare = (gotres != query.expectednres);
                  } else {
                    failcompare = compareRes(gotreslist, query.expectedres);
                    gotres = gotreslist[0];
                  }
                  if (failcompare) {
                    printf("query queryType=%d, row=%d, column=%d, time=%d, initime=%d, endtime=%d, expectedres=%d\n", query.type, query.row, query.column, query.time, query.initime, query.endtime, query.expectednres);
                    printf("count: got %d expected %d\n", gotres, query.expectednres);
                    
		    if ( ! (query.type == EDGE || query.type == EDGE_NEXT || query.type == EDGE_WEAK || query.type == EDGE_STRONG || query.type == SNAPSHOT||
                    query.type == CHANGE_POINT || query.type == CHANGE_INTERVAL || query.type == ACTIVED_POINT || query.type == ACTIVED_INTERVAL ||
                    query.type == DEACTIVED_POINT || query.type == DEACTIVED_INTERVAL) ) {
		        printf("expected: "); print_arraysort(query.expectedres);
                        printf("got     : "); print_arraysort(gotreslist);
	    	    }
		    exit(1);
                  }
                  totalres += gotres;
                }

#else
                totalres += *gotreslist;
#endif




        }
        size_t microsecs = endClockTime()/1000; //to microsecs

//	printf("time = (%lf), %d queries, %lf micros/query, %lf micros/arista\n",
//	               difftime, nqueries,
//	               difftime/nqueries, difftime/totalres);

	        //printf("time = %lf (%ld) (%lf), %d queries, %lf micros/query, %lf micros/arista\n",
	         //      timeFromBegin(), realTimeFromBegin(), difftime, nqueries,
	          //     difftime/nqueries, difftime/totalres);


        // datasets.structura query_input num_queries totaloutput timeperquery timeperoutput
        printf("%s\t%s\t%ld\t%d\t%d\t%lf\t%lf\n", argv[1], argv[2],
                       microsecs, nqueries, totalres, (double)microsecs/nqueries, (double)microsecs/totalres);

        //destroyK2Tree(tree);

        exit(0);

}
