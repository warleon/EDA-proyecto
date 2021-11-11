#pragma once
#include <cassert>
#include <cmath>

template <class Point_t>
struct BBox {
  using point_t = Point_t;
  using bbox_t = BBox<Point_t>;
  using coord_t = typename point_t::coord_t;

 private:
  size_t maxSize = 0;

 public:
  point_t corners[2];
  point_t* content;

  BBox() {}
  /*maybe not necesary yet
  BBox(bbox_t other) {
    maxSize = other.maxSize;
    corners[0].setCoords(other.corners[0]);
    corners[1].setCoords(other.corners[1]);
    if (other.content) {
    } else {
      content = nullptr;
    }
  }
  */
  BBox(point_t a, point_t b) {
    content = nullptr;
    corners[0] = a;
    corners[1] = b;
  }
  ~BBox() {
    if (content) delete content;
  }
  void setMaxSize(size_t s) { maxSize = s; }
  bool null() { return corners[0].null() || corners[1].null(); }
  void resize(point_t a, point_t b) {
    assert(!content);
    corners[0] = a;
    corners[1] = b;
  }
  void resize() {
    const size_t d = corners[0].dim();
    coord_t a[d], b[d];
    for (size_t i = 0; i < d; i++) {
      a[i] = min(i);
      b[i] = max(i);
    }

    corners[0].setCoords(a);
    corners[1].setCoords(b);
  }

  bool covers(point_t x) {
    assert(!x.null() && !null());
    return x.between(corners[0], corners[1]);
  }
  bool contains(bbox_t b) {
    assert(!b.null() && !null());
    return covers(b.corners[0]) && covers(b.corners[1]);
  }

  bool isFull() {
    assert(maxSize != 0);
    if (!content) return false;
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].null()) {
        return false;
      }
    }
    return true;
  }
  // tries to insert p in content but fails if is full
  bool tryInsert(point_t p) {
    assert(maxSize != 0);
    if (!content) content = new point_t[maxSize];
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].null()) {
        content[i] = p;
        resize();
        return true;
      }
    }

    return false;
  }
  coord_t min(size_t d) {
    assert(maxSize != 0 && content);
    coord_t r = content[0][d];
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].null()) continue;
      if (content[i][d] < r) r = content[i][d];
    }
    return r;
  }
  coord_t max(size_t d) {
    assert(maxSize != 0 && content);
    coord_t r = content[0][d];
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].null()) continue;
      if (content[i][d] > r) r = content[i][d];
    }
    return r;
  }
  // dummy way
  bbox_t trySplit(point_t p) {
    assert(maxSize);
    bbox_t nBox;
    if (!isFull()) {
      assert(tryInsert(p));
      return nBox;
    };
    for (size_t i = maxSize / 2; i < maxSize; i++) {
      assert(nBox.tryInsert(content[i]));
      content[i] = point_t();
    }
    assert(tryInsert(p));
    return nBox;
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