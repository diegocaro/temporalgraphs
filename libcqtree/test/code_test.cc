#include "utils.h"
#include "MXCompactQtree.h"
#include "gtest/gtest.h"

#include <vector>

using namespace std;
using namespace cqtree_utils;
using namespace cqtree_static;
using namespace cds_static;

namespace {

class CodeTest: public ::testing::Test {
protected:
	virtual void SetUp() {
		Point<uint> p(3);
		p[0] = 1;
		p[1] = 2;
		p[2] = 3;

		vp_.push_back(p);
		p[0] = 2;
		p[1] = 4;
		p[2] = 6;

		vp_.push_back(p);

		p[0] = 3;
		p[1] = 5;
		p[2] = 7;

		vp_.push_back(p);

		Point<uint> q(4);
		q[0] = 0;
		q[1] = 0;
		q[2] = 0;
		q[3] = 1;
		vp4_.push_back(q);

		q[0] = 0;
		q[1] = 0;
		q[2] = 2;
		q[3] = 14;
		vp4_.push_back(q);

		q[0] = 0;
		q[1] = 0;
		q[2] = 15;
		q[3] = 16;
		vp4_.push_back(q);

		q[0] = 0;
		q[1] = 0;
		q[2] = 17;
		q[3] = 18;
		vp4_.push_back(q);

		q[0] = 0;
		q[1] = 15;
		q[2] = 0;
		q[3] = 1;
		vp4_.push_back(q);

		q[0] = 15;
		q[1] = 0;
		q[2] = 0;
		q[3] = 15;
		vp4_.push_back(q);

		q[0] = 15;
		q[1] = 1;
		q[2] = 0;
		q[3] = 15;
		vp4_.push_back(q);

		q[0] = 15;
		q[1] = 2;
		q[2] = 5;
		q[3] = 20;
		vp4_.push_back(q);

	}

	void K2Equals2Tester() {
		// 
		MXCompactQtree a(vp_, NULL, 2, 2, 0, 0);
		EXPECT_EQ(0, a.code(vp_[0], 0));
		EXPECT_EQ(3, a.code(vp_[1], 0));
		EXPECT_EQ(3, a.code(vp_[2], 0));

		EXPECT_EQ(3, a.code(vp_[0], 1));
		EXPECT_EQ(5, a.code(vp_[1], 1));
		EXPECT_EQ(5, a.code(vp_[2], 1));

		EXPECT_EQ(5, a.code(vp_[0], 2));
		EXPECT_EQ(0, a.code(vp_[1], 2));
		EXPECT_EQ(7, a.code(vp_[2], 2));
	}

	void K1EqualsK2Equals2Tester() {
		// 
		MXCompactQtree a(vp_, NULL, 2, 2, 1, 0);
		EXPECT_EQ(0, a.code(vp_[0], 0));
		EXPECT_EQ(3, a.code(vp_[1], 0));
		EXPECT_EQ(3, a.code(vp_[2], 0));

		EXPECT_EQ(3, a.code(vp_[0], 1));
		EXPECT_EQ(5, a.code(vp_[1], 1));
		EXPECT_EQ(5, a.code(vp_[2], 1));

		EXPECT_EQ(5, a.code(vp_[0], 2));
		EXPECT_EQ(0, a.code(vp_[1], 2));
		EXPECT_EQ(7, a.code(vp_[2], 2));
	}

	void K1Equals4K2Equals2Tester() {
		// 
		MXCompactQtree a(vp_, NULL, 4, 2, 1, 0);
		EXPECT_EQ(5, a.code(vp_[0], 0));
		EXPECT_EQ(27, a.code(vp_[1], 0));
		EXPECT_EQ(27, a.code(vp_[2], 0));

		EXPECT_EQ(5, a.code(vp_[0], 1));
		EXPECT_EQ(0, a.code(vp_[1], 1));
		EXPECT_EQ(7, a.code(vp_[2], 1));
	}

	void K1EqualsK2Equals4Tester() {
		// 
		MXCompactQtree a(vp_, NULL, 4, 2, 2, 0);
		EXPECT_EQ(0, a.code(vp_[0], 0));
		EXPECT_EQ(5, a.code(vp_[1], 0));
		EXPECT_EQ(5, a.code(vp_[2], 0));

		EXPECT_EQ(27, a.code(vp_[0], 1));
		EXPECT_EQ(34, a.code(vp_[1], 1));
		EXPECT_EQ(55, a.code(vp_[2], 1));
	}

	void K2Equals2KiTester() {
		// 
		MXCompactQtree a(vp_, NULL, 2, 2, 0, 2);
		EXPECT_EQ(0, a.code(vp_[0], 0));
		EXPECT_EQ(3, a.code(vp_[1], 0));
		EXPECT_EQ(3, a.code(vp_[2], 0));

		EXPECT_EQ(3, a.code(vp_[0], 1));
		EXPECT_EQ(5, a.code(vp_[1], 1));
		EXPECT_EQ(5, a.code(vp_[2], 1));

		EXPECT_EQ(2, a.code(vp_[0], 2));
		EXPECT_EQ(0, a.code(vp_[1], 2));
		EXPECT_EQ(3, a.code(vp_[2], 2));

		EXPECT_EQ(1, a.code(vp_[0], 3));
		EXPECT_EQ(0, a.code(vp_[1], 3));
		EXPECT_EQ(1, a.code(vp_[2], 3));
	}

	void K1EqualsK2Equals2KiTester() {
		// 
		MXCompactQtree a(vp_, NULL, 2, 2, 1, 2);
		EXPECT_EQ(0, a.code(vp_[0], 0));
		EXPECT_EQ(3, a.code(vp_[1], 0));
		EXPECT_EQ(3, a.code(vp_[2], 0));

		EXPECT_EQ(3, a.code(vp_[0], 1));
		EXPECT_EQ(5, a.code(vp_[1], 1));
		EXPECT_EQ(5, a.code(vp_[2], 1));

		EXPECT_EQ(2, a.code(vp_[0], 2));
		EXPECT_EQ(0, a.code(vp_[1], 2));
		EXPECT_EQ(3, a.code(vp_[2], 2));

		EXPECT_EQ(1, a.code(vp_[0], 3));
		EXPECT_EQ(0, a.code(vp_[1], 3));
		EXPECT_EQ(1, a.code(vp_[2], 3));
	}

	void Dim4K1Equals4K2Equals2Tester() {
		//
		MXCompactQtree a(vp4_, NULL, 4, 2, 1, 0);

		EXPECT_EQ(0, a.code(vp4_[0], 0));
		EXPECT_EQ(1, a.code(vp4_[1], 0));
		EXPECT_EQ(6, a.code(vp4_[2], 0));

	}

	vector<Point<uint> > vp_;
	vector<Point<uint> > vp4_;

};

TEST_F(CodeTest, K2Equals2) {
	K2Equals2Tester();
}

TEST_F(CodeTest, K1EqualsK2Equals2) {
	K1EqualsK2Equals2Tester();
}

TEST_F(CodeTest, K1Equals4K2Equals2) {
	K1Equals4K2Equals2Tester();
}

TEST_F(CodeTest, K1EqualsK2Equals4) {
	K1EqualsK2Equals4Tester();
}

TEST_F(CodeTest, K2Equals2Ki) {
	K2Equals2KiTester();
}

TEST_F(CodeTest, K1EqualsK2Equals2Ki) {
	K1EqualsK2Equals2KiTester();
}

TEST_F(CodeTest, Dim4K1Equals4K2Equals2) {
	Dim4K1Equals4K2Equals2Tester();
}

}  // namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
