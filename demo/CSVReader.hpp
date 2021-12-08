#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

class CSVReader {
  std::unordered_map<std::string, size_t> nameMap;
  std::ifstream file;
  char sep;
  size_t currentLineSize;

 public:
  std::vector<std::string> currentLine;
  using iterator = std::vector<std::string>::iterator;
  CSVReader(std::string filename, char sep_) : sep(sep_) {
    file.open(filename);
    next();
    // load name index
    for (size_t i = 0; i < currentLine.size(); ++i) {
      nameMap[currentLine[i]] = i;
    }
  }
  ~CSVReader() { file.close(); }
  std::string getline() {
    std::string line = "";
    for (auto& t : currentLine) {
      line += t + ", ";
    }
    return line.substr(0, line.length() - 2);
  }

  static std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> result;
    size_t from = 0, to = -1;
    do {
      from = to + 1;
      to = s.find(sep, from);
      result.push_back(s.substr(from, to - from));
    } while (to != std::string::npos);
    return result;
  }

  void next() {
    std::string line;
    std::getline(file, line);
    currentLine = split(line, sep);
    currentLineSize = line.size();
  }
  bool ok() { return file.good() && !file.eof(); }
  std::string operator()(const std::string& index) {
    return currentLine[nameMap[index]];
  }
  std::string operator[](size_t index) { return currentLine[index]; }
  size_t fileOffset() { return file.tellg(); }
  size_t size() { return currentLineSize; }
  iterator begin() { return currentLine.begin(); }
  iterator end() { return currentLine.end(); }
};
