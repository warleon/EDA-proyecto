#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class EntryManager {
  using offSize_t = std::pair<size_t, size_t>;
  using offSize_set_t = std::vector<offSize_t>;
  std::unordered_map<std::string, offSize_set_t> filePtrs;

 public:
  EntryManager(){};
  ~EntryManager(){};

  void add(std::string key, size_t pos, size_t size) {
    offSize_t os = std::make_pair(pos, size);
    if (filePtrs.find(key) == filePtrs.end()) {
      filePtrs.insert(std::make_pair(key, offSize_set_t(1)));
    }
    filePtrs[key].push_back(os);
  }
  std::vector<std::string> FetchContent() {
    std::vector<std::string> result;

    for (const auto& entry : filePtrs) {
      std::ifstream file(entry.first);
      for (const auto& it : entry.second) {
        std::string s;
        s.resize(it.second);
        file.seekg(it.first);
        file.read(&s[0], s.size());
        result.push_back(s);
      }
    }

    return result;
  }
  template <class os_t>
  friend os_t& operator<<(os_t& os, EntryManager e) {
    for (const auto& entry : e.filePtrs) {
      os << entry.first << "{\n";
      std::ifstream file(entry.first);
      for (const auto& it : entry.second) {
        std::string s;
        s.resize(it.second);
        file.seekg(it.first);
        file.read(&s[0], s.size());
        os << s << "\n";
      }
    }
    return os << "}";
  }
};