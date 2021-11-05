#pragma once

template <class Point_t>
struct BBox {
  using point_t = Point_t;

  point_t corners[2];
  point_t* content;

  BBox(point_t a, point_t b) : content(nullptr) {
    corners[0] = a;
    corners[1] = b;
  }
  ~BBox() {
    if (content) delete content;
  }
};