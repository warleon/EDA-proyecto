#pragma once

#include <cassert>
#include <cmath>

template <class BBox_t, size_t M>
struct RTreeNode {
  using bbox_t = BBox_t;
  using point_t = typename bbox_t::point_t;
  using coord_t = typename bbox_t::coord_t;
  using node_t = RTreeNode<bbox_t, M>;

  bbox_t box;
  node_t* p = nullptr;  // parent for recursive spliting
  node_t* sons[M] = {0};

  RTreeNode() { box.setMaxSize(M); }
  RTreeNode(bbox_t b) : box(b) { b.setMaxSize(M); }

  bool null() { return box.null(); }
  bool isLeaf() {
    assert(!box.null());
    return box.content;
  }
  bool isFull() {
    assert(!null());
    if (!isLeaf()) {
      for (size_t i = 0; i < M; i++) {
        if (!sons[i]) {
          return false;
        }
      }
      return true;
    } else
      return box.isFull();
  }

  size_t size() { return M; }
  coord_t min(size_t d) {
    assert(!null() && sons[0]);
    coord_t m =
        std::min(sons[0]->box.corners[0][d], sons[0]->box.corners[1][d]);
    coord_t curr = m;

    for (size_t i = 0; i < M; i++) {
      if (!sons[i]) continue;
      curr = std::min(sons[i]->box.corners[0][d], sons[i]->box.corners[1][d]);
      if (curr < m) m = curr;
    }
    return m;
  }
  coord_t max(size_t d) {
    assert(!null() && sons[0]);
    coord_t m =
        std::max(sons[0]->box.corners[0][d], sons[0]->box.corners[1][d]);
    coord_t curr = m;

    for (size_t i = 0; i < M; i++) {
      if (!sons[i]) continue;
      curr = std::max(sons[i]->box.corners[0][d], sons[i]->box.corners[1][d]);
      if (curr > m) m = curr;
    }
    return m;
  }
  void resize() {
    assert(!null());
    const size_t d = box.corners[0].dim();
    coord_t a[d], b[d];
    for (size_t i = 0; i < d; i++) {
      a[i] = min(i);
      b[i] = max(i);
    }
    box.resize(point_t(a), point_t(b));
  }
  bool tryAddNode(node_t* n) {
    for (size_t i = 0; i < M; i++) {
      if (!sons[i]) {
        sons[i] = n;
        n->p = this;
        return true;
      }
    }
    return false;
  }
  // dummy split
  void trySplit(node_t* n) {
    assert(!isLeaf());
    if (!n) return;
    if (!isFull()) {
      assert(tryAddNode(n));
      return;
    }
    node_t* nNode = new node_t();
    for (size_t i = M / 2, j = 0; i < M; i++, j++) {
      nNode->sons[j] = sons[i];
      nNode->sons[j]->p = nNode;
      sons[i] = nullptr;
    }
    assert(tryAddNode(n));
    resize();
    nNode->resize();

    if (p) {
      p->trySplit(nNode);
    } else {
      p = new node_t();
      p->trySplit(this);
      p->trySplit(nNode);
    }
    nNode->p = p;
    return;
  }
  bool insert(point_t point) {
    assert(!point.null());
    if (!isLeaf()) {
      for (size_t i = 0; i < size(); i++) {
        if (!sons[i]) continue;
        if (sons[i]->box.covers(point)) {
          sons[i]->insert(point);
          resize();
          return true;
        }
      }
      // TODO
      // Find a BB in n such that:
      // Enlarging BB to contain the object will add
      // the least amount of area to the bounding box BB
      // dummy way: just insert in a non full region
      for (size_t i = 0; i < M; i++) {
        if (!sons[i] || sons[i]->isFull()) continue;
        sons[i]->insert(point);
        resize();
        return true;
      }
      // since is internal node shouldnt reach this line
    } else {
      bbox_t b = box.trySplit(point);
      if (b.null()) return true;
      node_t* nLeaf = new node_t(b);
      if (p)
        p->trySplit(nLeaf);
      else {
        p = new node_t();
        p->trySplit(this);
        p->trySplit(nLeaf);
      }
      return true;

      // TODO
      //  Split the leaf node into 2 nodes;
      // Find the bounding box for all the objects in each node;
      // Replace the parent's (1) BB by of the 2 bounding boxes;
      // (The parent node can overflow and split.
      //  You need a Insert procedure into an internal node
      //  - just like the B-tree algorithm)
    }
    return false;  //
  }

  // RTreeNode* operator[](size_t i) { return sons[i]; }
};