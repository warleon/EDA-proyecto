#pragma once

template <class T, class coord_t, std::size_t N>
struct Point {
  using point_t = Point<T, coord_t, N>;
  T data;
  coord_t coords[N];

  Point() = default;
  ~Point() = default;

  bool between(point_t a, point_t b) { return false; }
};