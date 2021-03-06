#pragma once

#include <cassert>
#include <vector>

template <class node_t>
class RTree {
  using bbox_t = typename node_t::bbox_t;
  using point_t = typename bbox_t::point_t;

  node_t* root;

 public:
  RTree() { root = new node_t; }
  ~RTree() {
    if (root) delete root;
  }

  bool insert(point_t p) {
    assert(!p.null() && root);
    if (root->null()) {               // only root could be null
      return root->box.tryInsert(p);  // should return true
    }
    root->insert(p);
    if (root->p) root = root->p;
    return true;
  }
  template <class os_t>
  friend os_t& operator<<(os_t& os, RTree<node_t>& n) {
    return os << n.root;
  }
};