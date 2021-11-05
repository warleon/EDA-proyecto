#pragma once
#include <cmath>

template <class T, class Coord_t, std::size_t N>
struct Point {
  using coord_t = Coord_t;
  using point_t = Point<T, coord_t, N>;
  T data;
  coord_t coords[N];

  Point(coord_t coords_[N]) {
    for (std::size_t i = 0; i < N; i++) {
      coords[i] = coords_[i];
    }
  }
  ~Point() = default;

  bool between(point_t a, point_t b) {
    for (std::size_t i = 0; i < N; i++) {
      if (coords[i] < std::min(a.coords[i], b.coords[i]) ||
          coords[i] > std::max(a.coords[i], b.coords[i]))
        return false;
    }
    return true;
  }
};