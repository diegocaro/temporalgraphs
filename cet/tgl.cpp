#include <cstring>
#include "tgl.h"
#include "arraysort.h"
#include "mywtkdmatrix.h"
//#include "interleave.h"


uint buffer[BUFFER];
uint buffer2[BUFFER];


TemporalGraphLog::TemporalGraphLog() {};

TemporalGraphLog::TemporalGraphLog(uint nodes, uint changes, uint maxtime) {
	this->nodes = nodes;
	this->changes = changes;
	this->maxtime = maxtime;
}

void TemporalGraphLog::set_nodes(uint nodes) {this->nodes = nodes;}
void TemporalGraphLog::set_changes(uint changes) {this->changes = changes;}
void TemporalGraphLog::set_maxtime(uint maxtime) {this->maxtime = maxtime;}


void TemporalGraphLog::set_log(usym *log, size_t size_log, BitSequenceBuilder *bs) {
	this->size_log = size_log;
	/*this->log = new WaveletTree(log, size_log,
			new wt_coder_binary(log, size_log, new MapperNone()),
			new BitSequenceBuilderRG(20),
			new MapperNone());
  */
	/*
  this->log = new WaveletTree(log, size_log,
  			new wt_coder_huff_morton(log, size_log),
  			new BitSequenceBuilderRG(20));
	*/
	this->log = new WaveletKdMatrix(log, size_log, bs);
  
}

void TemporalGraphLog::set_time(uint *time, size_t size_time, BitSequenceBuilder *bs) {
	this->size_time = size_time;
	this->time = bs->build(time, size_time);
}

void TemporalGraphLog::save(ofstream &fp) {
	saveValue<TemporalGraphLog>(fp, this, 1);
	this->log->save(fp);
	this->time->save(fp);
}

TemporalGraphLog* TemporalGraphLog::load(ifstream &fp) {
	TemporalGraphLog *ret;
	//ret = new TemporalGraphLog();
	ret = loadValue<TemporalGraphLog>(fp,1);
	//printf("changes: %u\n", ret->get_changes());
	ret->log = WaveletKdMatrix::load(fp);
	ret->time = BitSequence::load(fp);
	return ret;
}

size_t TemporalGraphLog::pos_time(size_t i) const {
//	return b->select1(i+1) - i; 	//in the paper this operation is
                                  //start(i) = select1(b, i) - i + 1

    if (i == (uint)(-1)) {
        return 0;
    }

  uint ret = time->select1(i+1);
  //printf("ret: %u\n", ret);
  if (ret == (uint)(-1)) {
    return time->getLength() - i;
  }
  else {
    return ret - i;
  }
}

void TemporalGraphLog::direct_point(uint node, uint t, uint *res) const {
    if (typegraph_ == kPoint) {
        direct_interval_pg(node,t,t+1,res);
        return;
    }

	size_t ptime;
	ptime = pos_time(t);

	*res = 0;
	((MyWaveletKdMatrix *)log)->axis<append_odd>(0U, ptime, 0U, node, res);
}

void TemporalGraphLog::direct_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const {
	size_t sptime;
	size_t eptime;
	sptime = pos_time(tstart);
	eptime = pos_time(tend);
	
	
	*buffer = 0;
	((MyWaveletKdMatrix *)log)->axis<append_symbol>(sptime, eptime, 0U, node, buffer);
	
	*buffer2 = 0;
	direct_point(node, tstart, buffer2);

	//uint i,j;
  //this semantic filter is O(d) where d is the out degree of the node
	if (semantic == 0) { //semantic weak
		/*
    j = *res;
		for (i = 1; i <= *buffer; i++) {
			res[++j] = buffer[i];
		}
		*res = j;

		qsort(&res[1], *res, sizeof(unsigned int), compare);

		remove_duplicates(res);
    */
    merge_arraysort(res, buffer2, buffer);
	}
	else if (semantic == 1) { //semantic strong

		//printf("direct neighbors: "); print_arraysort(buffer3);
		diff_arraysort(buffer2, buffer);
    memcpy(res, buffer2, (*buffer2+1)*sizeof(uint));
	}
	
}

void TemporalGraphLog::direct_weak(uint node, uint tstart, uint tend, uint *res) const {
    if (typegraph_ == kPoint) {
        direct_interval_pg(node,tstart,tend,res);
        return;
    }

	direct_interval(node, tstart, tend, 0, res);
}

void TemporalGraphLog::direct_strong(uint node, uint tstart, uint tend, uint *res) const {
    if (typegraph_ == kPoint) {
        if (tend == tstart+1) {
            direct_interval_pg(node,tstart,tend,res);
        }
        return;
    }

	direct_interval(node, tstart, tend, 1, res);
}


void TemporalGraphLog::reverse_point(uint node, uint t, uint *res) const {
    if (typegraph_ == kPoint) {
        reverse_interval_pg(node,t,t+1,res);
        return;
    }

	size_t ptime;
	ptime = pos_time(t);
	
	*res = 0;
	((MyWaveletKdMatrix *)log)->axis<append_odd>(0U, ptime, 1U, node, res);
}


void TemporalGraphLog::reverse_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const {
	size_t sptime;
	size_t eptime;
	sptime = pos_time(tstart);
	eptime = pos_time(tend);
	
	*buffer = 0;
	((MyWaveletKdMatrix *)log)->axis<append_symbol>(sptime, eptime, 1U, node, buffer);

	*buffer2 = 0;
	reverse_point(node, tstart, buffer2);

	//uint j;
	//uint i;
  
  //this semantic filter is O(d) where d is the in degree of the node
	if (semantic == 0) { //semantic weak
		/*
    j = *res;
		for (i = 1; i <= *buffer; i++) {
			res[++j] = buffer[i];
		}
		*res = j;

		qsort(&res[1], *res, sizeof(unsigned int), compare);

		remove_duplicates(res);
    */
    merge_arraysort(res, buffer2, buffer);
	}
	else if (semantic == 1) { //semantic strong

		//printf("direct neighbors: "); print_arraysort(buffer3);
		diff_arraysort(buffer2, buffer);
    memcpy(res, buffer2, (*buffer2+1)*sizeof(uint));
	}
}

void TemporalGraphLog::reverse_weak(uint node, uint tstart, uint tend, uint *res) const {
    if (typegraph_ == kPoint) {
        reverse_interval_pg(node,tstart,tend,res);
        return;
    }

	reverse_interval(node, tstart, tend, 0, res);
}

void TemporalGraphLog::reverse_strong(uint node, uint tstart, uint tend, uint *res) const {
    if (typegraph_ == kPoint) {
        if (tend == tstart+1) {
            reverse_interval_pg(node,tstart,tend,res);
        }
        return;
    }

	reverse_interval(node, tstart, tend, 1, res);
}


size_t TemporalGraphLog::snapshot(uint t) const {
    if (typegraph_ == kPoint) {
        return snapshot_pg(t);
    }

	size_t etime;
	etime = pos_time(t);
	
	size_t active_edges=0;
	
	((MyWaveletKdMatrix *)log)->rankall(0, etime,active_edges);

	
	return active_edges;
}


int TemporalGraphLog::edge_point(uint u, uint v, uint t) {
    if (typegraph_ == kPoint) {
        return edge_interval_pg(u,v,t,t+1);
       }

	size_t ptime;
	ptime = pos_time(t);
        
        struct symbols s;
        s.x=u;
        s.y=v;
        
        if (log->rank(s, ptime) % 2 == 1) {
                return 1;
        }
        return 0;
}

int TemporalGraphLog::edge_interval(uint u, uint v, uint tstart, uint tend, uint semantic) {
	size_t sptime;
    size_t eptime;

    sptime = pos_time(tstart)-1;
    eptime = pos_time(tend)-1;
        
        struct symbols s;
        s.x=u;
        s.y=v;
        
        uint rstart;
        uint rend;
        
        rstart = log->rank(s, sptime);
        rend = log->rank(s, eptime);
        
        if (semantic == 0) {
                if ( ( rstart % 2 == 1) || (rend > rstart) ) 
                        return 1;
        }
        else if (semantic == 1) {
                if ( ( rstart % 2 == 1) && (rend == rstart) ) 
                        return 1;
        }
        
        return 0;
        
}

int TemporalGraphLog::edge_weak(uint u, uint v, uint tstart, uint tend) {
    if (typegraph_ == kPoint) {
        return edge_interval_pg(u,v,tstart,tend);
       }
        return edge_interval(u, v, tstart, tend, 0);
}
int TemporalGraphLog::edge_strong(uint u, uint v, uint tstart, uint tend){
    if (typegraph_ == kPoint) {
           if (tend == tstart+1) {
               return edge_interval_pg(u,v,tstart,tend);
           }
           return 0;
       }

        return edge_interval(u, v, tstart, tend, 1);
}

int TemporalGraphLog::edge_next(uint u, uint v, uint t) {
    if (typegraph_ == kPoint) {
            return edge_next_pg(u,v,t);
           }

	size_t ptime;
	ptime = pos_time(t);
        
        struct symbols s;
        s.x=u;
        s.y=v;
        
        uint r;
        r = log->rank(s, ptime);
        
        if (r % 2 == 1) {
                return t;
        }
        
        uint l;
        l = log->select(s, r+1);
        if ( l != log->n ) {
                return time->rank1(l);
        }
        
        return -1;
}

size_t TemporalGraphLog::change_point(uint t) {
    return change_interval(t,t+1);
}

size_t TemporalGraphLog::change_interval(uint ts, uint te) {
    size_t stime, etime;
    stime = pos_time(ts-1);
    etime = pos_time(te-1);

    //printf("stime(%u): %lu\n", ts, stime);
    //printf("etime(%u): %lu\n", te, etime);

    size_t edges=0;

    ((MyWaveletKdMatrix *)log)->range<append_change>(stime, etime, edges);


    return edges;
}


size_t TemporalGraphLog::actived_point(uint t) {
    return actived_interval(t,t+1);
}

size_t TemporalGraphLog::actived_interval(uint ts, uint te) {
    if (typegraph_ == kPoint) {
        return actived_interval_pg(ts,te);
    }

    size_t stime, etime;
    stime = pos_time(ts-1);
    etime = pos_time(te-1);

    //printf("stime(%u): %lu\n", ts, stime);
    //printf("etime(%u): %lu\n", te, etime);
	
    size_t edges=0;

    ((MyWaveletKdMatrix *)log)->range<append_actived>(stime, etime, edges);


    return edges;
}

size_t TemporalGraphLog::deactived_point(uint t) {
    return deactived_interval(t,t+1);
}

size_t TemporalGraphLog::deactived_interval(uint ts, uint te) {
    if (typegraph_ == kPoint) {
        return deactived_interval_pg(ts,te);
    }

    size_t stime, etime;
    stime = pos_time(ts-1);
    etime = pos_time(te-1);

    //printf("stime(%u): %lu\n", ts, stime);
    //printf("etime(%u): %lu\n", te, etime);

    size_t edges=0;

    ((MyWaveletKdMatrix *)log)->range<append_deactived>(stime, etime, edges);


    return edges;
}







// point contact graphs
void TemporalGraphLog::direct_interval_pg(uint node, uint tstart, uint tend, uint *res) const {
    size_t sptime;
    size_t eptime;
    sptime = pos_time(tstart-1);
    eptime = pos_time(tend-1);

    ((MyWaveletKdMatrix *)log)->axis<append_symbol>(sptime, eptime, 0U, node, res);
}

void TemporalGraphLog::reverse_interval_pg(uint node, uint tstart, uint tend, uint *res) const {
    size_t sptime;
    size_t eptime;
    sptime = pos_time(tstart-1);
    eptime = pos_time(tend-1);

    ((MyWaveletKdMatrix *)log)->axis<append_symbol>(sptime, eptime, 1U, node, res);
}

int TemporalGraphLog::edge_interval_pg(uint u, uint v, uint tstart, uint tend) const {
    size_t sptime;
    size_t eptime;
    sptime = pos_time(tstart)-1;
    eptime = pos_time(tend)-1;

    struct symbols s;
    s.x=u;
    s.y=v;

    uint rstart;
    uint rend;

    rstart = log->rank(s, sptime);
    rend = log->rank(s, eptime);

    if (rend > rstart)
        return 1;

    return 0;
}
size_t TemporalGraphLog::snapshot_pg(uint t) const {
    size_t sptime;
    size_t eptime;
    sptime = pos_time(t-1);
    eptime = pos_time(t);

    size_t active_edges=0;

    ((MyWaveletKdMatrix *)log)->rankall(sptime, eptime,active_edges);


    return active_edges;
}

int TemporalGraphLog::edge_next_pg(uint u, uint v, uint t) {
    size_t ptime;
    size_t etime;
    ptime = pos_time(t);
    etime = pos_time(t+1);

        struct symbols s;
        s.x=u;
        s.y=v;

        uint rstart;
        uint rend;

        rstart = log->rank(s, ptime);
        rend = log->rank(s, etime);

        if (rend > rstart) {
            return t;
        }


        uint l;
        l = log->select(s, rend+1);
        if ( l != log->n ) {
                return time->rank1(l);
        }

        return -1;
}

size_t TemporalGraphLog::actived_interval_pg(uint ts, uint te) {
    return change_interval(ts,te);
}
size_t TemporalGraphLog::deactived_interval_pg(uint ts, uint te) {
    return change_interval(ts-1,te-1);
}

