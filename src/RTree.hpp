#pragma once

#include <cassert>
#include <vector>

template <class node_t>
class RTree {
  using bbox_t = typename node_t::bbox_t;
  using point_t = typename bbox_t::point_t;
  using pointSet_t = std::vector<point_t>;

  node_t* root;

 public:
  RTree() { root = new node_t; }
  ~RTree() {}

  bool insert(point_t p) {
    if (root->null()) {                // only root could be null
      return root->box.tryInsert(p);  // should return true
    }
    return root->insert(p);
  }
  pointSet_t getRegion(point_t a, point_t b) {
    pointSet_t result;
    // TODO
    return result;
  }
};