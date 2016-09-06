#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <getopt.h>
#include <cstring>

#include <map>
#include <vector>
#include <algorithm>

#include "debug.h"
#include "symbols.h"
#include "tgl.h"



struct infolog {
	uint nodes;
	uint changes;
	uint maxtime;
};

struct adjlog {
	uint nodes; //number of nodes from [0 ... nodes -1]
	uint changes; //total number of changes
	uint maxtime; //maximum time in the dataset from [0 ... maxtime-1]

	uint size_log; //size of the log
	usym *log; //including time and edges

	uint *time; // bitmap of time in nodes
	uint size_time;
};


enum bitseq {
	RG, R3, SD
};



struct opts {
	enum bitseq bs; //bits for wavelet tree
	enum bitseq bb; //bits for B bitmap
	char *outfile;
   enum TypeGraph typegraph;
};


// read temporal graph and populate adjlog
void read_stdin(struct adjlog *l) {
	uint nodes, changes, maxtime;
	uint a[4];
	uint t;
	scanf("%u %u %u", &nodes, &changes, &maxtime);
	
	l->nodes = nodes;
	l->changes= changes;
	l->maxtime = maxtime;
	l->size_log = changes;
	l->size_time = maxtime + changes; // in bits
	LOG("Nodes:\t%u", l->nodes);
	LOG("Changes:\t%u", l->changes);
	LOG("Maxtime:\t%u", l->maxtime);
	
	LOG("space for log:\t%.2lf MBytes (%lu bytes)", (double)l->size_log*sizeof(u_long)/1024/1024, l->size_log*sizeof(u_long));
	
	LOG("space for time:\t%.2lf MBytes (%lu bytes)", (double)(l->size_time/W+1)*sizeof(uint)/1024/1024, l->size_time/W*sizeof(uint));
	
	l->time = (uint *) calloc((l->size_time/W + 1), sizeof(uint));
	l->log = (usym *) malloc(l->size_log * sizeof(usym));
	
	INFO("Memory acquiered");
	
	//	printf("int1: %X\n", in1((uint)1, (uint)1));
	
	uint bitpos=0;
	uint i;
	t = 0;
	//u_long *p;
	//p = l->log;
	uint k=0;
	
#ifdef DEBUG
	uint changes_read=0;
#endif
	
	while(EOF != scanf("%u %u %u %u", &a[0], &a[1], &a[2], &a[3])) {
		//*p++ = in1(a[0], a[1]);
		l->log[k].x = a[0];
		l->log[k].y = a[1];    
		k++;
    
		if (t != a[2]) {
			//put time mark
			for( i = t; i < a[2]; i++) {
				//printf("setting in pos %u\n", bitpos);
				cds_utils::bitset(l->time, bitpos);
				bitpos++;
			}
		}
		
		t = a[2];
		bitpos++;
		
#ifdef DEBUG
		if(changes_read++%100000==0) fprintf(stderr,"Progress: %.2f%%\r", (float)changes_read/changes*100);
#endif
	}
	cds_utils::bitset(l->time, bitpos);
	
}




class Change {
public:
	uint t;
	uint u;
	uint v;
	
	Change() {
		
	}
	
	Change(const Change& rhs) {
		u = rhs.u; v = rhs.v; t = rhs.t;
	}
	
	bool operator<(const Change &rhs) const {
		if (t<rhs.t) return true;
		
		if (t == rhs.t) {
			if (u<rhs.u) return true;
			if (u == rhs.u) return (v<rhs.v);
		}
		
		return false;
	}

    bool operator==(const Change &rhs) const {
        if (t == rhs.t && v == rhs.v && u == rhs.u) return true;

        return false;
    }
};


bool cmpChange(const Change &lhs,const Change &rhs) {
	if (lhs.t<rhs.t) return true;
	
	if (lhs.t == rhs.t) {
		if (lhs.u<rhs.u) return true;
		if (lhs.u == rhs.u) return (lhs.v<rhs.v);
	}
	
	return false;
}


// read temporal graph from contacts
void read_contacts(struct opts &opts,struct adjlog *l) {
	uint nodes, edges, lifetime, contacts;
		uint u,v,a,b;

  	vector<Change> btable;

		scanf("%u %u %u %u", &nodes, &edges, &lifetime, &contacts);

    btable.reserve(2*contacts);
		uint c_read = 0;

  	Change c;
    uint changes = 0;
		while( EOF != scanf("%u %u %u %u", &u, &v, &a, &b)) {
			c_read++;
			if(c_read%500000==0) fprintf(stderr, "Processing %.1f%%\r", (float)c_read/contacts*100);
			c.u = u;
			c.v = v;
      c.t = a;
			btable.push_back(c);
      changes++;


      if (opts.typegraph == kPoint) continue;

      if ( opts.typegraph == kGrowth && b == lifetime-1) continue;

      changes++;
      c.t = b;
			btable.push_back(c);
		}
		fprintf(stderr, "Processing %.1f%%\r", (float)c_read/contacts*100);
		assert(c_read == contacts);

    // shrink_to_fit old
    //btable.resize(changes); 
    btable.shrink_to_fit();

//		uint changes = 2*contacts;

		l->nodes = nodes;
		l->changes= changes;
		l->maxtime = lifetime;

		l->size_log = changes; 
		l->size_time = lifetime + changes; 
		LOG("Nodes:\t%u", l->nodes);
		LOG("Changes:\t%u", l->changes);
		LOG("Maxtime:\t%u", l->maxtime);

		LOG("space for log:\t%.2lf MBytes (%lu bytes)", (double)l->size_log*sizeof(usym)/1024/1024, l->size_log*sizeof(usym));

		LOG("space for time:\t%.2lf MBytes (%lu bytes)", (double)(l->size_time/W+1)*sizeof(uint)/1024/1024, l->size_time/W*sizeof(uint));

		l->time = (uint *) calloc((l->size_time/W + 1), sizeof(uint));
		l->log = (usym *) malloc(l->size_log * sizeof(usym));

		INFO("Memory acquired");
    
    fprintf(stderr,"Sorting...\n");
    sort(btable.begin(), btable.end(), cmpChange);
    
  	vector<Change>::iterator it;
  	vector<Change>::iterator itlow;
    Change vlow;
    
    usym s;
    uint p=0;
		for(uint i=0; i < lifetime; i++) {
  		vlow.u = 0;
  		vlow.v = 0;
  		vlow.t = i;
      
  		itlow = lower_bound (btable.begin(), btable.end(), vlow, cmpChange);
      
  		for( it = itlow; it->t == i; ++it) {      
        s.x = it->u;
        s.y = it->v;
        
        l->log[p] = s;
        
        p++; 
      }

			cds_utils::bitset(l->time, i+p);
    }
    
    assert(p == changes);
		//l->size_log = p; //actual size (could be less than lenS)
		//l->size_time = lifetime + p; // in bits, actual value
  	btable.clear();

}

void create_index(TemporalGraphLog &tgl, struct adjlog *adjlog, struct opts *opts) {
	BitSequenceBuilder *bs;
	BitSequenceBuilder *bb;
	
	tgl.set_nodes(adjlog->nodes);
	tgl.set_changes(adjlog->changes);
	tgl.set_maxtime(adjlog->maxtime);
	tgl.set_typegraph(opts->typegraph);
	
	switch(opts->bs) {
		case RG:
		bs = new BitSequenceBuilderRG(20); // by default, 5% of extra space for bitmaps
		break;
		case R3:
		bs = new BitSequenceBuilderRRR(32); // DEFAULT_SAMPLING for RRR is 32 
		break;
		case SD:
		bs = new BitSequenceBuilderSDArray();
		break;
	}
	
	
	switch(opts->bb) {
		case RG:
		bb = new BitSequenceBuilderRG(20); // by default, 5% of extra space for bitmaps
		break;
		case R3:
		bb = new BitSequenceBuilderRRR(32); // DEFAULT_SAMPLING for RRR is 32 
		break;
		case SD:
		bb = new BitSequenceBuilderSDArray();
		break;
	}
	
	
	
	tgl.set_log(adjlog->log, adjlog->size_log, bs);
	
	tgl.set_time(adjlog->time, adjlog->size_time, bb);
	
}

int readopts(int argc, char **argv, struct opts *opts) {
	int o;
	
	
	// Default options
	opts->bs = RG;
	opts->bb = RG;
	
	opts->typegraph = kInterval;

	while ((o = getopt(argc, argv, "b:c:t:")) != -1) {
		switch (o) {
			case 'b':
			if(strcmp(optarg, "RG")==0) {
				INFO("Using RG for wavelet matrix");
				opts->bs = RG;
			}
			else if(strcmp(optarg, "RRR")==0) {
				INFO("Using RRR for wavelet matrix");
				opts->bs = R3;
			}
			else if(strcmp(optarg, "SD")==0) {
				INFO("Using SDarray for wavelet matrix");
				opts->bs = SD;
			}
			break;
			
			case 'c':
			if(strcmp(optarg, "RG")==0) {
				INFO("Using RG for bitmap B");
				opts->bb = RG;
			}
			else if(strcmp(optarg, "RRR")==0) {
				INFO("Using RRR for bitmap B");
				opts->bb = R3;
			}
			else if(strcmp(optarg, "SD")==0) {
				INFO("Using SDarray for bitmap B");
				opts->bb = SD;
			}
			break;
		case 't':
			if(strcmp(optarg, "I")==0) {
				INFO("Interval-contact Temporal Graph");
				opts->typegraph = kInterval;
			}
			else if(strcmp(optarg, "P")==0) {
				INFO("Point-contact Temporal Graph");
				opts->typegraph = kPoint;
			}
			else if(strcmp(optarg, "G")==0) {
				INFO("Growing Temporal Graph");
				opts->typegraph = kGrowth;
			}
			break;
			
			default: /* '?' */
			break;
		}
	}
	
        if (optind >= argc || (argc-optind) < 1) {
		fprintf(stderr, "%s [-b RG,RRR,SD] [-c RG,RRR,SD] <outputfile> \n", argv[0]);
		fprintf(stderr, "Expected argument after options\n");
	fprintf(stderr, "Expected type of graph (-t):\n");
			fprintf(stderr, "\tI for Interval-contact Temporal Graph\n");
			fprintf(stderr, "\tP for Point-contact Temporal Graph\n");
			fprintf(stderr, "\tG for Growing Temporal Graph\n\n");	

		exit(EXIT_FAILURE);
        }
	
	opts->outfile = argv[optind];
	
	return optind;

}


int main(int argc, char *argv[]) {
	struct adjlog tg;
	struct opts opts;
	int optind;
	
	TemporalGraphLog tgl;
	
	optind = readopts(argc, argv, &opts);
	
	INFO("Loading graph...");
	//read_stdin(&tg);
	read_contacts(opts,&tg);
	
	INFO("Creating index...");
	create_index(tgl, &tg, &opts);
	LOG("Size of index: %lu\n", tgl.getSize());
	
	//LOG("Depth: %u", tgl.get_log()->getDepth());
	
	ofstream file;
	LOG("Saving graph file in '%s'\n", opts.outfile);
	file.open(opts.outfile, ios::binary);
	tgl.save(file);
	file.close();
	
	return 0;
}
