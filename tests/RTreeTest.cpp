#include <gtest/gtest.h>

#include <BBox.hpp>
#include <Point.hpp>
#include <RTree.hpp>
#include <RTreeNode.hpp>

using point_t = Point<int, double, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = RTreeNode<bbox_t, 3>;
using rtree_t = RTree<node_t>;

// Demonstrate some basic assertions.
TEST(RTreeTest, insertTest) {
  rtree_t testTree;
  double p[2] = {5, 5};
  for (size_t i = 0; i < 9; i++) {
     ASSERT_TRUE(testTree.insert(point_t(p))) << "The insertion went wrong";
  }
  ASSERT_TRUE(testTree.insert(point_t(p))) << "The insertion went wrong";
}
TEST(RTreeTest, DefaultTest) {}