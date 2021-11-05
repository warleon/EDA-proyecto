#pragma once

#include <vector>

template <class node_t>
class RTree {
  using bbox_t = typename node_t::bbox_t;
  using point_t = typename bbox_t::point_t;
  using pointSet_t = std::vector<point_t>;

  node_t* root;

 public:
  RTree() : root(nullptr) {}
  ~RTree() {}

  bool insert(point_t p) {
    // TODO
    return false;
  }
  pointSet_t getRegion(point_t a, point_t b) {
    pointSet_t result;
    // TODO
    return result;
  }
};