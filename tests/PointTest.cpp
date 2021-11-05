#include <gtest/gtest.h>

#include <Point.hpp>
#include <string>
using point_t = Point<std::string, double, 2>;

TEST(PointTest, test1) {
  point_t testPoint;
  ASSERT_TRUE(testPoint.between(point_t(), point_t()));
}