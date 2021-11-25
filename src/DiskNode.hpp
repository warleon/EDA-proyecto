#pragma once

#include <array>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

template <class pool_t, size_t M>
class DiskNode;

template <class node_t, size_t size>
class Pool {
  friend class node_t;
  using id_t = size_t;
  using pos_t = size_t;

  std::array<node_t*, size> pool = {0};
  std::array<id_t, size> posToId = {0};
  std::unordered_map<id_t, pos_t> idToPos;
  std::bitset<size> dirty, clock;
  fs::path home, ext;

  // clock evict policy returns position to evict
  pos__t chooseToEvict() {
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
    count = ++count % size;
    return t;
  }

  // reads a node from disk and places it at pos if pos is dirty writes the
  // dirty node to disk
  node_t* fetchTo(id_t id, pos_t pos) {
    fs::path inPath(std::to_string(id) + ext);
    if (dirty[pos]) {
      write(pos);
      dirty.reset(pos);
    }
    std::ifstream is(home + path);
    is >> *pool[pos];
    return pool[pos];
  }
  void write(pos_t pos) {
    fs::path outPath(std::to_string(posToId[pos]) + ext);
    std::ofstream os(home + path);
    os << *pool[pos];
    os.close();
  }

 public:
  Pool(std::string s = "./", std::string e = ".node") : home(s), ext(e) {
    home += "nodes/";
    fs::create_directory(home);
    // populate pool with null nodes
    for (autoa& it : pool) {
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
    fs::path p(to_string(i) + ext);
    return fs::exists(home + p);
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
    }

    return node;
  }
  // create a file to hold a node
  size_t create() {
    id_t t = createId();
    fs::path path(std::to_string(t) + ext);

    std::ofstream file(home + path);
    file.close();

    return t;
  }
};

template <class pool_t, size_t M>
class DiskNode {
  using node_t = DiskNode<pool_t, M>;
  using id_t = typename pool_t::id_t;
  using pos_t = typename pool_t::pos_t;
  friend pool_t;
  static pool_t pool;
  // TODO
  std::array<id_t, M> sons;

 public:
  // TODO
  template <class os_t>
  friend os_t& operator<<(os_t& os, node_t& node) {
    return os;
  }
  template <class is_t>
  friend is_t& operator>>(is_t& is, node_t& node) {
    return is;
  }
};