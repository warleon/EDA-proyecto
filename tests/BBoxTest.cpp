#include <gtest/gtest.h>

#include <BBox.hpp>
#include <Point.hpp>
#include <iostream>
using point_t = Point<int, double, 2>;
using bbox_t = BBox<point_t, 3>;

double ps[][2] = {{0, 0}, {10, 10}, {5, 5}, {15, 15}};

TEST(BBoxTest, AreaTest) {
  point_t a(ps[0]), b(ps[1]);
  bbox_t testBox(a, b);
  double r = testBox.area();
  ASSERT_EQ(r, 100.0);
}
TEST(BBoxTest, CoverTest) {
  point_t a(ps[0]), b(ps[1]), c(ps[2]), d(ps[3]);
  bbox_t boxA(a, b);
  EXPECT_TRUE(boxA.covers(c));
  EXPECT_FALSE(boxA.covers(d));
}
TEST(BBoxTest, ContainTest) {
  point_t a(ps[0]), b(ps[1]), c(ps[2]), d(ps[3]);
  bbox_t boxA(a, d), boxB(b, c);
  EXPECT_TRUE(boxA.contains(boxB));
  EXPECT_FALSE(boxB.contains(boxA));
}
TEST(BBoxTest, PointyBoxTest) {
  point_t a(ps[2]);
  bbox_t box(a, a);
  ASSERT_TRUE(box.covers(a));
}
TEST(BBoxTest, PrintTest) {
  point_t a(ps[0]), b(ps[1]), c(ps[2]), d(ps[3]);
  bbox_t box(a, b), box2;
  std::cerr << "box1" << box << "\n";
  std::cerr << "box2" << box2 << "\n";
  box.tryInsert(a);
  box.tryInsert(b);
  box.tryInsert(c);
  std::cerr << "box1" << box << "\n";
  std::cerr << "box2" << box2 << "\n";
  box2 = box.trySplit(d);
  std::cerr << "box1" << box << "\n";
  std::cerr << "box2" << box2 << "\n";
}
TEST(BBoxTest, DefaultTest) {}