#pragma once

#include <array>
#include <bitset>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

class DiskNode;

template <size_t size>
class Pool {
  std::array<DiskNode*, size> pool = {0};
  std::unordered_map<size_t, size_t> idToPos;
  std::bitset<size> dirty;
  fs::path home;

  size_t chooseToEvict() {
    for (size_t i = 0; i < size; i++) {
      if (!pool[i]) return i;
      // TODO evict policy
    }
    // TODO evict policy

    return 0;
  }

 public:
  Pool(std::string s = "./") : home(s) {
    home /= "nodes";
    FS::create_directory(home);
  }
  ~Pool() {}

  DiskNode* get(size_t id) {
    DiskNode* node = nullptr;

    return node;
  }
  void create() {
    // TODO
    return;
  }
};

class DiskNode {
  // TODO
 public:
  // TODO
};