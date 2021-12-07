#pragma once
#include <cassert>
#include <cmath>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

template <class T, class Coord_t, size_t N>
struct Point {
  using coord_t = Coord_t;
  using point_t = Point<T, coord_t, N>;

 private:
  bool null_;
  coord_t coords[N];

 public:
  T data;

  void setCoords(coord_t coords_[N]) {
    null_ = false;
    for (std::size_t i = 0; i < N; i++) {
      coords[i] = coords_[i];
    }
  }

  Point(coord_t coords_[N]) { setCoords(coords_); }
  Point(coord_t coords_[N], T d) : data(d) { setCoords(coords_); }

  Point() {
    null_ = true;
    // coords is trash
    for (auto& c : coords) {
      c = 0;
    }
  }

  ~Point() = default;
  coord_t operator[](size_t i) {
    assert(i < N);
    return coords[i];
  }
  /*
  point_t operator=(point_t p) {
    null_ = p.null_;
    data = p.data;
    for (std::size_t i = 0; i < N; i++) {
      coords[i] = p[i];
    }
    return *this;
  }
  */

  bool null() { return null_; }
  bool between(point_t a, point_t b) {
    for (size_t i = 0; i < N; i++) {
      if (coords[i] < std::min(a[i], b[i]) || coords[i] > std::max(a[i], b[i]))
        return false;
    }
    return true;
  }
  size_t dim() { return N; }
  coord_t distAlong(point_t x, size_t d) {
    assert(!x.null() && !null() && d < N);
    return std::abs(coords[d] - x.coords[d]);
  }
  coord_t manDist(point_t x) {
    coord_t dist = 0;
    for (size_t i = 0; i < N; i++) {
      dist += distAlong(x, i);
    }
    return dist;
  }

 public:
  // Json format output
  template <class os_t>
  friend os_t& operator<<(os_t& os, point_t& point) {
    json format = point;

    return os << format;
  }
  // Json format input
  template <class is_t>
  friend is_t& operator>>(is_t& is, point_t& point) {
    json format;
    is >> format;
    point = format;
    return is;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(point_t, null_, coords, data)
};
