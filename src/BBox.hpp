#pragma once
#include <cmath>

template <class Point_t>
struct BBox {
  using point_t = Point_t;
  using bbox_t = BBox<Point_t>;
  using coord_t = typename point_t::coord_t;

  point_t corners[2];
  point_t* content;

  BBox(point_t a, point_t b) {
    content = nullptr;
    corners[0] = a;
    corners[1] = b;
  }
  ~BBox() {
    if (content) delete content;
  }

  bool covers(point_t x) { return x.between(corners[0], corners[1]); }
  bool contains(bbox_t b) {
    return covers(b.corners[0]) && covers(b.corners[1]);
  }

  coord_t area() {
    coord_t r = 1;
    std::size_t n = corners[0].dim();
    for (std::size_t i = 0; i < n; i++) {
      r *= std::abs(corners[0][i] - corners[1][i]);
    }
    return r;
  }
};