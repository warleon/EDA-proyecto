#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
struct entry {
  const char* fileName;
  size_t offset, size;

  entry() {}
  entry(const std::string& s, size_t a, size_t b)
      : fileName(s.c_str()), offset(a), size(b) {}

  std::string content() {
    assert(strlen(fileName) != 0);
    std::ifstream file(fileName);
    std::string value;
    value.resize(size);
    file.seekg(offset);
    file.read(&value[0], value.size());
    return value;
  }
};

class EntryManager {
  size_t count;
  std::vector<std::string> filenames;
  std::unordered_map<size_t, entry> entrys;

 public:
  EntryManager() { count = 0; };
  ~EntryManager(){};
  size_t add(std::string key, size_t pos, size_t size) {
    auto it = std::find(filenames.begin(), filenames.end(), key);
    if (it == filenames.end()) {
      filenames.push_back(key);
      entry ent(filenames.back(), pos, size);
      entrys.insert(std::make_pair(count++, ent));
    } else {
      entry ent(*it, pos, size);
      entrys.insert(std::make_pair(count++, ent));
    }
    return count - 1;
  }
  std::vector<std::pair<size_t, entry>> fetchAll() {
    std::vector<std::pair<size_t, entry>> result;
    for (auto it : entrys) {
      result.push_back(it);
    }
    return result;
  }
  entry get(size_t id) { return entrys[id]; }
  template <class os_t>
  friend os_t& operator<<(os_t& os, EntryManager e) {
    return os;
  }
};