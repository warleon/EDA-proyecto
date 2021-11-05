#pragma once

template <class node_t>
class RTree {
  using bbox_t = typename node_t::bbox_t;
  using point_t = typename bbox_t::point_t;
  node_t* root;

 public:
  RTree() : root(nullptr) {}
  ~RTree() {}

  bool insert(point_t p) { return false; }
};