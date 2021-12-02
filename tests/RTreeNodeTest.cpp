#include <gtest/gtest.h>

#include <BBox.hpp>
#include <DiskNode.hpp>
#include <Point.hpp>
#include <RTreeNode.hpp>

using point_t = Point<int, double, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = RTreeNode<bbox_t, 3>;
using dnode_t = DiskNode<500, bbox_t, 3>;
using pool_t = typename dnode_t::pool_t;

/*
TEST(RTreeNodeTest, TryAddTest) {}
TEST(RTreeNodeTest, TrySplitTest) {}
TEST(RTreeNodeTest, InsertTest) {}
TEST(RTreeNodeTest, IsLeafTest) {}
TEST(RTreeNodeTest, PrintTest) {}
TEST(RTreeNodeTest, DefaultTest) {}
*/

TEST(DiskNodeTest, DefaultTest) { dnode_t testNode[100]; }
