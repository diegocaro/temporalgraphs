/*
 * tgs.h
 *
 *  Created on: Mar 20, 2013
 *      Author: diegocaro
 */

#ifndef TGS_H_
#define TGS_H_

#include <libcdsBasics.h>
#include <BitSequence.h>
#include "mywtmatrix.h"

using namespace std;
using namespace cds_static;

#define BUFFER 1024*1024*10

enum TypeGraph {
    kInterval, kGrowth, kPoint
};


struct tgs {
	uint size_log;
	uint size_map;
	uint nodes;
	uint changes;
	uint maxtime;
	WaveletMatrix *log;
	BitSequence *map;
	TypeGraph typegraph;
};


void tgs_save(struct tgs *a, ofstream & f);
void tgs_free(struct tgs *a);
void tgs_load(struct tgs *a, ifstream & f);

size_t tgs_size(struct tgs *a);

size_t start(BitSequence *b, uint i) ;
inline uint belong(BitSequence *b, size_t i);

void get_neighbors_point(uint *res, struct tgs *g, uint node, uint time);
void get_neighbors_interval(uint *res, struct tgs *g, uint node, uint timestart, uint timeend, uint semantic);
void get_neighbors_weak(uint *res, struct tgs *g, uint node, uint timestart, uint timeend);
void get_neighbors_strong(uint *res, struct tgs *g, uint node, uint timestart, uint timeend);

void get_reverse_point(uint *res, struct tgs *g, uint node, uint time);

void get_reverse_interval(uint *res, struct tgs *g, uint node, uint ts, uint te, uint semantic);
void get_reverse_weak(uint *res, struct tgs *g, uint node, uint ts, uint te);
void get_reverse_strong(uint *res, struct tgs *g, uint node, uint ts, uint te);

//void get_reverse_point_slow(uint *res, struct tgs *g, uint node, uint time);
//void get_reverse_interval_slow(uint *res, struct tgs *g, uint node, uint ts, uint te, uint semantic);

uint get_snapshot(struct tgs *g, uint t);


int get_edge_point(struct tgs *g, uint u, uint v, uint t);
int get_edge_weak(struct tgs *g, uint u, uint v, uint tstart, uint tend);
int get_edge_strong(struct tgs *g, uint u, uint v, uint tstart, uint tend);

int get_edge_next(struct tgs *g, uint u, uint v, uint t);

int get_edge_interval(struct tgs *g, uint u, uint v, uint tstart, uint tend, uint semantic);

size_t get_change_node(struct tgs *g, uint n, uint ts, uint te);
size_t get_change_point(struct tgs *g, uint t);
size_t get_change_interval(struct tgs *g, uint ts, uint te);

size_t get_actived_node(struct tgs *g, uint n, uint ts, uint te);
size_t get_actived_point(struct tgs *g, uint t);
size_t get_actived_interval(struct tgs *g, uint ts, uint te);

size_t get_deactived_node(struct tgs *g, uint n, uint ts, uint te);
size_t get_deactived_point(struct tgs *g, uint t);
size_t get_deactived_interval(struct tgs *g, uint ts, uint te);




// point contact graphs
void get_reverse_interval_pg(uint *res, struct tgs *g, uint node, uint ts, uint te);
void get_neighbors_interval_pg(uint *res, struct tgs *g, uint node, uint timestart, uint timeend);
int get_edge_interval_pg(struct tgs *g, uint u, uint v, uint tstart, uint tend);
int get_edge_next_pg(struct tgs *g, uint u, uint v, uint t);

//size_t snapshot_pg(uint t); // already captured by get_neighbors_interval_pg

size_t get_actived_interval_pg(struct tgs *g, uint ts, uint te);
size_t get_deactived_interval_pg(struct tgs *g, uint ts, uint te);


#endif /* TGS_H_ */
