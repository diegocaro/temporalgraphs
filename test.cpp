#include <sys/types.h>
#include "tgl.h"
#include "debug.h"
//#include "interleave.h"

struct adjlog {
        uint nodes; //number of nodes from [0 ... nodes -1]
        uint changes; //total number of changes
        uint maxtime; //maximum time in the dataset from [0 ... maxtime-1]

        uint size_log; //size of the log
        usym *log; //including time and edges

        uint *time; // bitmap of time in nodes
        uint size_time;
};

// read temporal graph and populate adjlog
void read(struct adjlog *l) {
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
	
	l->time = (uint *) malloc((l->size_time/W + 1) * sizeof(uint));
	l->log = (usym *) malloc(l->size_log * sizeof(usym));
  
	
	uint bitpos=0;
	uint i;
	t = 0;
  
	//u_long *p;
	//p = l->log;
  uint k=0;
	while(EOF != scanf("%u %u %u %u", &a[0], &a[1], &a[2], &a[3])) {
		//*p++ = in1(a[0], a[1]);
    l->log[k].x = a[0];
    l->log[k].y = a[1];    
    k++;
    
		if (t != a[2]) {
			//put time mark
			for( i = t; i < a[2]; i++) {
				//printf("setting in pos %u\n", bitpos);
				bitset(l->time, bitpos);
				bitpos++;
			}
		}
		
		t = a[2];
		bitpos++;
	}
	bitset(l->time, bitpos);
	
}


int main(int argc, char *argv[]) {
	ifstream file;
	TemporalGraphLog *tgl;
	struct adjlog tg;
	
	WaveletKdMatrix *wt;
	
	INFO("Loading input graph...");
	read(&tg);
	
	LOG("Loading temporalgraph in file '%s'", argv[1]);
	file.open(argv[1], ios::binary);
	tgl = TemporalGraphLog::load(file);
	file.close();
	
	wt = tgl->get_log();
	
	
	printf("input changes: %u\n", tg.changes);
	printf("tgl changes: %u\n", tgl->get_changes());
	//printf("depth: %u\n", wt->getDepth());
	assert(tgl->get_changes() == tg.changes);
	// 
	
	
	printf("position of time 0: %lu\n", tgl->pos_time(0));
	printf("position of time 1: %lu\n", tgl->pos_time(1));
	printf("position of time 5: %lu\n", tgl->pos_time(5));
	printf("position of time 7: %lu\n", tgl->pos_time(7));
		printf("position of time 19: %lu\n", tgl->pos_time(19));
		printf("position of time 20: %lu\n", tgl->pos_time(20));
	 for( uint i = 0; i < tg.changes; i++) {
		  	assert(wt->access(i) == tg.log[i]);
			 }
	// 
	
	uint start, end;
	//uint i;
	start = 0;
	end = tgl->pos_time(0);
	/*
	uint x,y;
	usym z;
	for(i = start; i < end; i++) {
		z = wt->access(i);
		de1(z, &x, &y);
		printf("%lu: %u -> %u\n", z, x, y);
	}
  

	printf("range report\n");	
	vector<u_long> aa;
	wt->range_report(start, end, aa);
	//wt->range_report(start, end, 0, 1UL << 32, aa);
	for(i = 0; i < aa.size(); i+=2) {
		de1(aa[i], &x, &y);
		printf("%lu (%u,%u): %lu\n",aa[i],x,y,aa[i+1]);
	}
	
	
	printf("axis report\n");
	vector<u_long> bb;
	wt->range_axis_report(start, end, 1, 0UL, bb);
	for(i = 0; i < bb.size(); i+=2) {
		de1(bb[i], &x, &y);
		printf("%lu (%u,%u): %lu\n",bb[i],x,y,bb[i+1]);
	}
	
	
	printf("directo neighbors\n");
	bb.clear();
	tgl->direct_point(0, 10, bb);
	for(i = 0; i < bb.size(); i+=2) {
		de1(bb[i], &x, &y);
		printf("%lu (%u,%u): %lu\n",bb[i],x,y,bb[i+1]);
	}
	
	
	printf("reversp neighbors\n");
	bb.clear();
	tgl->reverse_point(0, 10, bb);
	for(i = 0; i < bb.size(); i+=2) {
		de1(bb[i], &x, &y);
		printf("%lu (%u,%u): %lu\n",bb[i],x,y,bb[i+1]);
	}
*/	
	return 0;
}