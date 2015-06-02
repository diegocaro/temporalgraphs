#ifndef TGL_H_
#define TGL_H_

#include <BitSequence.h>
#include "wavelet_kd_matrix.h"

#define BUFFER 1024*1024*10

enum TypeGraph {
    kInterval, kGrowth, kPoint
};

class TemporalGraphLog {
public:
	TemporalGraphLog();
	TemporalGraphLog(uint nodes, uint changes, uint maxtime);
	
	void set_log(usym *log, size_t size_log, BitSequenceBuilder *bs);
	void set_time(uint *time, size_t size_time, BitSequenceBuilder *bs);
	
	void set_nodes(uint nodes);
	void set_changes(uint changes);
	void set_maxtime(uint maxtime);
	void set_typegraph(TypeGraph typeg){typegraph_ = typeg;}
	
	void save(ofstream &fp);
	static TemporalGraphLog* load(ifstream &fp);
	
	WaveletKdMatrix *get_log() {return this->log;};

	uint get_changes() {return this->changes;}
	size_t getSize() { 
    fprintf(stderr, "S size: %lu\n", this->log->getSize());
    fprintf(stderr, "B size: %lu\n", this->time->getSize());
    return this->log->getSize() + this->time->getSize() + sizeof(TemporalGraphLog);
  }
	size_t pos_time(size_t i) const;
	
	uint getMaxtime() { return maxtime;}
	
	size_t snapshot(uint t) const;
	
	void direct_point(uint node, uint t, uint *res) const;
	void direct_weak(uint node, uint tstart, uint tend, uint *res) const;
	void direct_strong(uint node, uint tstart, uint tend, uint *res) const;	
	
	void reverse_point(uint node, uint t, uint *res) const;
	void reverse_weak(uint node, uint tstart, uint tend, uint *res) const;
	void reverse_strong(uint node, uint tstart, uint tend, uint *res) const;
	
        
        int edge_point(uint u, uint v, uint t);
        int edge_weak(uint u, uint v, uint tstart, uint tend);
        int edge_strong(uint u, uint v, uint tstart, uint tend);
        
        int edge_next(uint u, uint v, uint t);
        

    size_t change_point(uint t);
    size_t change_interval(uint ts, uint te);
    size_t actived_point(uint t);
    size_t actived_interval(uint ts, uint te);
    size_t deactived_point(uint t);
    size_t deactived_interval(uint ts, uint te);

protected:
	
	void direct_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const;
	void reverse_interval(uint node, uint tstart, uint tend, uint semantic, uint *res) const;
	
        int edge_interval(uint u, uint v, uint tstart, uint tend, uint semantic);
        


        // point contact graphs
        void direct_interval_pg(uint node, uint tstart, uint tend, uint *res) const;
        void reverse_interval_pg(uint node, uint tstart, uint tend, uint *res) const;
        int edge_interval_pg(uint u, uint v, uint tstart, uint tend) const;
        int edge_next_pg(uint u, uint v, uint t);

        size_t snapshot_pg(uint t) const;

        //size_t change_interval_pg(uint ts, uint te); //do not need an upgrade

        size_t actived_interval_pg(uint ts, uint te);
        size_t deactived_interval_pg(uint ts, uint te);

        uint nodes;
        uint changes;
        uint maxtime;
        WaveletKdMatrix *log;
        BitSequence *time;

        size_t size_log;
        size_t size_time;
		
		TypeGraph typegraph_;
		
		
		
};

#endif
