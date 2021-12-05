#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define MAX_SIZE size_t(-1)

namespace fs = std::filesystem;

template <class node_t, size_t size>
class Pool {
  friend node_t;
  using id_t = size_t;
  using pos_t = size_t;

  std::array<node_t*, size> pool = {0};
  std::array<id_t, size> posToId = {0};
  std::unordered_map<id_t, pos_t> idToPos;
  std::bitset<size> dirty, clock;
  fs::path home, ext;

  // clock evict policy returns position to evict
  pos_t chooseToEvict() {
    pos_t t;
    while (true) {
      t = tick();
      if (clock[t]) {
        clock.reset(t);
      } else {
        return t;
      }
    }
    return 0;
  }

  // creates a new id
  id_t createId() {
    static id_t count = 1;
    return count++;
  }
  // advances the clock counter
  pos_t tick() {
    static pos_t count = 0;
    pos_t t = count;
    count = (count + 1) % size;
    return t;
  }

  // reads a node from disk and places it at pos if pos is dirty writes the
  // dirty node to disk
  node_t* fetchTo(id_t id, pos_t pos) {
    if (dirty[pos]) {
      write(pos);
      dirty.reset(pos);
    }
    fs::path inPath(fs::path(std::to_string(id)) / ext);
    std::ifstream is(home / inPath);
    is >> *pool[pos];
    return pool[pos];
  }
  void write(pos_t pos) {
    fs::path outPath(std::to_string(posToId[pos]) + ext.string());
    std::ofstream os(home / outPath);
    os << *pool[pos];
    os.close();
  }

 public:
  Pool(std::string s = ".", std::string e = ".node") : home(s), ext(e) {
    home /= "nodes";
    fs::create_directory(home);
    // populate pool with null nodes
    for (auto& it : pool) {
      it = new node_t;
    }
  }
  ~Pool() {
    for (size_t i = 0; i < size; i++) {
      if (dirty[i]) {
        write(i);
        dirty.reset(i);
      }
      delete pool[i];
      pool[i] = nullptr;
    }
  }

  bool exists(id_t i) {
    fs::path p(fs::path(std::to_string(i)) / ext);
    return fs::exists(home / p);
  }

  // gets a pointer to the node with id
  node_t* get(id_t& id) {
    node_t* node = nullptr;
    if (id) {
      if (idToPos.find(id) != idToPos.end()) {  // if node in pool
        clock.set(idToPos[id]);
        node = pool.at(idToPos[id]);
      } else {  // evict a page to get it
        pos_t e = chooseToEvict();
        id_t i = posToId[e];
        idToPos.erase(i);
        idToPos[id] = e;
        node = fetchTo(id, e);
        posToId[e] = id;
      }
    } else {  // when null id create node
      id = create();
      return get(id);
    }

    return node;
  }
  // create a file to hold a node
  size_t create() {
    id_t t = createId();
    fs::path path(fs::path(std::to_string(t)) / ext);

    std::ofstream file(home / path);
    file.close();

    return t;
  }
};

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

  void setDirty() { node_t::pool.dirty.set(node_t::pool.idToPos[selfId]); }

  static node_t* get(size_t id) {
    auto node = node_t::pool.get(id);
    assert(node);
    node->selfId = id;
    return node;
  }
  ~DiskNode() {}
  DiskNode() {
    selfId = 0;
    parentId = 0;
  }
  id_t id() { return selfId; }
  bool isLeaf() { return box.content.size(); }
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
    const size_t d = box.corners[0].dim();
    if (isLeaf()) return box.resize();
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
    auto nbox = box.trySplit(p);
    node->box = nbox;
    return node->selfId;
  }
  id_t split(id_t id) {
    assert(isFull());
    node_t* node = node_t::get(0);
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
    for (size_t i = 0; i < M; i++) {
      if (!sonsId[i]) {
        sonsId[i] = node;
        return;
      }
    }
  }
  void add(point_t p) {
    assert(!box.isFull());
    box.tryInsert(p);
  }

  bool null() { return box.null(); }

  // returns new id if the insert causes an split else returns null id(0)
  id_t insert(const point_t& p) {
    bool resizeNeeded = false;
    id_t sId = 0;
    if (isLeaf()) {
      setDirty();
      if (!isFull()) {
        if (box.area(p) != box.area()) resizeNeeded = true;
        add(p);
      } else {
        sId = split(p);
      }
    } else {  // since is nonLeaf it has at least one child
      // get minimum Area enlargment bbox
      node_t* chosen = nullptr;
      for (auto sId : sonsId) {
        if (!sId) continue;
        auto son = node_t::get(sId);
        if (!chosen || son->box.area(p) < chosen->box.area(p)) {
          chosen = son;
        }
      }
      // check if box needs to be resized
      if (chosen->box.area(p) != chosen->box.area()) resizeNeeded = true;
      // resize after insertion
      sId = chosen->insert(p);
    }
    if (resizeNeeded) {
      setDirty();
      resize();
    }
    if (sId) {  // recursive split
      if (isFull())
        return split(sId);
      else
        add(sId);
    }
    return 0;
  }

  template <class os_t>
  friend os_t& operator<<(os_t& os, node_t& node) {
    os << node.selfId << "\n";
    os << node.parentId << "\n";
    os << node.box << "\n";
    for (auto& s : node.sonsId) {
      os << s << " ";
    }
    return os;
  }
  template <class is_t>
  friend is_t& operator>>(is_t& is, node_t& node) {
    node.setDirty();
    return is;
  }
};
