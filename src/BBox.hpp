#pragma once
#include <cassert>
#include <cmath>
#include <limits>
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

  BBox() { assert(maxSize >= 3); }
  BBox(point_t a, point_t b) : content(0) {
    assert(maxSize >= 3);
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
  point_t center() {
    const size_t d = corners[0].dim();
    coord_t ce[d];
    for (size_t i = 0; i < d; i++) {
      ce[i] = (corners[0][i] + corners[1][i]) / 2;
    }
    return point_t(ce);
  }
  coord_t min(size_t d) {
    coord_t r;
    if (content.size()) {
      r = content[0][d];
      for (size_t i = 0; i < maxSize; i++) {
        if (content[i].null()) continue;
        if (content[i][d] < r) r = content[i][d];
      }
    } else {
      assert(!null());
      r = std::min(corners[0][d], corners[1][d]);
    }
    return r;
  }
  coord_t max(size_t d) {
    coord_t r;
    if (content.size()) {
      r = content[0][d];
      for (size_t i = 0; i < maxSize; i++) {
        if (content[i].null()) continue;
        if (content[i][d] > r) r = content[i][d];
      }
    } else {
      assert(!null());
      r = std::max(corners[0][d], corners[1][d]);
    }
    return r;
  }
  // overlap
  bool overlap(bbox_t b) {
    assert(!b.null());
    assert(!null());
    const size_t d = corners[0].dim();
    size_t overlap_ = 0;
    for (size_t i = 0; i < d; i++) {
      coord_t ma = std::min(corners[0][i], corners[1][i]);
      coord_t Ma = std::max(corners[0][i], corners[1][i]);
      coord_t mb = std::min(b.corners[0][i], b.corners[1][i]);
      coord_t Mb = std::max(b.corners[0][i], b.corners[1][i]);
      if (((mb <= ma && ma <= Mb) || (mb <= Ma && Ma <= Mb)) ||
          ((ma <= mb && mb <= Ma) || (ma <= Mb && Mb <= Ma))) {
        overlap_++;
      }
    }
    return overlap_ == d;
  }

  // dummy split
  void fixSplit(bbox_t* a, bbox_t* b) {
    assert(a->isFull() || b->isFull());
    bbox_t* full;
    bbox_t* empty;
    if (a->isFull()) {
      full = a;
      empty = b;
    } else {
      full = b;
      empty = a;
    }
    auto M = full->content.size();
    for (size_t i = 0; i < M / 2; i++) {
      empty->content[i] = full->content[i];
      full->content[i].nullify();
    }
  }
  //
  bbox_t trySplit(point_t p) {
    bbox_t nBox;
    if (!isFull()) {
      assert(tryInsert(p));
      return nBox;
    };
    // find largest distance
    size_t fartest[] = {0, 1};
    coord_t mDist = content[0].manDist(content[1]);
    for (size_t i = 2; i < maxSize; i++) {
      auto d0 = content[i].manDist(content[fartest[0]]);
      auto d1 = content[i].manDist(content[fartest[1]]);
      if (std::max(d0, d1) > mDist) {
        mDist = std::max(d0, d1);
        fartest[d0 > d1 ? 1 : 0] = i;
      }
    }

    point_t pivots[] = {content[fartest[0]], content[fartest[1]]};
    bool here = p.manDist(pivots[0]) < p.manDist(pivots[1]);
    // move points
    for (size_t i = 0; i < maxSize; i++) {
      if (content[i].manDist(pivots[0]) < content[i].manDist(pivots[1]))
        continue;
      assert(nBox.tryInsert(content[i]));
      content[i].nullify();
    }
    if (isFull() || nBox.isFull()) fixSplit(this, &nBox);
    if (here) {
      assert(tryInsert(p));
    } else {
      assert(nBox.tryInsert(p));
    }
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
  coord_t manDist(bbox_t& other) { return center().manDist(other.center()); }
  // return manhatan distance between p and fartest point of box
  coord_t manDist(point_t p) {
    assert(!null());
    coord_t dist = 0;
    coord_t t = 0;
    for (size_t i = 0; i < p.dim(); i++) {
      t = std::max(p.distAlong(corners[0], i), p.distAlong(corners[1], i));
      dist += t * t;
    }
    return std::sqrt(dist);
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
  template <class os_t>
  friend void toSVG(os_t& os, bbox_t& object, coord_t x, coord_t y,
                    coord_t width, coord_t height) {
    os << "<g data-corners=\""
       << "(" << object.corners[0][0] << "," << object.corners[0][1] << ")"
       << "(" << object.corners[1][0] << "," << object.corners[1][1] << ")\""
       << ">\n";
    // draw rectangle
    os << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width
       << "\" height=\"" << height
       << R"(" fill="none" stroke-width="1" stroke="green"/>)"
       << "\n";

    coord_t min0 = object.min(0), min1 = object.min(1), max0 = object.max(0),
            max1 = object.max(1);
    for (auto& p : object.content) {
      if (p.null()) continue;
      // draw points relative to the box dimensions
      coord_t cx = x + ((p[0] - min0) / (max0 - min0)) * width;
      coord_t cy = y + ((p[1] - min1) / (max1 - min1)) * height;
      os << "<circle cx=\"" << cx << +"\" cy=\"" << cy << "\" data-coords=\""
         << "(" << p[0] << "," << p[1] << ")"
         << R"(" r="5" stroke-width="5" stroke="red"/>)"
         << "\n";
    }
    os << "</g>\n";
  }
};