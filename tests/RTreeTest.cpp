#include <gtest/gtest.h>

#include <BBox.hpp>
#include <DiskNode.hpp>
#include <Point.hpp>
#include <RTree.hpp>
#include <RTreeNode.hpp>
#include <iostream>

using point_t = Point<int, double, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = RTreeNode<bbox_t, 3>;
using rtree_t = RTree<node_t>;
using dnode_t = DiskNode<500, bbox_t, 3>;
using pool_t = typename dnode_t::pool_t;
using dtree_t = DiskRTree<dnode_t>;

// Demonstrate some basic assertions.
TEST(RTreeTest, insertTest) {
  rtree_t testTree;
  double p[2] = {5, 5};
  for (size_t i = 0; i < 9; i++) {
    std::cerr << i << "\n";
    std::cerr << testTree << "\n";
    ASSERT_TRUE(testTree.insert(point_t(p))) << "The insertion went wrong";
  }
  std::cerr << 9 << "\n";
  std::cerr << testTree << "\n";
  ASSERT_TRUE(testTree.insert(point_t(p))) << "The insertion went wrong";
  std::cerr << 10 << "\n";
  std::cerr << testTree << "\n";
}
TEST(RTreeTest, DefaultTest) { dtree_t testTree; }