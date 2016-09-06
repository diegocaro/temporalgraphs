/*
 * PRBCompactQtreeHeight.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: diegocaro
 */

#include "PRBCompactQtree.h"
#include "utils.h"
#include "debug.h"

#include <algorithm> //sort
#include <vector>
#include <queue>
#include <climits>

namespace cqtree_static {

PRBCompactQtree::PRBCompactQtree(vector<Point<uint> > &vp,
		BitSequenceBuilder *bs, BitSequenceBuilder *bb, int k1, int k2,
		int max_level_k1, int max_levels_ki) :
		k1_(k1), k2_(k2) {

	__setdefaultvalues();  //set default values for this class

	//preconditions
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
	LOG("resto1: %u\n",maxval/max_k1+1);

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
			//virtual_depth_.push_back(mylog(k1_,m+1));
		}

		for (int i = 0; i < levels_k2_; i++) {
			m = m / k2_;
			nk_.push_back(m);
			//virtual_depth_.push_back(mylog(k2_,m+1));
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
		LOG("ki_: %d\n",ki_); LOG("l/2: %d\n", levels_ki_); LOG("max_ki: %d",max_ki); LOG("resto2: %u\n", (maxval/max_k1)/max_ki);

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

		LOG("cdim: %d\n",cdim);
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
			//virtual_depth_.push_back(mylog(k1_,m));
		}

		for (int i = 0; i < levels_k2_; i++) {
			m = m / k2_;
			nk_.push_back(m);
			//   virtual_depth_.push_back(mylog(k2_,m));
		}

		for (int i = 0; i < levels_ki_; i++) {
			if (i % 2 == 0) {
				m = m / ki_;
			}
			nk_.push_back(m);
			//virtual_depth_.push_back(mylog(ki_,m));
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

	LOG("depth: %d\n", depth_); LOG("levelsk2: %d\n", levels_k2_);
	k_.insert(k_.begin(), levels_k1_, k1);
	k_.insert(k_.end(), levels_k2_, k2);
	k_.insert(k_.end(), levels_ki_, ki_);

	max_children_ = 0;
	for (int i = 0; i < depth_; i++) {
		//children_.push_back((uint) pow((double) k_[i], (double) pmax_dim.num_dims()));
		children_.push_back(mypow(k_[i], dims_[i]));

		max_children_ = max(max_children_, children_[i]);

		LOG("children[%d]: %d\n",i, children_[i]); LOG("dims[%d]: %d\n",i,dims_[i]); LOG("rangedim_by_level[%d]: [%d,%d)\n",i,rangedim_by_level_[i].first,rangedim_by_level_[i].second);
		//printf("virtual_depth_[%d]: %d\n",i, virtual_depth_[i]);
		LOG("nk_[%d]: %d\n",i, nk_[i]);
	}

	LOG("depth: %d\n",depth_);

	//childs_ = (uint) pow((double) _K, (double) pmax_dim.num_dims());
	//maxvalue_ = (uint) pow((double) _K, (double) depth_);
	//maxvalue_ = mypow(k1_,levels_k1_) * mypow(k2_,levels_k2_);
	LOG("max value: %u\n",maxvalue_);

	items_ = vp.size();

	get_stats(vp);

	size_t treebits = 0;
	for (int i = 0; i < depth_; i++) {
		printf("T[%d]: %lu\n", i, nodes_by_level_[i]);
		treebits += nodes_by_level_[i];
	}
	printf("Total size T: %lu\n", treebits);

	size_t leafbits = 0;
	for (int i = 0; i < depth_; i++) {
		printf("B[%d]: %lu\n", i, grayblack_[i]);
		leafbits += grayblack_[i];
	}
	printf("Total size B: %lu\n", leafbits);

	for (int i = 0; i < depth_; i++) {
		printf("leaves at level %d: %lu\n", i, leaves_per_level_[i]);
	}

	create(vp, bs, bb);
}

// this method uses counting sort
void PRBCompactQtree::get_stats(std::vector<Point<uint> > &vp) {
	struct Node z;
	queue<struct Node> q;
	size_t r[max_children_ + 1];

	z.level = 0;
	z.lo = 0;
	z.hi = vp.size();
	q.push(z);

	grayblack_.insert(grayblack_.begin(), depth_, 0);

	nodes_by_level_.insert(nodes_by_level_.begin(), depth_, 0);

	leaves_per_level_.insert(leaves_per_level_.begin(), depth_, 0);

	size_t start[max_children_ + 1];
	size_t end[max_children_ + 1];

	//int ktreenodes=0;
	while (!q.empty()) {
		z = q.front();
		q.pop();

		r[0] = z.lo;
		r[children_[z.level]] = z.hi;

		nodes_by_level_[z.level] += children_[z.level];

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
				fprintf(stderr, "Progress step1: %.2f\r",
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

				if (z.level < depth_ - 1) {
					if (r[j] - r[j - 1] == 1) { //was F
						// printf("level %u (bits) cut here %u %u\n", t.l, t.lo, t.hi);
						//printf("B set bit %u\n", setbits);
						// do nothing :D
						leaves_per_level_[z.level] += 1;
					} else {
						//printf("set bit %u\n", (ktreenodes-1)*childs_ + j-1);
						q.push(t);
					}

					// number of nodes with one or more leaves
					grayblack_[z.level] += 1;

				}

			}
		}
	}
}

void PRBCompactQtree::create(const std::vector<Point<uint> > &vp,
		BitSequenceBuilder *bs, BitSequenceBuilder *bb) {
	struct Node z;
	queue<struct Node> q;
	size_t r[max_children_ + 1];

	//int leaves[depth_];
	//for(int i=0; i < depth_; i++) leaves[i]=0;
	//int childs[depth_];
	//for(int i=0; i < depth_; i++) childs[i]=0;

	z.level = 0;
	z.lo = 0;
	z.hi = vp.size();

	q.push(z);

	//ulong ktreenodes = 0;

	vector<uint*> treebits;
	for (int i = 0; i < depth_; i++) {
		treebits.push_back(new uint[nodes_by_level_[i] / W + 1]);
		fill_n(treebits[i], nodes_by_level_[i] / W + 1, 0); //fill with zeroes
	}

	vector<uint*> leafbits;
	for (int i = 0; i < depth_; i++) {
		leafbits.push_back(new uint[grayblack_[i] / W + 1]);
		fill_n(leafbits[i], grayblack_[i] / W + 1, 0); //fill with zeroes
	}

	vector<size_t> setbits;
	setbits.insert(setbits.begin(), depth_, 0);

	vector<size_t> curr_nodes;
	curr_nodes.insert(curr_nodes.begin(), depth_, 0);

	vector<size_t> curr_pos;
	curr_pos.insert(curr_pos.begin(), depth_, 0);

	leaves_ = new Array**[depth_];
	uint maxval, maxvalb;
	for (int i = 0; i < depth_ - 1; i++) {
		maxval = nk_[i];

		//printf("creating %lu leaves at level %d\n",leaves_per_level_[i],i);
		leaves_[i] = new Array*[num_dims_];

		if (false == is_interleaved_) {

			for (int j = 0; j < num_dims_; j++) {

				leaves_[i][j] = new Array(leaves_per_level_[i], maxval - 1);
			}
		} else {

			if (i == 0) {
				maxvalb = maxvalue_;
			} else {
				maxvalb = nk_[i - 1];
			}

			//k1 and k2 levels
			//all dimensions have the same remaining info
			if (i < levels_k1_ + levels_k2_) {
				for (int j = 0; j < num_dims_; j++) {
					leaves_[i][j] = new Array(leaves_per_level_[i], maxval - 1);
				}
			} else {
				if (rangedim_by_level_[i].first == 0) {
					//less info on first levels
					//if (i != depth_-2) {
					for (int j = rangedim_by_level_[i].first;
							j < rangedim_by_level_[i].second; j++) {
						leaves_[i][j] = new Array(leaves_per_level_[i],
								maxval - 1);
					}
					//} else {
					//    for(int j=rangedim_by_level_[i].first; j < rangedim_by_level_[i].second; j++) {
					//                        leaves_[i][j] = new Array(0UL,0);
					//                     }
					//}

					for (int j = rangedim_by_level_[i].second; j < num_dims_;
							j++) {
						leaves_[i][j] = new Array(leaves_per_level_[i],
								maxvalb - 1);
					}
				} else {
					//all dimensions have the same remaining info
					for (int j = 0; j < num_dims_; j++) {
						leaves_[i][j] = new Array(leaves_per_level_[i],
								maxval - 1);
					}
				}
			}
		}

	}

	size_t start[max_children_ + 1];
	size_t end[max_children_ + 1];
	while (!q.empty()) {
		z = q.front();
		q.pop();

		//ktreenodes += 1;

		r[0] = z.lo;
		r[children_[z.level]] = z.hi;

		//for (int j = 1; j < children_[z.level]; j++)
		//    //r[j] = rank(vp, j, depth_- z.level - 1, z.lo, z.hi - 1);
		//    r[j] = rank(vp, j, z.level, z.lo, z.hi - 1);

		for (int i = 0; i < children_[z.level] + 1; i++) {
			start[i] = 0;
		}

		// Count.
		for (size_t i = z.lo; i < z.hi; ++i) {
			if (i % 1000000 == 0)
				fprintf(stderr, "Progress step2: %.2f\r",
						1.0 * ((z.level) * vp.size() + i) / (depth_ * vp.size())
								* 100);
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
		// we do not need to sort because the input is sorted in the last step

		//printf("r: %d %d %d %d %d\n", r[0], r[1], r[2], r[3], r[4]);

		for (int j = 1; j < children_[z.level] + 1; j++) {
			if (r[j] - r[j - 1] > 0) {
				struct Node t;
				t.lo = r[j - 1];

				t.hi = r[j];

				t.level = z.level + 1;

				if (z.level < depth_ - 1) {

					if (r[j] - r[j - 1] == 1) { //was F
						//black node found!

						// printf("level %u (bits) cut here %u %u\n", t.l, t.lo, t.hi);
						//printf("B set bit %u\n", setbits);

						//printf("set bit B_[%d] %lu\n", z.level, setbits[z.level]);
						cds_utils::bitset(leafbits[z.level], setbits[z.level]);

						Point<uint> p(vp[r[j - 1]]);
						uint maxval = nk_[z.level];

						if (false == is_interleaved_) {
							//printf("saving values:");
							for (int i = 0; i < num_dims_; i++) {
								leaves_[z.level][i]->setField(curr_pos[z.level],
										p[i] % maxval);
								//printf(" %u", p[i] % maxval);
							}
							//printf("\n");
						} else {
							uint maxvalb;
							if (z.level == 0) {
								maxvalb = maxvalue_;
							} else {
								maxvalb = nk_[z.level - 1];
							}

							//k1 and k2 levels
							if (z.level < levels_k1_ + levels_k2_) {
								for (int i = 0; i < num_dims_; i++) {
									// printf("level %d storing %u (dima %d) %d\n",z.level, p[i] % maxval,i,p[i]);
									leaves_[z.level][i]->setField(
											curr_pos[z.level], p[i] % maxval);
								}
							} else {

								if (0 == rangedim_by_level_[z.level].first) {
									//printf("range: [%d,%d)\n",rangedim_by_level_[z.level].first,rangedim_by_level_[z.level].second);

									//less info on first levels
									if (z.level != depth_ - 2) {
										for (int i = 0;
												i
														< rangedim_by_level_[z.level].second;
												i++) {
//                                  printf("level %d storing %u (dimb %d) %d\n",z.level, p[i] % maxval,i,p[i]);
											leaves_[z.level][i]->setField(
													curr_pos[z.level],
													p[i] % maxval);
										}
									}

									for (int i =
											rangedim_by_level_[z.level].second;
											i < num_dims_; i++) {
//                                  printf("level %d storing %u (dimc %d) %d\n",z.level, p[i] % maxvalb,i,p[i]);
										leaves_[z.level][i]->setField(
												curr_pos[z.level],
												p[i] % maxvalb);
									}
								} else {
									for (int i = 0; i < num_dims_; i++) {
//                                  printf("level %d storing %u (dimd %d) %d\n",z.level, p[i] % maxval,i,p[i]);
										leaves_[z.level][i]->setField(
												curr_pos[z.level],
												p[i] % maxval);
									}

								}
							}

						}

						curr_pos[z.level] += 1;
					} else {

						q.push(t);
					}
				}
				//printf("set bit T %lu\n", (ktreenodes-1)*childs_ + j-1);
//                  printf("set bit T_[%d] %lu\n", z.level, curr_nodes[z.level] + j - 1);
				cds_utils::bitset(treebits[z.level],
						curr_nodes[z.level] + j - 1);
				setbits[z.level] += 1;
			}
		}

		curr_nodes[z.level] += children_[z.level];

	}

	//building bitmaps for T and cleaning
	for (int i = 0; i < depth_; i++) {
		T_.push_back(bs->build(treebits[i], nodes_by_level_[i]));
		delete[] treebits[i];
	}

	//building bitmaps for B and cleaning
	for (int i = 0; i < depth_; i++) {
		B_.push_back(bb->build(leafbits[i], grayblack_[i]));
		delete[] leafbits[i];
	}
}

void PRBCompactQtree::all(Point<uint> p, size_t z, int level,
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
		if (level == -1) {
			y = 0;
		} else {
			size_t q = 0;
			q = T_[level]->rank1(z - 1);
			if (B_[level]->access(q) == 1) {
				// node is a leaf
				size_t b = 0;
				b = B_[level]->rank1(q - 1);

				Point<uint> c(p);

				if (false == is_interleaved_) {
					for (int i = 0; i < num_dims_; i++) {
						c[i] = p[i] + leaves_[level][i]->getField(b);
					}
				} else {
					//k1 and k2 levels
					if (level < levels_k1_ + levels_k2_) {
						for (int i = 0; i < num_dims_; i++) {
							c[i] = p[i] + leaves_[level][i]->getField(b);
						}
					} else {

						if (0 == rangedim_by_level_[level].first) {

							//less info on first levels
							if (level != depth_ - 2) {
								for (int i = 0;
										i < rangedim_by_level_[level].second;
										i++) {
									c[i] = p[i]
											+ leaves_[level][i]->getField(b);
								}
							}

							for (int i = rangedim_by_level_[level].second;
									i < num_dims_; i++) {
								c[i] = p[i] + leaves_[level][i]->getField(b);
							}
						} else {
							for (int i = 0; i < num_dims_; i++) {
								c[i] = p[i] + leaves_[level][i]->getField(b);
							}

						}
					}
				}

				vpall.push_back(c);
				if (vpall.size() % 100000 == 0) {
					fprintf(stderr, "Progress: %.2f%% \r",
							(float) vpall.size() / items_ * 100);
				}
				return;
			}
			// if this is not a leaf
			y = (q - B_[level]->rank1(q)) * children_[level + 1];

		}

		/*// simple code, for the simple mx quadtree
		 * for (uint u =0 ; u < _K; u++) { c[0] = p[0] + u*nk;
		 for (uint v=0; v < _K; v++) { c[1] = p[1] + v*nk;
		 for( uint a=0; a < _K; a++) { c[2] = p[2] + a*nk;
		 range(nk, from ,to, c, y + u*(1u << 2) + v*(1u << 1) + a, level+1, items);
		 }
		 }
		 }
		 */

		/* or this simple code */
		/*
		 Point<uint> c(p);
		 for(int i=0; i < children_[level+1]; i++) {
		 for(int j=0; j < num_dims_; j++) {
		 //c[j] = p[j] + nk * ((i/(1 << (c.num_dims()-j-1)) )%k_[level+1]);

		 c[j] = p[j] + nk * ((i/mypow(k_[level+1], num_dims_-j-1))%k_[level+1]);
		 }

		 range(nk, from ,to, c, y + i, level+1, items);
		 }
		 */

		uint nk = nk_[level + 1];

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

void PRBCompactQtree::range(Point<uint> &p, size_t z, int level,
		Point<uint> &from, Point<uint> &to, vector<Point<uint> > &vpall,
		size_t &items, bool pushval) {
	if (level != -1) {
		Point<uint> hi(p);
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

				//all are the same
				for (int i = 0; i < num_dims_; i++) {
					hi[i] += nk_[level];
				}

			}

		}
		if (!(p < to && from < hi)) {
			return;
		}
	}

	size_t y = 0;

	if (level == depth_ - 1) {
		count_ops::bitmapT.access();

		if (T_[level]->access(z) == 0)
			return;

		if (pushval)
			vpall.push_back(p);
		items++;

		return;
	}

	else if (level == -1 || T_[level]->access(z) == 1) {
		count_ops::bitmapT.access();

		if (level == -1) {
			y = 0;
		} else {
			size_t q = 0;

			count_ops::bitmapT.rank();

			q = T_[level]->rank1(z - 1);

			count_ops::bitmapB.access();

			if (B_[level]->access(q) == 1) {
				// node is a leaf
				size_t b = 0;

				count_ops::bitmapB.rank();

				b = B_[level]->rank1(q - 1);

				Point<uint> c(p);

				if (false == is_interleaved_) {
					for (int i = 0; i < num_dims_; i++) {
						c[i] = p[i] + leaves_[level][i]->getField(b);
					}
				} else {
					//k1 and k2 levels
					if (level < levels_k1_ + levels_k2_) {
						for (int i = 0; i < num_dims_; i++) {
							c[i] = p[i] + leaves_[level][i]->getField(b);
						}
					} else {
						if (0 == rangedim_by_level_[level].first) {
							//less info on first levels
							if (level != depth_ - 2) {
								for (int i = 0;
										i < rangedim_by_level_[level].second;
										i++) {
									c[i] = p[i]
											+ leaves_[level][i]->getField(b);
								}
							}
							for (int i = rangedim_by_level_[level].second;
									i < num_dims_; i++) {
								c[i] = p[i] + leaves_[level][i]->getField(b);
							}
						} else {
							for (int i = 0; i < num_dims_; i++) {
								c[i] = p[i] + leaves_[level][i]->getField(b);
							}
						}
					}
				}

				if (from <= c && c < to) {
					if (pushval)
						vpall.push_back(c);
					items++;
				}
				return;
			}
			// if this is not a leaf
			count_ops::bitmapB.rank();
			y = (q - B_[level]->rank1(q)) * children_[level + 1];
		}

		uint nk = nk_[level + 1];

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

void PRBCompactQtree::stats_space() const {
	size_t treebits = 0;
	size_t leafbits = 0;
	size_t arraysize = 0;
	printf(" l  k  d  k^d\t");
	printf("%10s\t%10s\t%8s\t%10s\t%8s\t", "|T|", "T.1s", "T.1s/|T|", "T.bytes",
			"T.cratio");
	printf("%10s\t%10s\t%8s\t%10s\t%8s\t", "|B|", "B.1s", "B.1s/|B|", "B.bytes",
			"B.cratio");
	printf("%10s\n", "A.bytes");
	for (int i = 0; i < depth_; i++) {
		printf("%2d %2d %2d %3d\t", i, k_[i], dims_[i], children_[i]);
		printf("%10lu\t%10lu\t%8.2f\t%10lu\t%8.2f\t", T_[i]->getLength(),
				T_[i]->countOnes(),
				1.0 * T_[i]->countOnes() / T_[i]->getLength(), T_[i]->getSize(),
				1.0 * T_[i]->getSize() * 8 / T_[i]->getLength());
		printf("%10lu\t%10lu\t%8.2f\t%10lu\t%8.2f\t", B_[i]->getLength(),
				B_[i]->countOnes(),
				1.0 * B_[i]->countOnes() / B_[i]->getLength(), B_[i]->getSize(),
				1.0 * B_[i]->getSize() * 8 / B_[i]->getLength());

		size_t A = 0;
		for (int j = 0; i != depth_ - 1 && j < num_dims_; j++) {
			A += leaves_[i][j]->getSize();
		}
		printf("%10lu\n", A);

		treebits += T_[i]->getSize();
		leafbits += B_[i]->getSize();
		arraysize += A;
	}
	printf("T space (MBytes): %0.1f\n", 1.0 * treebits / 1024 / 1024);
	printf("B space (MBytes): %0.1f\n", 1.0 * leafbits / 1024 / 1024);
	printf("A space (MBytes): %0.1f\n", 1.0 * arraysize / 1024 / 1024);
	printf("Total space (MBytes): %0.1f\n",
			1.0 * treebits / 1024 / 1024 + 1.0 * leafbits / 1024 / 1024
					+ 1.0 * arraysize / 1024 / 1024);
}

PRBCompactQtree::PRBCompactQtree(ifstream & f) {
	uint type = loadValue<uint>(f);
	// TODO:throw an exception!
	if (type != PRQBLK_SAV) {
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

	loadVector(f, grayblack_);
	loadVector(f, leaves_per_level_);
	loadVector(f, kpower_per_level_dim_);

	for (int i = 0; i < depth_; i++) {
		T_.push_back(NewBitSequence::load(f));
	}

	for (int i = 0; i < depth_; i++) {
		B_.push_back(NewBitSequence::load(f));
	}

	leaves_ = new Array**[depth_];
	for (int i = 0; i < depth_ - 1; i++) {
		leaves_[i] = new Array*[num_dims_];
		for (int j = 0; j < num_dims_; j++) {
			leaves_[i][j] = new Array(f);
		}
	}

}

void PRBCompactQtree::save(ofstream &f) const {
	uint wr = PRQBLK_SAV;
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

	saveVector(f, grayblack_);
	saveVector(f, leaves_per_level_);
	saveVector(f, kpower_per_level_dim_);

	for (int i = 0; i < depth_; i++) {
		T_[i]->save(f);
	}

	for (int i = 0; i < depth_; i++) {
		B_[i]->save(f);
	}

	for (int i = 0; i < depth_ - 1; i++) {
		for (int j = 0; j < num_dims_; j++) {
			leaves_[i][j]->save(f);
		}
	}
}

void PRBCompactQtree::print_leaves() {
	//Point<uint> p(num_dims_);
	//print_leaves(p, -1, -1);

	for (int i = 0; i < depth_; i++) {
		for (size_t j = 0; j < leaves_per_level_[i]; j++) {
			printf("%d", i);
			for (int k = 0; k < num_dims_; k++) {
				printf("\t%d", leaves_[i][k]->getField(j));
			}
			printf("\n");
		}
	}
}

} /* namespace cqtree_static */

