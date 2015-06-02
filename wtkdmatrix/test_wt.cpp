#include <cstdio>
#include "wavelet_kd_matrix.h"
#include "wtkdtest.h"

int main() {
	

	size_t N;
	cin >> N;
	usym * seq = new usym[N];
	usym * seq2 = new usym[N];
	uint i;
	uint a, b;
	usym t;
	for(size_t i=0;i<N;i++) {
		cin >> seq[i].x;
		cin >> seq[i].y;
		seq2[i] = seq[i];
	}


	WaveletKdMatrix * wt1 = new WaveletKdMatrix(seq2, N, new BitSequenceBuilderRRR(32));
	WTKDTest<usym> wttest(seq, N);

  printf("size of wt1: %lu bytes\n", wt1->getSize());


	for( size_t i = 0; i < N; i++) {
		//t = wttest.access(i);
		//printf("test: {%u, %u}\n", t.x, t.y);

		//t = wt1->access(i);
		//printf("wt: {%u, %u}\n", t.x, t.y);

		assert(wttest.access(i) == wt1->access(i));
		//printf("params: %u, %u\n", seq[i], i);
		//printf("%u %u\n", wttest.rank(seq[i], i), wt1->rank(seq[i], i));

		//assert(wttest.rank(seq[i], i) ==  wt1->rank(seq[i],i));
		//assert(wttest.select(seq[i], 1U) ==  wt1->select(seq[i],1U));
	}
  return 0;
  
	uint rank_n;
	cin >> rank_n;
	for(i = 0; i < rank_n; i++) {
		cin >> t.x;
		cin >> t.y;
		cin >> b;
		//printf("params: %u, %u\n", a, b);
		//printf("%u %u\n", wttest.rank(a, b), wt1->rank(a, b));
		assert(wttest.rank(t, b) ==  wt1->rank(t, b));
	}

	uint select_n;
	cin >> select_n;
	for(i = 0; i < select_n; i++) {
		cin >> t.x;
		cin >> t.y;
		cin >> b;
		//printf("{%u, %u}\n", t.x, t.y);
		//printf("test: %u\n", wttest.select(t, b));
		//printf("wt: %u\n", wt1->select(t, b));
		assert(wttest.select(t, b) ==  wt1->select(t, b));
	}
	
	vector< pair<uint,size_t> > res1;
	vector< pair<uint,size_t> > res2;


	uint axis_n;
	uint axis, node;
	cin >> axis_n;

	for(i = 0; i < axis_n; i++) {
		cin >> axis;
		cin >> node;

		cin >> a; //start
		cin >> b; //end

		wt1->axis(a, b, axis, node, res1);
		//wttest.range(a, b, lowvoc, uppvoc, res2);
	//	assert(res2.size() == res1.size());

		if (axis == 0) printf("Direct ");
		else printf("Reverse ");
		//printf("%u in [%u, %u]:", node, a, b);

		//assert(res1.size() == res2.size());
		for(i = 0; i < res1.size(); i++) {
			//printf(" %u", res1[i]);
		//	assert(res2[i].first == res1[i].first);
		//	assert(res2[i].second == res1[i].second);
		}
		printf("\n");

		res1.clear();
		res2.clear();
	}


	vector< pair<usym, size_t> > ar;
	usym ff, gg;
	ff.x = 0; ff.y = 0;
	gg.x = 7; gg.y = 7;
	wt1->range(0, 12, ff, gg, ar);


	wt1->rankall(0, 12, ar);

	printf("wuuu\n");
	wt1->rankall_slow(0, 12, ar);


	ofstream file;
		file.open("testsave", ios::binary);
		wt1->save(file);
		file.close();
		printf("saved file\n");

		ifstream file2;
		WaveletKdMatrix *wt2;

		file2.open("testsave", ios::binary);
		wt2 = WaveletKdMatrix::load(file2);
		file2.close();
		for( size_t i = 0; i < N; i++) {
			//t = wttest.access(i);
			//printf("test: {%u, %u}\n", t.x, t.y);

			//t = wt1->access(i);
			//printf("wt: {%u, %u}\n", t.x, t.y);

			assert(wttest.access(i) == wt2->access(i));
			//printf("params: %u, %u\n", seq[i], i);
			//printf("%u %u\n", wttest.rank(seq[i], i), wt1->rank(seq[i], i));

			//assert(wttest.rank(seq[i], i) ==  wt1->rank(seq[i],i));
			//assert(wttest.select(seq[i], 1U) ==  wt1->select(seq[i],1U));
		}

	/*

	
	vector<uint> res1;
	vector<uint> res2;
	uint lowvoc, uppvoc;
	uint range_n;
	cin >> range_n;
	
	for(i = 0; i < range_n; i++) {
		cin >> a;
		cin >> b;
		cin >> lowvoc;
		cin >> uppvoc;
		
		wt1->range(a, b, lowvoc, uppvoc, res1);
		wttest.range(a, b, lowvoc, uppvoc, res2);
		assert(res2.size() == res1.size());
	
		for(i = 0; i < res1.size(); i++) {
			//printf(" %u \n", res1[i]);
			assert(res2[i] == res1[i]);
		}
		
		res1.clear();
		res2.clear();
	}

	*/


	return 0;
	
	
	
}
