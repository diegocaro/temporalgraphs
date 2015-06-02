#include <cstdio>
#include "wavelet_matrix.h"
#include "wttest.h"
int main() {
        printf("prev value\n");
       /* uint *p = new uint[12];
        p[0]=7; p[1]= 1; p[2] = 2;p[3]=3;p[4]=9; p[5]=1; p[6]=2;p[7]=3; p[8]=10; p[9]=11, p[10]=1;p[11]=1; p[12]=2;
        WaveletMatrix * wtprev = new WaveletMatrix(p, 12, new BitSequenceBuilderRG(20));
        printf("prev 0,6 de 15: %u", wtprev->prev_value(15, 0, 6));
        */
        
	size_t N;
	cin >> N;
	uint * seq = new uint[N];
	uint * seq2 = new uint[N];
	uint i, j;
	uint a, b;
	
	for(size_t i=0;i<N;i++) {
		cin >> seq[i];
		seq2[i] = seq[i];
	}


	WaveletMatrix * wt1 = new WaveletMatrix(seq2, N, new BitSequenceBuilderRG(20));
	WTest<uint> wttest(seq, N);

	//printf("select 3 2 %u\n", wt1->select(3,2));

	for( size_t i = 0; i < N; i++) {
		
		assert(wttest.access(i) == wt1->access(i));
		//printf("params: %u, %u\n", seq[i], i);
		//printf("%u %u\n", wttest.rank(seq[i], i), wt1->rank(seq[i], i));
		assert(wttest.rank(seq[i], i) ==  wt1->rank(seq[i],i));
		assert(wttest.select(seq[i], 1U) ==  wt1->select(seq[i],1U));
	}
	
	uint rank_n;
	cin >> rank_n;
	for(i = 0; i < rank_n; i++) {
		cin >> a;
		cin >> b;
		//printf("params: %u, %u\n", a, b);
		//printf("%u %u\n", wttest.rank(a, b), wt1->rank(a, b));
		assert(wttest.rank(a, b) ==  wt1->rank(a, b));
	}
	
	uint select_n;
	cin >> select_n;
	for(i = 0; i < select_n; i++) {
		cin >> a;
		cin >> b;
		assert(wttest.select(a, b) ==  wt1->select(a,b));
	}
	
	vector<size_t> res1;
	vector<size_t> res2;
	cin >> select_n;
	for(i = 0; i < select_n; i++) {
		cin >> a;
		wt1->select_all(a, res1);
		wttest.select_all(a,res2);
		
		
		
		//assert(res2.size() == res1.size());
	
		for(j = 0; j < res1.size(); j++) {
			//printf(" %u \n", res1[i]);
			assert(res2[j] == res1[j]);
		}
		
		res1.clear();
		res2.clear();
	}
	
	
	
	
	uint lowvoc, uppvoc;
	uint range_n;
	cin >> range_n;
	vector< pair<uint,size_t> > resa;
	vector< pair<uint,size_t> > resb;
	for(i = 0; i < range_n; i++) {
		cin >> a;
		cin >> b;
		cin >> lowvoc;
		cin >> uppvoc;
		//printf("pos [%u, %u) sigma [%u, %u)\n", a, b, lowvoc, uppvoc);
		wt1->range_report(a, b, lowvoc, uppvoc, resa);
		wttest.range_report(a, b, lowvoc, uppvoc, resb);
		assert(res2.size() == res1.size());
	
//		printf("%u %u\n", res1.size(), res2.size());
		for(j = 0; j < res1.size(); j++) {
//			printf(" %u %u \n", res1[j], res2[j]);
			assert(res2[j] == res1[j]);
		}
		
		res1.clear();
		res2.clear();
	}
	
	uint value_pos_n;
	uint symbol;
	cin >> value_pos_n;
	
	for(i = 0; i < value_pos_n; i++) {
		cin >> symbol;
		cin >> a;
		cin >> b;
		printf("symbol: %u in [%u,%u]\n",symbol,a,b);
		printf("%u %u\n", wt1->next_value_pos(symbol, a, b), wttest.next_value_pos(symbol, a ,b));
		printf("pos [%u, %u) sigma [%u, %u)\n", a, b, lowvoc, uppvoc);
		assert(wt1->next_value_pos(symbol, a, b) == wttest.next_value_pos(symbol, a ,b));
	
	}




	ofstream file;
	file.open("testsave", ios::binary);
	wt1->save(file);
	file.close();
	printf("saved file\n");
	
	ifstream file2;
	WaveletMatrix *wt2;
	
	file2.open("testsave", ios::binary);
	wt2 = WaveletMatrix::load(file2);
	file2.close();

	for( size_t i = 0; i < N; i++) {
		
		assert(wttest.access(i) == wt2->access(i));
		//printf("params: %u, %u\n", seq[i], i);
		//printf("%u %u\n", wttest.rank(seq[i], i), wt1->rank(seq[i], i));
		assert(wttest.rank(seq[i], i) ==  wt2->rank(seq[i],i));
		assert(wttest.select(seq[i], 1U) ==  wt2->select(seq[i],1U));
	}





	return 0;
	
	
	
}
