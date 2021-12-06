#pragma once
#include <cassert>
#include <cmath>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

template <class Point_t, size_t maxSize>
struct BBox {
  using point_t = Point_t;
  using bbox_t = BBox<Point_t, maxSize>;
  using coord_t = typename point_t::coord_t;

 public:
  point_t corners[2];
  std::vector<point_t> content;

  BBox() { assert(maxSize); }
  BBox(point_t a, point_t b) {
    assert(maxSize);
    corners[0] = a;
    corners[1] = b;
  }
  ~BBox() {}
  bool null() { return corners[0].null() || corners[1].null(); }
  void resize(point_t a, point_t b) {
    assert(!content.size());
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
    if (!content.size()) return false;
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
    if (!content.size()) content.resize(maxSize);
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
    assert(content.size());
    coord_t r = content[0][d];
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].null()) continue;
      if (content[i][d] < r) r = content[i][d];
    }
    return r;
  }
  coord_t max(size_t d) {
    assert(content.size());
    coord_t r = content[0][d];
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].null()) continue;
      if (content[i][d] > r) r = content[i][d];
    }
    return r;
  }
  //overlap
  bool overlap(bbox_t b){
    assert(!b.null() && !null());
    const size_t d = corners[0].dim();
    bool overlap_ = false;
    for (size_t i = 0; i < d; i++)
    {
      coord_t ma = min(corners[0][d], corners[1][d]);
      coord_t Ma = max(corners[0][d], corners[1][d]);
      coord_t mb = min(b.corners[0][d], b.corners[1][d]);
      coord_t Mb = max(b.corners[0][d], b.corners[1][d]);
      if(
        ((mb <= ma && ma <= Mb)||(mb <= Ma && Ma <= Mb)) || 
        ((ma <= mb && mb <= Ma)||(ma <= Mb && Mb <= Ma)) )
      {
        overlap = true;
      }
    }
    return overlap_;
  }
  // dummy way
  bbox_t trySplit(point_t p) {
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
    size_t n = corners[0].dim();
    for (size_t i = 0; i < n; i++) {
      auto min = std::min(corners[0][i], corners[1][i]);
      auto max = std::max(corners[0][i], corners[1][i]);
      r *= max - min;  // diference always positive
    }
    return r;
  }
  coord_t area(point_t p) {
    coord_t r = 1;
    size_t n = corners[0].dim();
    for (size_t i = 0; i < n; i++) {
      auto min = std::min(std::min(corners[0][i], corners[1][i]), p[i]);
      auto max = std::max(std::max(corners[0][i], corners[1][i]), p[i]);
      r *= max - min;  // diference always positive
    }
    return r;
  }

  size_t size() { return maxSize; }

 public:
  // Json format output
  template <class os_t>
  friend os_t& operator<<(os_t& os, bbox_t& box) {
    json format = box;

    return os << format;
  }
  // Json format input
  template <class is_t>
  friend is_t& operator>>(is_t& is, bbox_t& box) {
    json format;
    is >> format;
    box = format;
    return is;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(bbox_t, corners, content)
};