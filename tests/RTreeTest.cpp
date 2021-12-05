#include <gtest/gtest.h>

#include <BBox.hpp>
#include <DiskNode.hpp>
#include <DiskRTree.hpp>
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

double ps[][2] = {{0, 0}, {10, 10}, {5, 5}, {15, 15}};

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
TEST(DiskRTreeTest, DefaultTest) { dtree_t testTree; }
TEST(DiskRTreeTest, insertTest1) {
  dtree_t testTree;
  double p[2] = {5, 5};
  for (size_t i = 0; i < 9; i++) {
    std::cerr << i << "\n";
    std::cerr << testTree << "\n";
    (testTree.insert(point_t(p)));
  }
  std::cerr << 9 << "\n";
  std::cerr << testTree << "\n";
  (testTree.insert(point_t(p)));
  std::cerr << 10 << "\n";
  std::cerr << testTree << "\n";
}

TEST(DiskRTreeTest, insertTest2) {
  point_t a(ps[0]), b(ps[1]), c(ps[2]), d(ps[3]);
  dtree_t testTree;
  std::cerr << testTree << "\n";
  testTree.insert(a);
  std::cerr << testTree << "\n";
  testTree.insert(b);
  std::cerr << testTree << "\n";
  testTree.insert(c);
  std::cerr << testTree << "\n";
  testTree.insert(d);
  std::cerr << testTree << "\n";
}