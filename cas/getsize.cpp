#include <stdio.h>
#include "tgs.h"


int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "%s <struct.tgs>\n", argv[0]);
		return 1;
	}
	
	struct tgs index;
	ifstream f;
	size_t s;
        f.open(argv[1], ios::binary);
	
	if (f.is_open()) {
		tgs_load(&index, f);
        	f.close();
		s = tgs_size(&index);
		
		printf("%lu bytes (%.2lf Mbytes) used by '%s'\n", s, (double)s/1024/1024, argv[1]);
		
	} else {
		fprintf(stderr,"Unable to open '%s'.\n", argv[1]);
		return 1;
	}
	
	return 0;
}