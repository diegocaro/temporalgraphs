/*
 * tgs.c
 *
 *  Created on: Mar 20, 2013
 *      Author: diegocaro
 */

#include <stdio.h>
#include <limits.h>
#include <cstring>
#include <sys/types.h>

#include "tgs.h"
#include "arraysort.h"

using namespace std;
using namespace cds_static;

uint buffer1[BUFFER];
uint buffer3[BUFFER];
size_t buffer2[2*BUFFER];
uint buffer4[BUFFER];

void tgs_save(struct tgs *a, ofstream & f) {
	f.write(reinterpret_cast<char *>(a), sizeof(struct tgs));
  a->map->save(f);
  a->log->save(f);
	//save(a->map, f);
	//wt_save(a->log, f);
}

void tgs_load(struct tgs *a, ifstream & f) {
//fread(a, sizeof(struct tgs), 1, f);
  f.read(reinterpret_cast<char *>(a), sizeof(struct tgs));

  a->map = BitSequence::load(f);
  a->log = WaveletMatrix::load(f);

	//a->map = malloc(sizeof(bitRankW32Int));
	//a->log = malloc(sizeof(struct wt));

	//load(a->map, f);
	//wt_load(a->log, f);
}

void tgs_free(struct tgs *a) {
	//free_wt(a->log);
	//destroyBitRankW32Int(a->map);
	//free(a->log);
  
  delete a->log;
  delete a->map;
  
	a->log = NULL;
	a->map = NULL;
}


size_t tgs_size(struct tgs *a) {
//	printf("%lu + %lu \n", a->log->getSize(), a->map->getSize());
	
	//return (size_t)sizeof(struct tgs) + (size_t)a->log->getSize() + (size_t)a->map->getSize();
	return 0;
}


size_t start(BitSequence *b, uint i) {
//	return b->select1(i+1) - i; 	//in the paper this operation is
                                  //start(i) = select1(b, i) - i + 1

	uint ret = b->select1(i+1);
  if (ret == (uint)-1) {
    return b->getLength() - i;
  }
  else {
    return ret -i;
  }
}

inline uint belong(BitSequence *b, size_t i) {
	//return rank(b, i) - 1;

	//printf("raro %lu\n", b->rank1( b->select0(i) ) - 1);
	//printf("deberia: %lu\n", b->rank1( i ) - 1);

	return b->rank1( b->select0(i) ) - 1;
}

uint get_snapshot(struct tgs *g, uint t) {
	/*uint startnode, endnode, endnode_log;
	
	vector<uint> myres;
	

	uint i;
	*/
	uint count = 0;
	uint node;
	for (node = 0; node < g->nodes; node++) {

		get_neighbors_point(buffer1, g, node, t);
		count += *buffer1;



		/*
		 * Old method
		 * Not used anymore
		startnode = start(g->map, node);
		endnode = start(g->map, node + 1);
		
		endnode_log = g->log->next_value_pos( g->nodes + t + 1, startnode, endnode);

	//	printf("endnode_log: %u\n", endnode_log);
		if (endnode_log < endnode) {
			endnode = endnode_log;
		}
		
		myres.clear();
		g->log->range_report(startnode, endnode, 0, g->nodes , myres);
		for (i = 0; i < myres.size(); i += 2) {
			//printf("buffer2[%u] = %u\n", i, buffer2[i]);
			if (myres[i+1] % 2 == 1) {
				count++;
			}
		}*/
	}
	
	return count;
}

void get_neighbors_point(uint *res, struct tgs *g, uint node, uint t) {
    if (g->typegraph == kPoint) {
        get_neighbors_interval_pg(res, g, node, t,t+1);
        return;
    }

	uint startnode, endnode, endnode_log;
	//uint i, j;
  //printf("node: %u\n", node);
	startnode = start(g->map, node);
	endnode = start(g->map, node + 1);

//	printf("startnode: %u\n", startnode);
	//printf("endnode: %u\n", endnode);

	endnode_log = g->log->next_value_pos( g->nodes + t +1, startnode, endnode);

		//printf("endnode_log: %u\n", endnode_log);


	if (endnode_log < endnode) {
		endnode = endnode_log;
		//printf("acato\n");
	}

	*res = 0;

	((MyWaveletMatrix *)g->log)->range_report<append_odd>(startnode, endnode, 0, g->nodes , res);
	/*
	uint f;
	for(uint i = startnode; i < endnode; i++) {
		f = ((MyWaveletMatrix *)g->log)->access(i);
		printf("S[%u] = ", i);
		if (f < g->nodes) {
			printf("%u (node)\n", f);
		}
		else {
			printf("%u (time %u)\n", f, f - g->nodes);
		}
	}*/
	
}

int get_edge_point(struct tgs *g, uint node, uint v, uint t) {
    if (g->typegraph == kPoint) {
        return get_edge_interval_pg(g, node,v, t,t+1);
    }
	uint startnode, endnode, endnode_log;

	startnode = start(g->map, node);
	endnode = start(g->map, node + 1);

//	printf("startnode: %u\n", startnode);
//	printf("endnode: %u\n", endnode);

	endnode_log = g->log->next_value_pos( g->nodes + t + 1, startnode, endnode);

//	printf("endnode_log: %u\n", endnode_log);


	if (endnode_log < endnode) {
		endnode = endnode_log;
	}
        
        uint rs, re;
        rs = g->log->rank(v, startnode);
        re = g->log->rank(v, endnode);
	uint r = re - rs;
        if ( r%2 == 1 ) return 1;

        return 0;
}

int get_edge_weak(struct tgs *g, uint u, uint v, uint tstart, uint tend) {
    if (g->typegraph == kPoint) {
           return get_edge_interval_pg(g, u,v, tstart,tend);
       }
        return get_edge_interval(g, u, v, tstart, tend, 0);
}
int get_edge_strong(struct tgs *g, uint u, uint v, uint tstart, uint tend) {
    if (g->typegraph == kPoint) {
        if (tend == tstart+1) {
           return get_edge_interval_pg(g, u,v, tstart,tend);
        }
        return 0;
       }

        return get_edge_interval(g, u, v, tstart, tend, 1);        
}

int get_edge_interval(struct tgs *g, uint node, uint v, uint timestart, uint timeend, uint semantic) {
	uint startnode, endnode;
	
	uint pos_stime, pos_etime;
	uint *buffer;

	buffer = (uint *)malloc(sizeof(uint)*BUFFER);

	startnode = start(g->map, node);
	endnode = start(g->map, node + 1);

	pos_stime = g->log->next_value_pos(g->nodes + timestart + 1, startnode, endnode);
	if (pos_stime > endnode) {
		pos_stime = endnode;
	}

	pos_etime = g->log->next_value_pos(g->nodes + timeend + 1, startnode, endnode);
	if (pos_etime > endnode) {
		pos_etime = endnode;
	}
	
        
        uint ris, rie;
        
	ris = g->log->rank(v, pos_stime);
        rie = g->log->rank(v, pos_etime);

        uint r = rie - ris;


	uint e = get_edge_point(g, node, v, timestart);

	if (semantic == 0) { //semantic weak
                if (e ==1 || r>0) return 1;
                
	}
	else if (semantic == 1) { //semantic strong
                if (e==1 && r==0) return 1;
	}
        return 0;
}
int get_edge_next(struct tgs *g, uint node, uint v, uint t) {
    if (g->typegraph == kPoint) {
      return get_edge_next_pg(g, node,v, t);
      }

	uint startnode, endnode, endnode_log;

	startnode = start(g->map, node);
	endnode = start(g->map, node + 1);
//	printf("startnode: %u\n", startnode);
//	printf("endnode: %u\n", endnode);

	endnode_log = g->log->next_value_pos( g->nodes + t + 1, startnode, endnode);

//	printf("endnode_log: %u\n", endnode_log);


	if (endnode_log < endnode) {
		endnode = endnode_log;
	}
        
        uint rs, re;
        rs = g->log->rank(v, startnode);
        re = g->log->rank(v, endnode);
	uint r = re - rs;
        if ( r%2 == 1 ) return t;

        uint c;
        c = g->log->select(v, re+1);
        
        if (c < endnode) {
                return g->log->prev_value(g->nodes + g->maxtime, startnode, c);
        }


        return -1;
        
}


void get_neighbors_weak(uint *res, struct tgs *g, uint node, uint timestart, uint timeend) {
    if (g->typegraph == kPoint) {
        get_neighbors_interval_pg(res, g, node, timestart,timeend);
        return;
    }

	return get_neighbors_interval(res, g, node, timestart, timeend, 0);
}

void get_neighbors_strong(uint *res, struct tgs *g, uint node, uint timestart, uint timeend) {
    if (g->typegraph == kPoint) {
        if (timestart+1 == timeend) {
            get_neighbors_interval_pg(res,g,node,timestart,timeend);
        }
        return;
    }
	return get_neighbors_interval(res, g, node, timestart, timeend, 1);
}


void get_neighbors_interval(uint *res, struct tgs *g, uint node, uint timestart, uint timeend, uint semantic) {
	uint startnode, endnode;
	uint i, j;
	uint pos_stime, pos_etime;


	startnode = start(g->map, node);
	endnode = start(g->map, node + 1);

	pos_stime = g->log->next_value_pos(g->nodes + timestart + 1, startnode, endnode);
	if (pos_stime > endnode) {
		pos_stime = endnode;
	}

	pos_etime = g->log->next_value_pos(g->nodes + timeend + 1, startnode, endnode);
	if (pos_etime > endnode) {
		pos_etime = endnode;
	}
	//printf("stime: %u\netime: %u\n", pos_stime, pos_etime);
	*buffer1 = 0;

	((MyWaveletMatrix *)g->log)->range_report<append_symbol>(pos_stime, pos_etime, 0, g->nodes , buffer1);
	//count_symbols_range(g->log, startnode, endnode, buffer2);

	*res = 0;
	get_neighbors_point(buffer3, g, node, timestart);

  //this semantic filter is O(d) where d is the out degree of the node
	if (semantic == 0) { //semantic weak
/*		j = *res;
		for (i = 1; i <= *buffer1; i++) {
			res[++j] = buffer1[i];
		}
		*res = j;

		qsort(&res[1], *res, sizeof(unsigned int), compare);

		remove_duplicates(res);
    */
    merge_arraysort(res, buffer3, buffer1);
    
	}
	else if (semantic == 1) { //semantic strong

		//printf("direct neighbors: "); print_arraysort(buffer3);
		//diff_arraysort(res, buffer1);
    
		diff_arraysort(buffer3, buffer1);
    memcpy(res, buffer3, (*buffer3+1)*sizeof(uint));
	}

}




void get_reverse_point(uint *res, struct tgs *g, uint node, uint time) {
    if (g->typegraph == kPoint) {
        get_reverse_interval_pg(res, g, node, time,time+1);
        return;
    }

	uint curr_node;
	size_t i, j;
	size_t cant, cpos;
	size_t startnode, endnode, endnode_log;
	//uint *buffer;
  //vector<uint> buffer;
	size_t nextnode;
	
  //buffer = malloc(sizeof(uint)*BUFFER);
	//*buffer = 0;

	size_t *p;  
	((MyWaveletMatrix *)g->log)->select_all(node, buffer2);
  
	j = 0;
	for (i = 1; i <= *buffer2; i++) {
		curr_node = belong(g->map, buffer2[i]);

		startnode = start(g->map, curr_node);
		nextnode = start(g->map, curr_node + 1);
    
		endnode_log = g->log->next_value_pos(g->nodes + time + 1, startnode, nextnode);
		endnode = nextnode;
		if (endnode_log < endnode) {
			endnode = endnode_log;
		}

		cant = i;
		//while(i <= *buffer2 && buffer2[i] < endnode ) {
		//	i++;
		//}
		p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], endnode);
                i = p - &buffer2[0];

		cpos = i;

		if ( (cpos - cant) % 2 == 1 ) {
			res[++j] = curr_node;
		}

		p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], nextnode);
		i = p - &buffer2[0];

		//se salta hasta el siguiente nodo, reemplazado por el lower_bound (binsearch)
//		while(i <= *buffer2 && buffer2[i] < nextnode) {
//			i++;
//		}

		i--;

	}
	*res = j;

	//free(buffer);
}


/*
void get_reverse_point_slow(uint *res, struct tgs *g, uint node, uint time) {
	uint last_node, curr_node;
	uint i, j;
	uint cant, cpos;
	uint startnode, endnode, endnode_log;
  
  vector<uint> buffer;
  
	//uint *buffer;
  //buffer = malloc(sizeof(uint)*BUFFER);
  // *buffer = 0;

	g->log->select_all(node, buffer);

	last_node = UINT_MAX;

	j = 0;

	for (i = 0; i < buffer.size(); i++) {
		curr_node = belong(g->map, buffer[i]);

#ifdef DEBUG
		printf("curr_node: %u\n", curr_node);
#endif

		if ( curr_node != last_node ) {
			startnode = start(g->map, curr_node);
			endnode = start(g->map, curr_node + 1);

			endnode_log = g->log->next_value_pos(g->nodes + time + 1, startnode, endnode);
			if (endnode_log < endnode) {
				endnode = endnode_log;
			}
#ifdef DEBUG
			printf("startnode: %u\nendnode: %u\n", startnode, endnode);
#endif
			cant = g->log->rank(node, startnode);
			cpos = g->log->rank(node, endnode);

			//printf("cant: %u\ncpos: %u\n", cant,cpos);

			if ( (cpos - cant) % 2 == 1 ) {
				res[++j] = curr_node;
			}
		}
		last_node = curr_node;
	}
	*res = j;

	//free(buffer);
}
*/

void get_reverse_weak(uint *res, struct tgs *g, uint node, uint ts, uint te) {
    if (g->typegraph == kPoint) {
        get_reverse_interval_pg(res, g, node, ts,te);
        return;
    }
	return get_reverse_interval(res, g, node, ts, te, 0);
}

void get_reverse_strong(uint *res, struct tgs *g, uint node, uint ts, uint te) {
    if (g->typegraph == kPoint) {
        if (te == ts+1) {
            get_reverse_interval_pg(res,g,node,ts,te);
        }
        return;
    }
	return get_reverse_interval(res, g, node, ts, te, 1);
}

void get_reverse_interval(uint *res, struct tgs *g, uint node, uint ts, uint te, uint semantic) {
	uint last_node, curr_node;
	size_t i, j;
	size_t cstart, cts, cte;
	size_t startnode, endnode, pos_stime, pos_etime;
	//uint *buffer;
	size_t nextnode;
	size_t *p;

 // vector<uint> buffer;

	//buffer = malloc(sizeof(uint)*BUFFER);

	//*buffer = 0;

	((MyWaveletMatrix *)g->log)->select_all(node, buffer2);

	last_node = UINT_MAX;

	j = 0;

	for (i = 1; i <= *buffer2; i++) {
		curr_node = belong(g->map, buffer2[i]);

		startnode = start(g->map, curr_node);
		nextnode = start(g->map, curr_node + 1);

		endnode = nextnode;

		pos_stime = g->log->next_value_pos(g->nodes + ts + 1, startnode, endnode);
		if (pos_stime > endnode) {
			pos_stime = endnode;
		}

		pos_etime = g->log->next_value_pos(g->nodes + te + 1, startnode, endnode);
		if (pos_etime > endnode) {
			pos_etime = endnode;
		}

   // printf("startnode: %u\nendnode: %u\n", startnode, endnode);
    //printf("pos_stime: %u\npos_etime: %u\n", pos_stime, pos_etime);

#ifdef DEBUG
		printf("startnode: %u\nendnode: %u\n", startnode, endnode);
#endif
		//cstart = rank_wt(g->log, node, startnode);
		cstart = i;
		//printf("rankwt: %u\n", g->log->rank(node, startnode));
		//printf("cstart: %u\n", cstart);



		//cts = rank_wt(g->log, node, pos_stime);
		// or
		//while(i <= *buffer2 && buffer2[i] < pos_stime ) {
		//			i++;
		//}
		//but better a binary search
		p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], pos_stime);
                i = p - &buffer2[0];
		cts = i;

    //printf("rankwt: %u\n", g->log->rank(node, pos_stime));
    //printf("cts: %u\n", cts);


		//cte = rank_wt(g->log, node, pos_etime);
		// or
		//while(i <= *buffer2 && buffer2[i] < pos_etime ) {
		//	i++;
		//}
		// but better a binary search
		p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], pos_etime);
                i = p - &buffer2[0];
		cte = i;
		
    //printf("rankwt: %u\n", g->log->rank(node, pos_etime));
    //printf("cte: %u\n", cte);



		if (semantic == 0) {
			// semantic = 0 weak
			if ( (cts - cstart) % 2 == 1 || cte > cts) {
				res[++j] = curr_node;
			}
		}
		else if (semantic == 1) {
			//semantic = 1 strong
			if ( (cts - cstart) % 2 == 1 && cte == cts) {
				res[++j] = curr_node;
			}
		}


		//while(i <= *buffer2 && buffer2[i] < nextnode) {
		//	i++;
		//}

		p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], nextnode);
                i = p - &buffer2[0];
		i--;


	}
	*res = j;

	//free(buffer);
}

/*
void get_reverse_interval_slow(uint *res, struct tgs *g, uint node, uint ts, uint te, uint semantic) {
	uint last_node, curr_node;
	uint i, j;
	uint cstart, cts, cte;
	uint startnode, endnode, pos_stime, pos_etime;
	//uint *buffer;

//	buffer = malloc(sizeof(uint)*BUFFER);

	// *buffer = 0;

  vector<uint> buffer;

	g->log->select_all(node, buffer);

	last_node = UINT_MAX;

	j = 0;

	for (i = 0; i < buffer.size(); i++) {
		curr_node = belong(g->map, buffer[i]);

#ifdef DEBUG
		printf("curr_node: %u\n", curr_node);
#endif

		if ( curr_node != last_node ) {
			startnode = start(g->map, curr_node);
			endnode = start(g->map, curr_node + 1);

			pos_stime = g->log->next_value_pos(g->nodes + ts + 1, startnode, endnode);
			if (pos_stime > endnode) {
				pos_stime = endnode;
			}

			pos_etime = g->log->next_value_pos(g->nodes + te + 1, startnode, endnode);
			if (pos_etime > endnode) {
				pos_etime = endnode;
			}



#ifdef DEBUG
			printf("startnode: %u\nendnode: %u\n", startnode, endnode);
#endif
			cstart = g->log->rank(node, startnode);
			cts = g->log->rank(node, pos_stime);
			cte = g->log->rank(node, pos_etime);

			if (semantic == 0) {
				// semantic = 0 weak
				if ( (cts - cstart) % 2 == 1 || cte > cts) {
					res[++j] = curr_node;
				}
			}
			else {
				//semantic = 1 strong
				if ( (cts - cstart) % 2 == 1 && cte == cts) {
					res[++j] = curr_node;
				}
			}

		}
		last_node = curr_node;
	}
	*res = j;

	//free(buffer);
}
*/

size_t get_change_node(struct tgs *g, uint node, uint timestart, uint timeend) {
    uint startnode, endnode;
    uint i, j;
    uint pos_stime, pos_etime;


    startnode = start(g->map, node);
    endnode = start(g->map, node + 1);

    pos_stime = g->log->next_value_pos(g->nodes + timestart + 1, startnode, endnode);
    if (pos_stime > endnode) {
        pos_stime = endnode;
    }

    pos_etime = g->log->next_value_pos(g->nodes + timeend + 1, startnode, endnode);
    if (pos_etime > endnode) {
        pos_etime = endnode;
    }

    *buffer1 = 0;

    ((MyWaveletMatrix *)g->log)->range_report<append_dummy>(pos_stime, pos_etime, 0, g->nodes , buffer1);

    return (size_t)*buffer1;
}


size_t get_change_interval(struct tgs *g, uint ts, uint te) {
    size_t edges = 0;
    uint node;
    for (node = 0; node < g->nodes; node++) {
        edges += get_change_node(g,node,ts-1,te-1);
    }

    return edges;
}

size_t get_change_point(struct tgs *g, uint t) {
    return get_change_interval(g,t,t+1);
}



size_t get_actived_node(struct tgs *g, uint node, uint timestart, uint timeend) {
    uint startnode, endnode;
    uint i, j;
    uint pos_stime, pos_etime;


    startnode = start(g->map, node);
    endnode = start(g->map, node + 1);

    pos_stime = g->log->next_value_pos(g->nodes + timestart + 1, startnode, endnode);
    if (pos_stime > endnode) {
        pos_stime = endnode;
    }

    pos_etime = g->log->next_value_pos(g->nodes + timeend + 1, startnode, endnode);
    if (pos_etime > endnode) {
        pos_etime = endnode;
    }

    *buffer1 = 0;
    ((MyWaveletMatrix *)g->log)->range_report<append_symbol>(pos_stime, pos_etime, 0, g->nodes , buffer1);


    get_neighbors_point(buffer3, g, node, timestart);

    diff_arraysort(buffer1, buffer3);

    return (size_t)*buffer1;
}


size_t get_actived_interval(struct tgs *g, uint ts, uint te) {
    if (g->typegraph == kPoint) {
        return get_actived_interval_pg(g,ts,te);
    }

    size_t edges = 0;
    uint node;
    for (node = 0; node < g->nodes; node++) {
        edges += get_actived_node(g,node,ts-1,te-1);
    }

    return edges;
}

size_t get_actived_point(struct tgs *g, uint t) {
    return get_actived_interval(g,t,t+1);
}

size_t get_deactived_node(struct tgs *g, uint node, uint timestart, uint timeend) {
    uint startnode, endnode;
    uint i, j;
    uint pos_stime, pos_etime;


    startnode = start(g->map, node);
    endnode = start(g->map, node + 1);

    pos_stime = g->log->next_value_pos(g->nodes + timestart + 1, startnode, endnode);
    if (pos_stime > endnode) {
        pos_stime = endnode;
    }

    pos_etime = g->log->next_value_pos(g->nodes + timeend + 1, startnode, endnode);
    if (pos_etime > endnode) {
        pos_etime = endnode;
    }

    *buffer1 = 0;

    ((MyWaveletMatrix *)g->log)->range_report<append_symbol>(pos_stime, pos_etime, 0, g->nodes , buffer1);


    get_neighbors_point(buffer3, g, node, timestart);

    intersection_arraysort(buffer4, buffer1, buffer3);

    return (size_t)*buffer4;
}


size_t get_deactived_interval(struct tgs *g, uint ts, uint te) {
    if (g->typegraph == kPoint) {
        return get_deactived_interval_pg(g,ts,te);
    }

    size_t edges = 0;
    uint node;
    for (node = 0; node < g->nodes; node++) {
        edges += get_deactived_node(g,node,ts-1,te-1);
    }

    return edges;
}

size_t get_deactived_point(struct tgs *g, uint t) {
    return get_deactived_interval(g,t,t+1);
}




// point contact graphs
void get_reverse_interval_pg(uint *res, struct tgs *g, uint node, uint ts, uint te) {
    uint last_node, curr_node;
    size_t i, j;
    size_t cstart, cts, cte;
    size_t startnode, endnode, pos_stime, pos_etime;
    //uint *buffer;
    size_t nextnode;
    size_t *p;

 // vector<uint> buffer;

    //buffer = malloc(sizeof(uint)*BUFFER);

    //*buffer = 0;

    ((MyWaveletMatrix *)g->log)->select_all(node, buffer2);

    last_node = UINT_MAX;

    j = 0;

    for (i = 1; i <= *buffer2; i++) {
        curr_node = belong(g->map, buffer2[i]);

        startnode = start(g->map, curr_node);
        nextnode = start(g->map, curr_node + 1);

        endnode = nextnode;

        pos_stime = g->log->next_value_pos(g->nodes + ts , startnode, endnode);
        if (pos_stime > endnode) {
            pos_stime = endnode;
        }

        pos_etime = g->log->next_value_pos(g->nodes + te , startnode, endnode);
        if (pos_etime > endnode) {
            pos_etime = endnode;
        }

   // printf("startnode: %u\nendnode: %u\n", startnode, endnode);
    //printf("pos_stime: %u\npos_etime: %u\n", pos_stime, pos_etime);

#ifdef DEBUG
        printf("startnode: %u\nendnode: %u\n", startnode, endnode);
#endif
        //cstart = rank_wt(g->log, node, startnode);
        cstart = i;
        //printf("rankwt: %u\n", g->log->rank(node, startnode));
        //printf("cstart: %u\n", cstart);



        //cts = rank_wt(g->log, node, pos_stime);
        // or
        //while(i <= *buffer2 && buffer2[i] < pos_stime ) {
        //          i++;
        //}
        //but better a binary search
        p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], pos_stime);
                i = p - &buffer2[0];
        cts = i;

    //printf("rankwt: %u\n", g->log->rank(node, pos_stime));
    //printf("cts: %u\n", cts);


        //cte = rank_wt(g->log, node, pos_etime);
        // or
        //while(i <= *buffer2 && buffer2[i] < pos_etime ) {
        //  i++;
        //}
        // but better a binary search
        p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], pos_etime);
                i = p - &buffer2[0];
        cte = i;

    //printf("rankwt: %u\n", g->log->rank(node, pos_etime));
    //printf("cte: %u\n", cte);



            if ( cte > cts) {
                res[++j] = curr_node;
            }

        //while(i <= *buffer2 && buffer2[i] < nextnode) {
        //  i++;
        //}

        p = lower_bound(&buffer2[i], &buffer2[*buffer2+1], nextnode);
                i = p - &buffer2[0];
        i--;


    }
    *res = j;

    //free(buffer);
}

void get_neighbors_interval_pg(uint *res, struct tgs *g, uint node, uint timestart, uint timeend) {
    uint startnode, endnode;
    uint i, j;
    uint pos_stime, pos_etime;

    startnode = start(g->map, node);
    endnode = start(g->map, node + 1);

    pos_stime = g->log->next_value_pos(g->nodes + timestart , startnode, endnode);
    if (pos_stime > endnode) {
        pos_stime = endnode;
    }

    pos_etime = g->log->next_value_pos(g->nodes + timeend , startnode, endnode);
    if (pos_etime > endnode) {
        pos_etime = endnode;
    }

    ((MyWaveletMatrix *)g->log)->range_report<append_symbol>(pos_stime, pos_etime, 0, g->nodes , res);
}

int get_edge_interval_pg(struct tgs *g, uint node, uint v, uint timestart, uint timeend) {
    uint startnode, endnode;

    uint pos_stime, pos_etime;
    uint *buffer;

    buffer = (uint *)malloc(sizeof(uint)*BUFFER);

    startnode = start(g->map, node);
    endnode = start(g->map, node + 1);

    pos_stime = g->log->next_value_pos(g->nodes + timestart , startnode, endnode);
    if (pos_stime > endnode) {
        pos_stime = endnode;
    }

    pos_etime = g->log->next_value_pos(g->nodes + timeend , startnode, endnode);
    if (pos_etime > endnode) {
        pos_etime = endnode;
    }
        uint ris, rie;

    ris = g->log->rank(v, pos_stime);
        rie = g->log->rank(v, pos_etime);

        uint r = rie - ris;

       if(r>0) return 1;
            return 0;

}
int get_edge_next_pg(struct tgs *g, uint node, uint v, uint t) {
    uint startnode, endnode, endnode_log;

    startnode = start(g->map, node);
    endnode = start(g->map, node + 1);
//  printf("startnode: %u\n", startnode);
//  printf("endnode: %u\n", endnode);

    endnode_log = g->log->next_value_pos( g->nodes + t + 1, startnode, endnode);

//  printf("endnode_log: %u\n", endnode_log);


    if (endnode_log < endnode) {
        endnode = endnode_log;
    }

        uint rs, re;
        rs = g->log->rank(v, startnode);
        re = g->log->rank(v, endnode);
    uint r = re - rs;
        if ( r > 1 ) return t;

        uint c;
        c = g->log->select(v, re+1);

        if (c < endnode) {
                return g->log->prev_value(g->nodes + g->maxtime, startnode, c);
        }


        return -1;
}

size_t get_actived_interval_pg(struct tgs *g, uint ts, uint te) {
return get_change_interval(g,ts,te);
}
size_t get_deactived_interval_pg(struct tgs *g, uint ts, uint te) {
return get_change_interval(g,ts-1,te-1);
}

