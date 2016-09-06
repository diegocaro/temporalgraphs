/*
 * MXCompactQtreeHeight.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: diegocaro
 */

#include "MXCompactQtree.h"
#include "utils.h"

#include <algorithm> //sort
#include <vector>
#include <queue>
#include <climits>

#include "debug.h"

#include "bitvector.h"

namespace cqtree_static {

MXCompactQtree::MXCompactQtree(vector<Point<uint> > &vp, BitSequenceBuilder *bs,
		int k1, int k2, int max_level_k1, int max_levels_ki) :
		k1_(k1), k2_(k2) {

	__setdefaultvalues();  //set default values for this class

	//preconditions
	assert(k1 >= 2);
	assert(k2 >= 2);
	assert(max_level_k1 >= 0);
	assert(max_levels_ki >= 0);

	if (max_levels_ki > 0) {
		assert(max_levels_ki % 2 == 0); //number of levels interleaved must be even

		is_interleaved_ = true;
		ki_ = 2; //by default k for interleaved section is 2

		levels_ki_ = max_levels_ki;
	}

	Point<uint> pmax_dim(max(vp));
	num_dims_ = pmax_dim.num_dims();

	uint maxval;
	maxval = max(pmax_dim);

	//depth_ = ceil(log((double) maxval) / log((double) _K));

	uint max_k1 = 0;
	//max_k1 = (uint) pow((double) k1, (double) max_level_k1);
	max_k1 = mypow(k1, max_level_k1);

	levels_k1_ = max_level_k1;
	levels_k2_ = 0;
	printf("resto1: %u\n", maxval / max_k1 + 1);

	if (false == is_interleaved_) {

		//levels using k2_ are automatically calculed
		if (max_k1 < maxval) {
			//depth_ += ceil(log((double) maxval/max_k1) / log((double) k2));
			levels_k2_ = mylog(k2, maxval / max_k1 + 1);
		}

		maxvalue_ = mypow(k1_, levels_k1_) * mypow(k2_, levels_k2_);
		//virtual_depth_k2_ = mylog(k2, maxvalue_);
		//printf("virtual_depth_k2_: %d\n",virtual_depth_k2_);

		depth_ = levels_k1_ + levels_k2_;
		dims_.insert(dims_.end(), depth_, num_dims_);
		rangedim_by_level_.insert(rangedim_by_level_.end(), depth_,
				make_pair(0, num_dims_));

		uint m = maxvalue_;
		for (int i = 0; i < levels_k1_; i++) {
			m = m / k1_;
			nk_.push_back(m);
		}

		for (int i = 0; i < levels_k2_; i++) {
			m = m / k2_;
			nk_.push_back(m);
		}

		for (int i = 0; i < levels_k1_; i++) {
			for (int j = 0; j < num_dims_; j++) {
				kpower_per_level_dim_.push_back(mypow(k1_, num_dims_ - j - 1));
			}
		}

		for (int i = 0; i < levels_k2_; i++) {
			for (int j = 0; j < num_dims_; j++) {
				kpower_per_level_dim_.push_back(mypow(k2_, num_dims_ - j - 1));
			}
		}

	} else {
		// k1 must have available levels
		assert(max_k1 < maxval);

		// the number of levels availables must be at least the number of levels for ki_
		int lvl_availables_ki = mylog(ki_, maxval / max_k1 + 1);
		assert(lvl_availables_ki >= levels_ki_ / 2);

		int max_ki = mypow(ki_, levels_ki_ / 2);
		LOG("ki_: %d",ki_);
		LOG("l/2: %d", levels_ki_);
		LOG("max_ki: %d",max_ki); LOG("resto2: %u", (maxval/max_k1)/max_ki);

		levels_k2_ = mylog(k2, (maxval / max_k1) / max_ki + 1);
		LOG("levels for k2: %d\n", levels_k2_);

		maxvalue_ = mypow(k1_, levels_k1_) * mypow(k2_, levels_k2_)
				* mypow(ki_, levels_ki_ / 2);

		depth_ = levels_k1_ + levels_k2_ + levels_ki_;

		dims_.insert(dims_.end(), levels_k1_, num_dims_);
		dims_.insert(dims_.end(), levels_k2_, num_dims_);

		rangedim_by_level_.insert(rangedim_by_level_.end(), levels_k1_,
				make_pair(0, num_dims_));
		rangedim_by_level_.insert(rangedim_by_level_.end(), levels_k2_,
				make_pair(0, num_dims_));

		int cdim = ceil((float) num_dims_ / ki_);
		//int fdim = floor((float)num_dims_/2);

		LOG("cdim: %d",cdim);
		for (int i = 0; i < levels_ki_; i++) {
			if (i % 2 == 0) {
				dims_.push_back(cdim);
				rangedim_by_level_.push_back(make_pair(0, cdim));
			} else {
				dims_.push_back(num_dims_ - cdim);
				rangedim_by_level_.push_back(make_pair(cdim, num_dims_));
			}
		}

		uint m = maxvalue_;
		for (int i = 0; i < levels_k1_; i++) {
			m = m / k1_;
			nk_.push_back(m);
		}

		for (int i = 0; i < levels_k2_; i++) {
			m = m / k2_;
			nk_.push_back(m);
		}

		for (int i = 0; i < levels_ki_; i++) {
			if (i % 2 == 0) {
				m = m / ki_;
			}
			nk_.push_back(m);
		}

		for (int i = 0; i < levels_k1_; i++) {
			for (int j = 0; j < num_dims_; j++) {
				kpower_per_level_dim_.push_back(mypow(k1_, num_dims_ - j - 1));
			}
		}

		for (int i = 0; i < levels_k2_; i++) {
			for (int j = 0; j < num_dims_; j++) {
				kpower_per_level_dim_.push_back(mypow(k2_, num_dims_ - j - 1));
			}
		}

		for (int i = 0; i < levels_ki_; i++) {
			if (i % 2 == 0) {

				for (int j = 0; j < cdim; j++) {
					kpower_per_level_dim_.push_back(mypow(ki_, cdim - j - 1));
				}

				for (int j = cdim; j < num_dims_; j++) {
					kpower_per_level_dim_.push_back(0);
				}
			} else {
				for (int j = 0; j < cdim; j++) {
					kpower_per_level_dim_.push_back(0);
				}

				for (int j = 0; j < num_dims_ - cdim; j++) {
					kpower_per_level_dim_.push_back(
							mypow(ki_, num_dims_ - cdim - j - 1));
				}
			}
		}

	}

	LOG("depth: %d", depth_); LOG("levelsk2: %d", levels_k2_);
	k_.insert(k_.end(), levels_k1_, k1);
	k_.insert(k_.end(), levels_k2_, k2);
	k_.insert(k_.end(), levels_ki_, ki_);

	max_children_ = 0;
	for (int i = 0; i < depth_; i++) {
		//children_.push_back((uint) pow((double) k_[i], (double) pmax_dim.num_dims()));
		children_.push_back(mypow(k_[i], dims_[i]));

		max_children_ = max(max_children_, children_[i]);

		LOG("children[%d]: %d",i, children_[i]); LOG("dims[%d]: %d",i,dims_[i]); LOG("rangedim_by_level[%d]: [%d,%d)",i,rangedim_by_level_[i].first,rangedim_by_level_[i].second);
		//printf("virtual_depth_[%d]: %d\n",i, virtual_depth_[i]);
		LOG("nk_[%d]: %d",i, nk_[i]);
	}

	LOG("depth: %d\n",depth_);

	//childs_ = (uint) pow((double) _K, (double) pmax_dim.num_dims());
	//maxvalue_ = (uint) pow((double) _K, (double) depth_);
	//maxvalue_ = mypow(k1_,levels_k1_) * mypow(k2_,levels_k2_);
	LOG("max value: %u",maxvalue_);

	items_ = vp.size();

	if (NULL != bs) {
		build(vp, bs);
	}

	if (NULL != bs) {
		size_t treebits = 0;
		for (int i = 0; i < depth_; i++) {
			printf("T[%d]: %lu\n", i, nodes_by_level_[i]);
			treebits += nodes_by_level_[i];
		}
		printf("Total size T: %lu\n", treebits);
	}
}

// this method uses counting sort
void MXCompactQtree::build(std::vector<Point<uint> > &vp,
		BitSequenceBuilder *bs) {
	struct Node z;
	queue<struct Node> q;
	size_t r[max_children_ + 1];

	z.level = 0;
	z.lo = 0;
	z.hi = vp.size();

	q.push(z);

	bitvector *bt = new bitvector(children_[0]);

	vector<size_t> curr_nodes;
	curr_nodes.insert(curr_nodes.begin(), depth_, 0);

	int curr_level = 0;

	size_t start[max_children_ + 1];
	size_t end[max_children_ + 1];

	while (!q.empty()) {
		z = q.front();
		q.pop();

		//ktreenodes += 1;
		if (curr_level != z.level) {
			curr_level = z.level;
			T_.push_back(bs->build((uint*) bt->data(), bt->length()));

			delete bt;

			bt = new bitvector(
					T_[curr_level - 1]->countOnes() * children_[curr_level]);
		}

		r[0] = z.lo;
		r[children_[z.level]] = z.hi;

		for (int i = 0; i < children_[z.level] + 1; i++) {
			start[i] = 0;
		}

		// Count.
		for (size_t i = z.lo; i < z.hi; ++i) {
			start[code(vp[i], z.level)] += 1;
		}

		// Compute partial sums.
		size_t sum = z.lo;
		for (int bin = 0; bin < children_[z.level]; ++bin) {
			int tmp = start[bin];
			start[bin] = sum;
			end[bin] = sum;
			sum += tmp;
		}
		start[children_[z.level]] = z.hi;

		for (int i = 1; i < children_[z.level]; i++) {
			r[i] = start[i];
		}

		// Move elements.
		int cur_bin = 0;
		int bin = 0;
		for (size_t i = z.lo; i < z.hi; ++i) {
			if (i % 1000000 == 0)
				fprintf(stderr, "Progress: %.2f\r",
						1.0 * ((z.level) * vp.size() + i) / (depth_ * vp.size())
								* 100);

			// set the bin for position i
			while (i >= start[cur_bin + 1]) {
				++cur_bin;
			}
			if (i < end[cur_bin]) {
				// Element has already been processed.
				continue;
			}

			//move the object i to the correct bin in pos j=end[bin]++
			bin = code(vp[i], z.level);

			while (bin != cur_bin) {
				size_t j = end[bin]++;
				// Swap bin and a[j]
				//printf("Moving i %lu <-> j %lu\n",i,j);
				//Point<uint> tmp (vp[j]);
				//vp[j] = vp[i];
				//vp[i] = tmp;
				std::swap(vp[i], vp[j]);
				bin = code(vp[i], z.level);
			}
			//printf("moving\n");
			//a[i] = bin;
			++end[cur_bin];
		}

		for (int j = 1; j < children_[z.level] + 1; j++) {
			if (r[j] - r[j - 1] > 0) {
				struct Node t;
				t.lo = r[j - 1];

				t.hi = r[j];

				t.level = z.level + 1;

				if (z.level < depth_ - 1)
					q.push(t);

				//printf("set bit T %lu\n", (ktreenodes-1)*childs_ + j-1);
				//printf("set bit T_[%d] %lu\n", z.level, curr_nodes[z.level] + j - 1);
				bt->bitset(curr_nodes[z.level] + j - 1);
			}
		}

		curr_nodes[z.level] += children_[z.level];

	}

	T_.push_back(bs->build((uint*) bt->data(), bt->length()));
	delete bt;

	//updates nodes_by_level_
	for (int i = 0; i < depth_; i++) {
		nodes_by_level_.push_back(curr_nodes[i]);
	}
}

void MXCompactQtree::range(Point<uint> &p, size_t z, int level,
		Point<uint> &from, Point<uint> &to, vector<Point<uint> > &vpall,
		size_t &items, bool pushval) {
	Point<uint> hi(p);
	if (level != -1) {
		if (false == is_interleaved_) {
			hi += nk_[level];
		} else {

			if (rangedim_by_level_[level].first == 0) {

				for (int j = rangedim_by_level_[level].first;
						j < rangedim_by_level_[level].second; j++) {
					hi[j] += nk_[level];
				}

				for (int j = rangedim_by_level_[level].second; j < num_dims_;
						j++) {
					hi[j] += (level == 0) ? maxvalue_ : nk_[level - 1];
				}
			} else {

				hi += nk_[level];
			}

		}

//    if (  to[0] <= p[0] || to[1] <= p[1] || to[2] <= p[2] || to[3] <= p[3] || hi[0]<= from[0] || hi[1] <= from[1] || hi[2] <= from[2] || hi[3] <= from[3]) {
//      return;
//    }

		if (!(p < to && from < hi)) {
			return;
		}
	}

	size_t y = 0;

	if (level == depth_ - 1) {

		count_ops::bitmapT.access();

		if (T_[level]->access(z) == 0)
			return;
		if (pushval) {
			vpall.push_back(p);
		}

		items++;

		return;
	}

	else if (level == -1 || T_[level]->access(z) == 1) {
		if (level == -1)
			y = 0;
		else
			y = T_[level]->rank1(z - 1) * children_[level + 1];

		count_ops::bitmapT.access();
		count_ops::bitmapT.rank();

		uint nk = nk_[level + 1];
		//printf("nk[%d]: %u\n", level+1, nk_[level+1]);

		if (false == is_interleaved_) {
			//uint nk=n/k_[level+1];

			Point<uint> c(p);
			for (int i = 0; i < children_[level + 1]; i++) {
				for (int j = 0; j < num_dims_; j++) {
					c[j] = p[j]
							+ nk
									* ((i
											/ kpower_per_level_dim_[num_dims_
													* (level + 1) + j])
											% k_[level + 1]);
				}

				range(c, y + i, level + 1, from, to, vpall, items, pushval);
			}
		} else {
			for (int i = 0; i < children_[level + 1]; i++) {
				Point<uint> c(p);
				for (int j = rangedim_by_level_[level + 1].first;
						j < rangedim_by_level_[level + 1].second; j++) {
					//printf("updating dim: %d\n", rangedim_by_level_[level+1].first + j);
					//c[rangedim_by_level_[level+1].first+j] += nk * ((i/mypow(k_[level+1], dims_[level+1]-j-1))%k_[level+1]);
					c[j] = p[j]
							+ nk
									* ((i
											/ kpower_per_level_dim_[num_dims_
													* (level + 1) + j])
											% k_[level + 1]);
				}

				range(c, y + i, level + 1, from, to, vpall, items, pushval);
			}
		}
	}
}

void MXCompactQtree::all(Point<uint> p, size_t z, int level,
		vector<Point<uint> > &vpall) {
	size_t y = 0;

	if (level == depth_ - 1) {

		if (T_[level]->access(z) == 0)
			return;
		vpall.push_back(p);

		if (vpall.size() % 100000 == 0) {
			fprintf(stderr, "Progress: %.2f%% \r",
					(float) vpall.size() / items_ * 100);
		}

		return;
	}

	else if (level == -1 || T_[level]->access(z) == 1) {
		if (level == -1)
			y = 0;
		else
			y = T_[level]->rank1(z - 1) * children_[level + 1];

		uint nk = nk_[level + 1];
		//printf("nk[%d]: %u\n", level+1, nk_[level+1]);

		if (false == is_interleaved_) {
			//uint nk=n/k_[level+1];

			Point<uint> c(p);
			for (int i = 0; i < children_[level + 1]; i++) {
				for (int j = 0; j < num_dims_; j++) {
					//c[j] = p[j] + nk * ((i/(1 << (c.num_dims()-j-1)) )%k_[level+1]);

					//c[j] = p[j] + nk * ((i/mypow(k_[level+1], num_dims_-j-1))%k_[level+1]);
					c[j] = p[j]
							+ nk
									* ((i
											/ kpower_per_level_dim_[num_dims_
													* (level + 1) + j])
											% k_[level + 1]);
				}

				all(c, y + i, level + 1, vpall);
			}
		} else {
			for (int i = 0; i < children_[level + 1]; i++) {
				Point<uint> c(p);
				for (int j = rangedim_by_level_[level + 1].first;
						j < rangedim_by_level_[level + 1].second; j++) {
					//printf("updating dim: %d\n", rangedim_by_level_[level+1].first + j);
					//c[rangedim_by_level_[level+1].first+j] += nk * ((i/mypow(k_[level+1], dims_[level+1]-j-1))%k_[level+1]);
					c[j] = p[j]
							+ nk
									* ((i
											/ kpower_per_level_dim_[num_dims_
													* (level + 1) + j])
											% k_[level + 1]);
				}

				all(c, y + i, level + 1, vpall);
			}
		}
	}
}

void MXCompactQtree::stats_space() const {
	size_t treebits = 0;
	printf(" l  k  d  k^d\t");
	printf("%10s\t%10s\t%10s\t%10s\t%s\n", "|T|", "T.1s", "T.1s/|T|", "T.bytes",
			"T.cratio");
	for (int i = 0; i < depth_; i++) {
		printf("%2d %2d %2d %3d\t", i, k_[i], dims_[i], children_[i]);
		printf("%10lu\t%10lu\t%.2f\t%10lu\t%.2f\n", T_[i]->getLength(),
				T_[i]->countOnes(),
				1.0 * T_[i]->countOnes() / T_[i]->getLength(), T_[i]->getSize(),
				1.0 * T_[i]->getSize() * 8 / T_[i]->getLength());
		treebits += T_[i]->getSize();
	}
	printf("T space (MBytes): %0.1f\n", 1.0 * treebits / 1024 / 1024);
}

// load from file
MXCompactQtree::MXCompactQtree(ifstream & f) {
	uint type = loadValue<uint>(f);
	// TODO:throw an exception!
	if (type != MXQDPT_SAV) {
		abort();
	}

	loadValue(f, items_);
	loadValue(f, levels_k1_);
	loadValue(f, levels_k2_);
	loadValue(f, levels_ki_);

	loadValue(f, k1_);
	loadValue(f, k2_);
	loadValue(f, ki_);

	loadValue(f, depth_);
	loadValue(f, maxvalue_);

	loadValue(f, num_dims_);

	loadValue(f, max_children_);
	loadValue(f, is_interleaved_);

	loadVector(f, k_);
	loadVector(f, children_);
	loadVector(f, nk_);
	loadVector(f, rangedim_by_level_);
	loadVector(f, dims_);
	loadVector(f, nodes_by_level_);
	loadVector(f, kpower_per_level_dim_);

	for (int i = 0; i < depth_; i++) {
		T_.push_back(NewBitSequence::load(f));
		//printf("s T_[%d].size() = %lu %p\n", i,T_[i]->getLength(),T_[i]);
	}

}

void MXCompactQtree::save(ofstream &f) const {
	uint wr = MXQDPT_SAV;
	cds_utils::saveValue(f, wr);

	cds_utils::saveValue(f, items_);

	cds_utils::saveValue(f, levels_k1_);
	cds_utils::saveValue(f, levels_k2_);
	cds_utils::saveValue(f, levels_ki_);

	cds_utils::saveValue(f, k1_);
	cds_utils::saveValue(f, k2_);
	cds_utils::saveValue(f, ki_);

	cds_utils::saveValue(f, depth_);
	cds_utils::saveValue(f, maxvalue_);

	cds_utils::saveValue(f, num_dims_);

	cds_utils::saveValue(f, max_children_);
	cds_utils::saveValue(f, is_interleaved_);

	saveVector(f, k_);
	saveVector(f, children_);
	saveVector(f, nk_);
	saveVector(f, rangedim_by_level_);
	saveVector(f, dims_);
	saveVector(f, nodes_by_level_);
	saveVector(f, kpower_per_level_dim_);

	for (int i = 0; i < depth_; i++) {
		T_[i]->save(f);
	}

}

MXCompactQtree::~MXCompactQtree() {
	for (size_t i = 0; i < T_.size(); i++) {
		delete T_[i];
	}
	T_.clear();

	__setdefaultvalues();
}

}
