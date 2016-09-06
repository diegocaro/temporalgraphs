#include "point.h"
#include "gtest/gtest.h"

using namespace cqtree_utils;

namespace {

TEST(point_int_3d, constructor_dims) {
	Point<uint> p(3);
	EXPECT_EQ(3, p.num_dims());

	p[0] = 1;
	p[1] = 2;
	p[2] = 3;

	EXPECT_EQ(1u, p[0]);
	EXPECT_EQ(2u, p[1]);
	EXPECT_EQ(3u, p[2]);

	EXPECT_EQ(1u, p.comp(0));
	EXPECT_EQ(2u, p.comp(1));
	EXPECT_EQ(3u, p.comp(2));
}

//TEST(point_int_3d, destructor)
//{
//  	Point<uint> *p = new Point<uint>(3);
//
//	delete p;
//
//	EXPECT_EQ(0, p->num_dims());
//}

TEST(point_int_3d, constructor_copy) {
	Point<uint> p(3);
	p[0] = 1;
	p[1] = 2;
	p[2] = 3;

	Point<uint> q(p);

	EXPECT_EQ(3, q.num_dims());

	EXPECT_EQ(1u, q[0]);
	EXPECT_EQ(2u, q[1]);
	EXPECT_EQ(3u, q[2]);
}

TEST(point_int_3d, equal) {
	Point<uint> p(3);
	p[0] = 1;
	p[1] = 2;
	p[2] = 3;

	Point<uint> q(p);
	EXPECT_EQ(true, q.num_dims() == p.num_dims());

	EXPECT_EQ(true, q[0] == p[0]);
	EXPECT_EQ(true, q[1] == p[1]);
	EXPECT_EQ(true, q[2] == p[2]);
}

TEST(point_int_3d, less) {
	Point<uint> p(3);
	p[0] = 1;
	p[1] = 2;
	p[2] = 3;

	Point<uint> q(3);
	q[0] = 0;
	q[1] = 1;
	q[2] = 1;

	EXPECT_EQ(true, q < p);
}

TEST(point_int_3d, assignment) {
	Point<uint> p(3);
	p[0] = 1;
	p[1] = 2;
	p[2] = 3;

	Point<uint> q;

	p = q;

	EXPECT_EQ(true, q == p);
}

}  // namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
