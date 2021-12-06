#pragma once

#include <Pool.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
using json = nlohmann::json;

#define MAX_SIZE size_t(-1)

template <size_t pool_size, class BBox_t, size_t M>
struct DiskNode {
  using bbox_t = BBox_t;
  using point_t = typename bbox_t::point_t;
  using coord_t = typename point_t::coord_t;
  using node_t = DiskNode<pool_size, bbox_t, M>;
  using pool_t = Pool<node_t, pool_size>;
  using id_t = typename pool_t::id_t;
  using pos_t = typename pool_t::pos_t;

  friend pool_t;

  inline static pool_t pool;
  bbox_t box;
  id_t parentId;
  id_t selfId;
  std::array<id_t, M> sonsId = {0};

  void setDirty() {
    assert(selfId);
    node_t::pool.dirty.set(node_t::pool.idToPos[selfId]);
  }
  void setUsed() {
    assert(selfId);
    node_t::pool.clock.set(node_t::pool.idToPos[selfId]);
  }

  static node_t* get(size_t id) {
    auto node = node_t::pool.get(id);
    assert(node);
    node->selfId = id;
    node->setUsed();
    return node;
  }
  ~DiskNode() {}
  DiskNode() {
    selfId = 0;
    parentId = 0;
  }
  id_t id() { return selfId; }
  bool isLeaf() { return box.content.size() /* && hasSons() == MAX_SIZE*/; }
  size_t hasSons() {
    for (size_t i = 0; i < M; i++) {
      if (!sonsId[i]) return i;
    }
    return MAX_SIZE;
  }
  bool isFull() {
    if (isLeaf()) {
      return box.isFull();
    }
    return hasSons() != MAX_SIZE;
  }

  coord_t min(size_t d) {
    assert(hasSons() != MAX_SIZE);
    auto son0 = node_t::get(sonsId[0]);
    coord_t m = std::min(son0->box.corners[0][d], son0->box.corners[1][d]);
    coord_t curr = m;

    for (size_t i = 0; i < M; i++) {
      if (!sonsId[i]) continue;
      auto son = node_t::get(sonsId[i]);
      curr = std::min(son->box.corners[0][d], son->box.corners[1][d]);
      if (curr > m) m = curr;
    }
    return m;
  }
  coord_t max(size_t d) {
    assert(hasSons() != MAX_SIZE);
    auto son0 = node_t::get(sonsId[0]);
    coord_t m = std::max(son0->box.corners[0][d], son0->box.corners[1][d]);
    coord_t curr = m;

    for (size_t i = 0; i < M; i++) {
      if (!sonsId[i]) continue;
      auto son = node_t::get(sonsId[i]);
      curr = std::max(son->box.corners[0][d], son->box.corners[1][d]);
      if (curr > m) m = curr;
    }
    return m;
  }
  void resize() {
    setDirty();
    if (isLeaf()) return box.resize();
    const size_t d = box.corners[0].dim();
    coord_t a[d], b[d];
    for (size_t i = 0; i < d; i++) {
      a[i] = min(i);
      b[i] = max(i);
    }
    box.resize(point_t(a), point_t(b));
  }
  id_t split(point_t p) {
    assert(isFull());
    auto node = node_t::get(0);
    assert(node);
    node->setDirty();
    node->box = box.trySplit(p);
    node->parentId = parentId;
    return node->selfId;
  }
  id_t split(id_t id) {
    assert(isFull());
    node_t* node = node_t::get(0);
    assert(node);
    node->setDirty();
    // dummy split algorithm
    for (size_t i = M / 2, j = 0; i < M; i++, j++) {
      node->sonsId[j] = sonsId[i];
      node_t::get(node->sonsId[j])->parentId = node->selfId;
      sonsId[i] = 0;
    }
    add(id);
    resize();
    node->resize();
    node->parentId = parentId;

    return node->selfId;
  }
  void add(id_t node) {
    assert(!isFull());
    setDirty();
    for (size_t i = 0; i < M; i++) {
      if (!sonsId[i]) {
        sonsId[i] = node;
        node_t::get(node)->parentId = selfId;
        return;
      }
    }
  }
  void add(point_t p) {
    assert(!box.isFull());
    setDirty();
    box.tryInsert(p);
  }

  bool null() { return box.null(); }

  // returns new id if the insert causes an split else returns null id(0)
  id_t insert(const point_t& p) {
    bool resizeNeeded = false;
    id_t sId = 0;
    if (isLeaf()) {
      if (!isFull()) {
        if (box.area(p) != box.area()) resizeNeeded = true;
        add(p);
      } else {
        sId = split(p);
      }
    } else {  // since is nonLeaf it has at least one child
      // get minimum Area enlargment bbox
      node_t* chosen = nullptr;
      for (auto sonId : sonsId) {
        if (!sonId) continue;
        auto son = node_t::get(sonId);
        if (!chosen || son->box.area(p) < chosen->box.area(p)) {
          chosen = son;
        }
      }
      if (chosen->box.area(p) != chosen->box.area()) resizeNeeded = true;
      sId = chosen->insert(p);
    }
    // resize after insertion
    if (resizeNeeded) {
      resize();
    }
    if (sId) {              // if son node has splited
      if (isFull())         // and current node is full
        return split(sId);  // split current node and comunicate to parent node
      else                  // if not full
        add(sId);           // just add the new node
    }
    return 0;  // comunicate to parent that current node has not been splited
  }

 public:
  // Json format output
  template <class os_t>
  friend os_t& operator<<(os_t& os, node_t& node) {
    json format = node;
    os << format;
    return os;
  }
  // Json format input
  template <class is_t>
  friend is_t& operator>>(is_t& is, node_t& node) {
    json format;
    is >> format;
    node = format;
    return is;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(node_t, box, parentId, selfId, sonsId)
};
