#include <gtest/gtest.h>

#include <BBox.hpp>
#include <Point.hpp>
#include <RTree.hpp>
#include <RTreeNode.hpp>
#include <string>

using point_t = Point<std::string, double, 2>;
using bbox_t = BBox<point_t>;
using node_t = RTreeNode<bbox_t, 3>;
using rtree_t = RTree<node_t>;

// Demonstrate some basic assertions.
TEST(RTreeTest, test1) {
  rtree_t testTree;
  double p[2] = {5, 5};
  ASSERT_TRUE(testTree.insert(point_t(p))) << "The insertion went wrong";
}