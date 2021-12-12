#pragma once

#include <Pool.hpp>
#include <limits>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>
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
  using pointSet_t = std::vector<point_t>;

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
  bool isLeaf() { return box.content.size() /*&& hasSons() == MAX_SIZE*/; }
  // returns the position of the first non null son or MAX_SIZE if there isn't
  size_t hasSons() {
    for (size_t i = 0; i < M; i++) {
      if (sonsId[i]) return i;
    }
    return MAX_SIZE;
  }
  bool isFull() {
    if (isLeaf()) {
      return box.isFull();
    }

    for (size_t i = 0; i < M; i++) {
      if (!sonsId[i]) return false;
    }
    return true;
  }

  coord_t min(size_t d) {
    size_t first = hasSons();
    assert(first != MAX_SIZE);
    auto son0 = node_t::get(sonsId[first]);
    coord_t m = std::min(son0->box.corners[0][d], son0->box.corners[1][d]);
    coord_t curr = m;

    for (size_t i = first; i < M; i++) {
      if (!sonsId[i]) continue;
      auto son = node_t::get(sonsId[i]);
      curr = std::min(son->box.corners[0][d], son->box.corners[1][d]);
      if (curr < m) m = curr;
    }
    return m;
  }
  coord_t max(size_t d) {
    size_t first = hasSons();
    assert(first != MAX_SIZE);
    auto son0 = node_t::get(sonsId[first]);
    coord_t m = std::max(son0->box.corners[0][d], son0->box.corners[1][d]);
    coord_t curr = m;

    for (size_t i = first; i < M; i++) {
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
    // serach min and max points between sons
    for (size_t i = 0; i < d; i++) {
      a[i] = min(i);
      b[i] = max(i);
    }
    point_t pa(a), pb(b);
    box.resize(pa, pb);
  }
  id_t split(point_t p) {
    assert(isFull());
    assert(isLeaf());
    auto node = node_t::get(0);
    assert(node);
    node->setDirty();
    node->box = box.trySplit(p);
    assert(!node->box.null());
    node->parentId = parentId;
    return node->selfId;
  }

  id_t split(id_t id) {
    assert(id);
    assert(isFull());
    assert(!isLeaf());
    node_t* node = node_t::get(0);
    assert(node);
    // begin split algorithm
    // find largest distance
    // size_t fartest[] = {0, 1};
    node_t* fnode[] = {node_t::get(sonsId[0]), node_t::get(sonsId[1])};
    coord_t mDist = fnode[0]->box.manDist(fnode[1]->box);
    node_t* cnode = nullptr;
    coord_t d0;
    coord_t d1;
    for (size_t i = 0; i < M; i++) {
      cnode = node_t::get(sonsId[i]);
      d0 = cnode->box.manDist(fnode[0]->box);
      d1 = cnode->box.manDist(fnode[1]->box);
      if (std::max(d0, d1) > mDist) {
        mDist = std::max(d0, d1);
        // fartest[d0 > d1 ? 1 : 0] = i;
        fnode[d0 > d1 ? 1 : 0] = cnode;
      }
    }
    bool here = node_t::get(id)->box.manDist(fnode[0]->box) <
                node_t::get(id)->box.manDist(fnode[1]->box);
    // move points
    for (size_t i = 0; i < M; i++) {
      cnode = node_t::get(sonsId[i]);
      d0 = cnode->box.manDist(fnode[0]->box);
      d1 = cnode->box.manDist(fnode[1]->box);
      if (d0 < d1) continue;
      node->add(sonsId[i]);
      sonsId[i] = 0;
    }
    assert(!isFull());
    assert(!node->isFull());
    if (here) {
      add(id);
    } else {
      node->add(id);
    }
    // end of split algorithm
    resize();
    node->resize();
    node->parentId = parentId;

    return node->selfId;
  }
  void add(id_t node) {
    assert(!isFull());
    assert(!isLeaf());
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
    assert(!isFull());
    assert(isLeaf());
    setDirty();
    box.tryInsert(p);
  }

  bool null() { return box.null(); }

  // returns new id if the insert causes an split else returns null id(0)
  id_t insert(const point_t& p) {
    id_t sId = 0;
    if (isLeaf()) {
      if (!isFull()) {
        add(p);
      } else {
        return split(p);
      }
    } else {  // since is nonLeaf it has at least one child
              // find minimun enlargment area
      node_t* chosen = nullptr;
      for (auto sonId : sonsId) {
        if (!sonId) continue;
        auto son = node_t::get(sonId);
        if (!chosen) chosen = son;
        auto cA = chosen->box.area();
        auto sA = son->box.area();
        auto cAp = chosen->box.area(p);
        auto sAp = son->box.area(p);
        if ((sAp - sA) < (cAp - cA)) {
          chosen = son;
        }
      }
      assert(chosen);
      sId = chosen->insert(p);
    }
    if (sId) {              // if son node has splited
      if (isFull())         // and current node is full
        return split(sId);  // split current node and comunicate to parent node
      else                  // if not full
        add(sId);           // just add the new node
    }
    resize();
    return 0;  // comunicate to parent that current node has not been splited
  }

  void getRegion(pointSet_t& result, bbox_t& region) {
    if (isLeaf()) {
      for (auto& p : box.content) {
        if (p.null()) continue;
        if (region.covers(p)) {
          result.push_back(p);
        }
      }
    } else {
      for (auto& sId : sonsId) {
        if (!sId) continue;
        auto son = node_t::get(sId);
        assert(son);
        if (son->box.overlap(region)) {
          son->getRegion(result, region);
        }
      }
    }
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
  template <class os_t>
  friend void toSVG(os_t& os, node_t& object, coord_t x, coord_t y,
                    coord_t width, coord_t height) {
    // draw box
    if (object.isLeaf())
      toSVG(os, object.box, x, y, width, height);
    else {
      coord_t min0 = object.min(0), min1 = object.min(1);
      coord_t max0 = object.max(0), max1 = object.max(1);
      os << "<g data-corners=\""
         << "(" << min0 << "," << min1 << ")"
         << "(" << min1 << "," << max1 << ")\""
         << ">\n";
      for (auto& sId : object.sonsId) {
        if (!sId) continue;
        // draw children boxes with sizes relative to this box
        auto son = *node_t::get(sId);
        coord_t smin0 = son.box.min(0), smin1 = son.box.min(1);
        coord_t smax0 = son.box.max(0), smax1 = son.box.max(1);
        coord_t rX = x + ((smin0 - min0) / (max0 - min0)) * width;
        coord_t rY = y + ((smin1 - min1) / (max1 - min1)) * height;
        coord_t rW = ((smax0 - smin0) / (max0 - min0)) * width;
        coord_t rH = ((smax1 - smin1) / (max1 - min1)) * height;
        toSVG(os, son, rX, rY, rW, rH);
      }
      os << "</g>\n";
    }
  }
};
