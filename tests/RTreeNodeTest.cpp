#include <gtest/gtest.h>

#include <BBox.hpp>
#include <DiskNode.hpp>
#include <Point.hpp>
#include <RTreeNode.hpp>

using point_t = Point<int, double, 2>;
using bbox_t = BBox<point_t, 3>;
using node_t = RTreeNode<bbox_t, 3>;

TEST(RTreeNodeTest, TryAddTest) {}
TEST(RTreeNodeTest, TrySplitTest) {}
TEST(RTreeNodeTest, InsertTest) {}
TEST(RTreeNodeTest, IsLeafTest) {}
TEST(RTreeNodeTest, PrintTest) {}
TEST(RTreeNodeTest, DefaultTest) {}

TEST(DiskNodeTest, DefaultTest) {}
