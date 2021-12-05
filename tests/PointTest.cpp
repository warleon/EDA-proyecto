#include <gtest/gtest.h>

#include <Point.hpp>
#include <iostream>
#include <sstream>
#include <string>
using point_t = Point<int, double, 2>;

double points[][2] = {{5, 5}, {0, 0}, {10, 10}};
TEST(PointTest, betweenTest) {
  point_t p1(points[0]);
  point_t p2(points[1]);
  point_t p3(points[2]);
  bool r = p1.between(p2, p3);
  bool r1 = p1.between(p3, p2);
  bool r2 = p2.between(p1, p3);
  EXPECT_TRUE(r);
  EXPECT_TRUE(r1);
  EXPECT_FALSE(r2);
}
TEST(PointTest, PrintTest) {
  point_t p1(points[0]);
  std::cerr << p1 << "\n";
}
TEST(PointTest, JsonTest) {
  point_t tPoint;
  std::string json = R"(
    {
      "null_":false,
      "coords":[1,2],
      "data": 4
    }
  )";
  std::stringstream ss;
  ss << json;
  ss >> tPoint;
  std::cerr << "\n";
  std::cerr << tPoint;
  std::cerr << "\n";
}
TEST(PointTest, DefaultTest) {}